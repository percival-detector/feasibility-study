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

#include "PercivalDataType.h"
#include "PercivalSubFrame.h"
#include "DataReceiver.h"
#include "PercivalBuffer.h"
#include "PercivalBufferPool.h"

class PercivalSubFrame;

class PercivalServer
{
  public:

	  PercivalServer();
    virtual ~PercivalServer();

    void setDebug(uint32_t level);

    void setWatchdogTimeout(uint32_t time);

    std::string errorMessage();

    void setDescramble(bool descramble);

    int setupFullFrame(uint32_t width,              // Width of full frame in pixels
                       uint32_t height,             // Height of full frame in pixels
                       DataType type,               // Data type (unsigned 8, 16 or 32 bit)
                       uint32_t *dataIndex,         // Index in output array of input data
                       uint32_t *ADCIndex,          // Index of ADC to use for each input data point
                       float    *ADCLowGain,        // Array of low gain ADC gains, one per ADC
                       float    *ADCHighGain,       // Array of high gain ADC gains, one per ADC
                       float    *ADCOffset,         // Combined offset for both ADC's
                       float    *stageGains,        // Gain to apply for each of the output stages (in scrambled order)
                       float    *stageOffsets);     // Offsets to apply for each of the output stages (in scrambled order)

    int setupSubFrame(uint32_t frameID,
                      const std::string& host,
                      unsigned short port,
                      uint32_t topLeftX,
                      uint32_t topLeftY,
                      uint32_t bottomRightX,
                      uint32_t bottomRightY,
                      uint32_t subFrames);

    int releaseSubFrame(int frameID);

    int releaseAllSubFrames();

    int startAcquisition();

    int stopAcquisition();

    int registerCallback(IPercivalCallback *callback);

    int processSubFrame(uint32_t frameID, PercivalBuffer *buffer, uint32_t frameNumber);

    int timeout(uint32_t frameID);

    void unscramble(int      numPts,       // Number of points to process
                    uint8_t *in_data,     // Input data
                    uint8_t *reset_data,  // Reset data
                    uint8_t *out_data,    // Output data
                    uint32_t x1,
                    uint32_t x2,
                    uint32_t y1);

    void unscramble(int      numPts,       // Number of points to process
                    uint16_t *in_data,     // Input data
                    uint16_t *reset_data,  // Reset data
                    uint16_t *out_data,    // Output data
                    uint32_t x1,
                    uint32_t x2,
                    uint32_t y1);

    void unscramble(int      numPts,       // Number of points to process
                    uint32_t *in_data,     // Input data
                    uint32_t *reset_data,  // Reset data
                    uint32_t *out_data,    // Output data
                    uint32_t x1,
                    uint32_t x2,
                    uint32_t y1);

  private:

    uint32_t     debug_;             // Debug level
    uint32_t     watchdogTimeout_;   // Watchdog timeout (ms)
    std::string  errorMessage_;      // Error message string
    bool         acquiring_;         // Are we acquiring
    boost::mutex access_;            // Mutex for mask locking
    bool         descramble_;        // Should we descramble or just reconstruct the raw input frame

    uint32_t     width_;             // Width of full frame
    uint32_t     height_;            // Height of full frame
    DataType     type_;              // Data type of full frame
    uint32_t     pixelSize_;         // No of pixels per full frame
    uint32_t     byteSize_;          // No of bytes per full frame
    uint32_t     *dataIndex_;        // Pointer to array of unscrambled pixel locations
    uint32_t     *ADCIndex_;         // Pointer to array of ADCs for each pixel
    float        *ADCLowGain_;       // Pointer to array of low gain ADC gains, one per ADC
    float        *ADCHighGain_;      // Pointer to array of high gain ADC gains, one per ADC
    float        *ADCOffset_;        // Pointer to combined offset for both ADC's
    float        *stageGains_;       // Gain to apply for each of the output stages (in scrambled order)
    float        *stageOffsets_;     // Offsets to apply for each of the output stages (in scrambled order)

    std::map<int, PercivalSubFrame *>   subFrameMap_; // Map to contain sub-image UDP data receivers
    uint32_t                            serverMask_;  // bitmask of servers setup
    uint32_t                            serverReady_; // bitmask of servers that have notified of frames
    uint32_t                            frameNumber_; // Current frame number

    IPercivalCallback                   *callback_;   // Callback interface
    PercivalBuffer                      *fullFrame_;   // Full frame buffer

};

#endif /* DATASENDER_H_ */

