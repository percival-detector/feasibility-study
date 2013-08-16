/*
 * DataSender.h
 *
 *  Created on: 5th Aug 2013
 *      Author: gnx91527
 */

#ifndef DATASENDER_H_
#define DATASENDER_H_

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <time.h>

typedef struct bufferInfo_t
{
	uint8_t*     addr;
	unsigned int length;
} BufferInfo;

typedef struct packetHeader_t
{
	uint32_t frameNumber;
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

typedef uint64_t FrameNumber;

/*
typedef boost::function<BufferInfo(void)> allocateCallback_t;
typedef boost::function<void(int)> freeCallback_t;
typedef boost::function<void(int, time_t)> receiveCallback_t;
typedef boost::function<void(int)> signalCallback_t;
*/
/*
typedef struct callbackBundle_t
{
	allocateCallback_t allocate;
	freeCallback_t     free;
	receiveCallback_t  receive;
	signalCallback_t   signal;

} CallbackBundle;

namespace FemDataSenderSignal {
	typedef enum {
		femAcquisitionNullSignal,
		femAcquisitionComplete,
		femAcquisitionCorruptImage
	} FemDataReceiverSignals;
}
*/

const unsigned int kWatchdogHandlerIntervalMs = 1000;

class DataSender
{
  public:

	  DataSender();
    virtual ~DataSender();

    void setDebug(uint32_t level);

    std::string errorMessage();

    int setupSocket(const std::string& localHost,
                    unsigned short localPort, 
                    const std::string& remoteHost,
                    unsigned short remotePort);

    int shutdownSocket();


//    void sendImage(uint32_t frameNumber, 
    int sendImage(uint32_t *buffer, uint32_t size, uint32_t subFrames, uint32_t packetSize, uint32_t time);

    void send(uint32_t frameNumber, uint32_t packetNumber, bool sof, bool eof, uint8_t *payload, uint32_t payloadSize);

    //void startAcquisition(void);
    //void stopAcquisition(void);

    //void registerCallbacks(CallbackBundle* aBundle);

    void setFrameLength(unsigned int mFrameLength);
    void setFrameHeaderLength(unsigned int aHeaderLength);
    void setFrameHeaderPosition(DataSenderHeaderPosition aPosition);
    void setNumSubFrames(unsigned int aNumSubFrames);

    void setAcquisitionPeriod(unsigned int aPeriodMs);
    void setAcquisitionTime(unsigned int aTimeMs);
    void enableFrameCounterCheck(bool aEnable);

    bool acqusitionActive(void);

  private:

	  boost::asio::io_service   	      ioService_;
	  boost::asio::ip::udp::endpoint	  remoteEndpoint_;
	  boost::asio::ip::udp::socket      *sendSocket_;

    // Header data position (start or end of packet)
    DataSenderHeaderPosition          headerPosition_;
    // Debug level
    uint32_t                          debug_;
    // Error message string
    std::string                       errorMessage_;
    // Are we running (socket setup OK)
    bool                              running_;



//	boost::asio::deadline_timer       mWatchdogTimer;
//	boost::shared_ptr<boost::thread>  mReceiverThread;

/*	unsigned int                      mRecvWatchdogCounter;

	CallbackBundle     				  mCallbacks;

	bool							  mAcquiring;
	unsigned int                      mRemainingFrames;
*/
	unsigned int                    mNumFrames;
	unsigned int                    mFrameLength;
	unsigned int                    mFrameHeaderLength;


	unsigned int                    mAcquisitionPeriod;
	unsigned int                    mAcquisitionTime;
	unsigned int                    mNumSubFrames;
	unsigned int                    mSubFrameLength;
	bool                            mEnableFrameCounterCheck;

	PacketHeader                    mPacketHeader;

	BufferInfo                      mCurrentBuffer;

	FrameNumber                     mCurrentFrameNumber;
	//FrameNumber                       mLatchedFrameNumber;

	//unsigned int                      mFrameTotalBytesReceived;
	//unsigned int                      mFramePayloadBytesReceived;
	//unsigned int                      mSubFramePacketsReceived;
	//unsigned int                      mSubFramesReceived;
	//unsigned int                      mSubFrameBytesReceived;
	//unsigned int                      mFramesReceived;

	//FemDataReceiverSignal::FemDataReceiverSignals mLatchedErrorSignal;

	//void*                               lScratchBuffer;

	//void handleReceive(const boost::system::error_code& errorCode, std::size_t bytesReceived);
	//void simulateReceive(BufferInfo aBuffer);
	//void watchdogHandler(void);

};

#endif /* DATASENDER_H_ */

