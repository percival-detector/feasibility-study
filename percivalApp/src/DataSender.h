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

#include "PercivalDataType.h"
#include "PercivalPacketCounter.h"

class DataSender
{
  public:

	  DataSender();
    virtual ~DataSender();

    void setDebug(uint32_t level);

    std::string errorMessage();

    void report();

    int setupSocket(const std::string& localHost,
                    unsigned short localPort, 
                    const std::string& remoteHost,
                    unsigned short remotePort);

    int shutdownSocket();

    int sendImage(void     *buffer,          // Pointer to the data to send
                  uint32_t bufferSize,       // Size of buffer in bytes
                  uint8_t  subFrameNumber,   // Sub-Frame number
                  uint32_t packetSize,       // UDP Packet size
                  uint16_t frameNumber,      // Frame number
                  uint8_t  reset);           // Is this a reset frame or not

    void send(uint16_t frameNumber,          // Frame number
              uint8_t subFrameNumber,        // Sub-Frame number
              uint16_t packetNumber,         // UDP Packet number
              uint8_t reset,                 // Is this a reset frame or not
              uint8_t *payload,              // Pointer to the data to send
              uint32_t payloadSize);         // Size of the data send

    void setFrameHeaderPosition(DataSenderHeaderPosition aPosition);

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

	PacketHeader                    packetHeader_;

	//BufferInfo                      mCurrentBuffer;

	//FrameNumber                     mCurrentFrameNumber;
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
    PercivalPacketCounter             *counter_;

};

#endif /* DATASENDER_H_ */

