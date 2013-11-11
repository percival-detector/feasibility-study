/*
 * PercivalServer.h
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 *
 *  This class manages multiple UDP sockets and coordinates the reconstruction
 *  of full frames from any number of sub-frames.  Each sub-frame is defined by
 *  dimensions and UDP socket setup and is assigned an ID.  The UDP sockets are
 *  setup with asynchronous callbacks so no thread management is required; this
 *  class will simply execute a callback once a frame has been reconstructed or
 *  is considered corrupt.  This class will make a callback for an allocated
 *  frame when it requires one, and it is the reponsibility of the callback to
 *  free the memory once this class has either flagged the frame as complete or
 *  corrupt.
 */

#ifndef PERCIVALSERVER_H_
#define PERCIVALSERVER_H_

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include "PercivalDataType.h"
#include "PercivalSubFrame.h"
#include "DataReceiver.h"
#include "PercivalBuffer.h"
#include "PercivalBufferPool.h"
#include "PercivalPacketChecker.h"

typedef struct errorStats_t
{
  uint32_t missingPackets;
  uint32_t latePackets;
  uint32_t duplicatePackets;
  uint32_t incorrectSubFramePackets;
} ErrorStats;

class PercivalSubFrame;

class PercivalServer : public IPercivalCallback
{
  public:

	  PercivalServer();
    virtual ~PercivalServer();

    void setDebug(uint32_t level);

    void setToSpatialMode(uint32_t subFrameID);

    void setToTemporalMode();

    void setWatchdogTimeout(uint32_t time);

    std::string errorMessage();

    void setDescramble(uint32_t descramble);

    int getCpuGroup();

    void setCpuGroup(int cpuGroup);

    int setupFullFrame(uint32_t width,              // Width of full frame in pixels
                       uint32_t height,             // Height of full frame in pixels
                       DataType type,               // Data type (unsigned 8, 16 or 32 bit)
                       uint32_t *dataIndex,         // Index in output array of input data
                       uint32_t *ADCIndex,          // Index of ADC to use for each input data point
//                       uint32_t *ADCLowGain,        // Array of low gain ADC gains, one per ADC
//                       uint32_t *ADCHighGain,       // Array of high gain ADC gains, one per ADC
//                       uint32_t *ADCOffset,         // Combined offset for both ADC's
//                       uint32_t *stageGains,        // Gain to apply for each of the output stages (in scrambled order)
//                       uint32_t *stageOffsets);     // Offsets to apply for each of the output stages (in scrambled order)
                       float    *ADCLowGain,        // Array of low gain ADC gains, one per ADC
                       float    *ADCHighGain,       // Array of high gain ADC gains, one per ADC
                       float    *ADCOffset,         // Combined offset for both ADC's
                       float    *stageGains,        // Gain to apply for each of the output stages (in scrambled order)
                       float    *stageOffsets);     // Offsets to apply for each of the output stages (in scrambled order)

    int setupChannel(const std::string& host, unsigned short port, uint32_t packetSize);

    int setupSubFrame(uint32_t subFrameID,
                      uint32_t topLeftX,
                      uint32_t topLeftY,
                      uint32_t bottomRightX,
                      uint32_t bottomRightY);

    int releaseSubFrame(int subFrameID);

    int releaseAllSubFrames();

    int startAcquisition();

    int stopAcquisition();

    int readErrorStats(uint32_t *dupPkt,
                       uint32_t *misPkt,
                       uint32_t *ltePkt,
                       uint32_t *incPkt,
                       uint32_t *dupRPkt,
                       uint32_t *misRPkt,
                       uint32_t *lteRPkt,
                       uint32_t *incRPkt,
                       uint32_t *missingResetFrames);

    int readProcessTime(uint32_t *processTime, uint32_t *resetProcessTime, uint32_t *serviceTime);

    int registerCallback(IPercivalCallback *callback);

    virtual void imageReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType);

    void resetPacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType);

    void framePacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType);

    void processFrame(uint16_t frameNumber, PercivalBuffer *rawFrame);
    //void processFrame(uint16_t frameNumber);

    void temporalResetPacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType);

    void processTemporalResetFrame(uint16_t frameNumber);

    void temporalFramePacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType);

    void processTemporalFrame(uint16_t frameNumber, PercivalBuffer *subFrameBuffers[8]);
    //void processTemporalFrame(uint16_t frameNumber);

    virtual void timeout();

    virtual PercivalBuffer *allocateBuffer();

    virtual void releaseBuffer(PercivalBuffer *buffer);

    void unscramble(int      numPts,       // Number of points to process
                    uint16_t *in_data,     // Input data
                    uint16_t *reset_data,  // Reset data
                    float    *out_data,    // Output data
                    uint32_t x1,
                    uint32_t x2,
                    uint32_t y1);

    void unscrambleToFull(int      numPts,       // Number of points to process
                          uint16_t *in_data,     // Input data
                          uint16_t *reset_data,  // Reset data
                          float    *out_data,    // Output data
                          uint32_t x1,
                          uint32_t x2,
                          uint32_t y1);

  private:

    uint32_t       debug_;           // Debug level
    uint32_t       watchdogTimeout_; // Watchdog timeout (ms)
    std::string    errorMessage_;    // Error message string
    bool           acquiring_;       // Are we acquiring
    boost::mutex   access_;          // Mutex for packet locking
    boost::mutex   frameAccess_;     // Mutex for frame process locking
    boost::mutex   resetAccess_;     // Mutex for reset process locking
    uint32_t       descramble_;      // Should we (0 = reconstruct raw, 1 = full descramble, 2 = reorder only)
    std::string    host_;            // Host IP address to bind to
    unsigned short port_;            // Port to bind to
    uint32_t       packetSize_;      // Size of UDP packet payload in bytes
    uint32_t       mode_;            // Current mode, (0 = spatial, 1 = temporal)
    uint32_t       currentSubFrame_; // When in spatial mode this informs which subframe to use

long startTime_;

    uint32_t     width_;             // Width of full frame
    uint32_t     height_;            // Height of full frame
    DataType     type_;              // Data type of full frame
    uint32_t     pixelSize_;         // No of pixels per full frame
    uint32_t     byteSize_;          // No of bytes per full frame
    uint32_t     *dataIndex_;        // Pointer to array of unscrambled pixel locations
    uint32_t     *ADCIndex_;         // Pointer to array of ADCs for each pixel
//    uint32_t        *ADCLowGain_;       // Pointer to array of low gain ADC gains, one per ADC
//    uint32_t        *ADCHighGain_;      // Pointer to array of high gain ADC gains, one per ADC
//    uint32_t        *ADCOffset_;        // Pointer to combined offset for both ADC's
//    uint32_t        *stageGains_;       // Gain to apply for each of the output stages (in scrambled order)
//    uint32_t        *stageOffsets_;     // Offsets to apply for each of the output stages (in scrambled order)
    float        *ADCLowGain_;       // Pointer to array of low gain ADC gains, one per ADC
    float        *ADCHighGain_;      // Pointer to array of high gain ADC gains, one per ADC
    float        *ADCOffset_;        // Pointer to combined offset for both ADC's
    float        *stageGains_;       // Gain to apply for each of the output stages (in scrambled order)
    float        *stageOffsets_;     // Offsets to apply for each of the output stages (in scrambled order)
    boost::shared_ptr<boost::thread>  workerThread_;

    int                                 cpuGroup_;            // Used to pin 3 threads to cpuGroup_*3, +1, +2

    std::map<int, PercivalSubFrame *>   subFrameMap_;         // Map to contain sub-image objects

    uint32_t                            serverMask_;          // bitmask of servers setup
    uint32_t                            serverReady_;         // bitmask of servers that have notified of frames
    uint32_t                            frameNumber_;         // Current frame number
    uint32_t                            expectNewFrame_;      // Set when we have completed a frame
    uint32_t                            resetFrameNumber_;    // Current reset frame number
    uint32_t                            expectNewResetFrame_; // Set when we have completed a reset frame
    uint32_t                            resetFrameReady_;     // Set when we have completed a reset frame
    uint32_t                            processTime_;         // Process time in microseconds for a frame
    uint32_t                            serviceTime_;         // Service time in microseconds for a frame
    uint32_t                            resetProcessTime_;    // Process time in microseconds for a reset frame

    IPercivalCallback                   *callback_;           // Callback interface
    PercivalBuffer                      *fullFrame_;          // Full frame buffer
    PercivalBuffer                      *rawFrame_;           // Scrambled full frame buffer
    PercivalBuffer                      *processFrame_;       // Frame pointer used for descrambling
    PercivalBuffer                      *subFrames_[8];       // In temporal mode we need to store all subframes in buffers
    PercivalBuffer                      *subFrameBuffers_[8]; // In temporal mode we need to store all subframes in buffers
    PercivalBuffer                      *resetSubFrames_[8];  // In temporal mode we need to store all subframes in buffers
    PercivalBuffer                      *resetFrame1_;        // Buffer for reset frame as it is received
    std::map<uint16_t, PercivalBuffer*> resetFrameMap_;       // Map for storing reset frames
    DataReceiver                        *receiver_;           // Data receiver
    PercivalBufferPool                  *buffers_;            // Pool of individual UDP packet buffers
    PercivalBufferPool                  *sfBuffers_;          // Pool of buffers for subframes
    PercivalPacketChecker               *checker_;            // Keep a record of incoming packets
    PercivalPacketChecker               *resetChecker_;       // Keep a record of incoming reset packets
    ErrorStats                          errorStats_;          // Keep a record of error statistics
    ErrorStats                          resetErrorStats_;     // Keep a record of error statistics
    uint32_t                            resetFramesMissing_;  // Keep a record of the number of missing reset frames
};

#endif /* DATASENDER_H_ */

