/*
 * DataReceiver.h
 *
 *  Created on: 19th Aug 2013
 *      Author: gnx91527
 */

#ifndef DATARECEIVER_H_
#define DATARECEIVER_H_

#include "IPercivalCallback.h"

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <time.h>

#include "PercivalDataType.h"

/*
typedef struct bufferInfo_t
{
	uint8_t*     addr;
	unsigned int length;
} BufferInfo;

typedef struct packetHeader_t
{
	uint32_t frameNumber;
	uint32_t subFrameNumber;
	uint32_t packetNumberFlags;
} PacketHeader;

const uint32_t kStartOfFrameMarker = 1 << 31;
const uint32_t kEndOfFrameMarker   = 1 << 30;
const uint32_t kPacketNumberMask   = 0x3FFFFFFF;

typedef enum
{
	headerAtStart,
	headerAtEnd
} DataSenderHeaderPosition;

typedef uint32_t FrameNumber;
*/

//const unsigned int kWatchdogHandlerIntervalMs = 1000;

class DataReceiver
{
  public:

	  DataReceiver();
    virtual ~DataReceiver();

    void setDebug(uint32_t level);

    void setWatchdogTimeout(uint32_t time);

    std::string errorMessage();

    int registerCallback(IPercivalCallback *callback);

    int setupSocket(const std::string& host, unsigned short port);

    int shutdownSocket();

    int startAcquisition(uint32_t frameBytes, uint32_t subFrames);

    int stopAcquisition();

  private:

    void handleReceive(const boost::system::error_code& errorCode, std::size_t bytesReceived);
    void watchdogHandler(void);

    IPercivalCallback                 *callback_;                 // Callback interface
    DataSenderHeaderPosition          headerPosition_;            // Header data position (start or end of packet)
    uint32_t                          debug_;                     // Debug level
    uint32_t                          watchdogTimeout_;           // Watchdog timeout (ms)
    std::string                       errorMessage_;              // Error message string
    bool                              running_;                   // Are we running (socket setup OK)
    bool                              acquiring_;                 // Are we listening for packets
    PacketHeader                      packetHeader_;              // Structure containing packet header information
    PercivalBuffer                    *currentBuffer_;            // Pointer to the current buffer memory allocation
    boost::asio::io_service   	      ioService_;                 // Boost service object
    boost::asio::ip::udp::endpoint	  remoteEndpoint_;
    boost::asio::ip::udp::socket      *recvSocket_;
    boost::asio::deadline_timer       *watchdogTimer_;
	  uint32_t                          recvWatchdogCounter_;
    boost::shared_ptr<boost::thread>  receiverThread_;
    boost::shared_ptr<boost::thread>  workerThread_;
    uint32_t                          frameHeaderLength_;         // 
    uint32_t                          framePayloadBytesReceived_; // 
    uint32_t                          frameTotalBytesReceived_;   // Running
    uint32_t                          subFramesReceived_;         // Number of sub-frames received
    uint32_t                          subFramePacketsReceived_;   // Number of sub-frame packets received
    uint32_t                          subFrameBytesReceived_;     // Number of sub-frame bytes received
    uint32_t                          subFrameLength_;            // Number of bytes in each sub-frame
    uint32_t                          frameBytes_;                // Number of bytes in a full frame
    uint32_t                          subFrames_;                 // Number of sub-frames in a full frame
    uint32_t                          framesReceived_;            // Number of frames received this run
    uint32_t                          latchedFrameNumber_;        // Sanity check that frames are increasing
    uint32_t                          currentFrameNumber_;
};






#endif /* DATARECEIVER_H_ */
