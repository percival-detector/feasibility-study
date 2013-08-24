/*
 * DataSender.cpp
 *
 *  Created on: 5th Aug 2013
 *      Author: gnx91527
 */

#include "DataSender.h"
#include <iostream>
#include <time.h>


DataSender::DataSender()
	: headerPosition_(headerAtStart),
    debug_(0),
    errorMessage_(""),
    running_(false),
    mCurrentFrameNumber(0)
	  //mNumSubFrames(1),
	  //mSubFrameLength(0),
	  //mEnableFrameCounterCheck(true),
	  //mFrameTotalBytesSent(0),
	  //mFramePayloadBytesSent(0)
{
  static const char *functionName = "DataSender::DataSender";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }

}

DataSender::~DataSender()
{
  static const char *functionName = "DataSender::~DataSender";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }

}

void DataSender::setDebug(uint32_t level)
{
  static const char *functionName = "DataSender::setDebug";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
    std::cout << "[DEBUG] debug level set to " << level << std::endl;
  }
  debug_ = level;
}

std::string DataSender::errorMessage()
{
  static const char *functionName = "DataSender::errorMessage";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
    std::cout << "[DEBUG] message: " << errorMessage_ << std::endl;
  }
  return errorMessage_;
}

int DataSender::setupSocket(const std::string& localHost,
                            unsigned short localPort, 
                            const std::string& remoteHost,
                            unsigned short remotePort)
{
  static const char *functionName = "DataSender::setupSocket";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }
  try
  {
    boost::asio::ip::udp::resolver resolver(ioService_);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), remoteHost, "0");
    remoteEndpoint_ = *resolver.resolve(query);
    remoteEndpoint_.port(remotePort);
  } catch (boost::exception& e){
		std::cout << "[ERROR] " << functionName << " - Unable to setup and resolve remote endpoint" << std::endl;
    errorMessage_ = "Unable to resolve remote endpoint";
    if (debug_ > 0){
      std::cout << "[DEBUG] " << functionName << " - remoteHost [" << remoteHost << "]   remotePort [" << remotePort << "]" << std::endl;
      std::cout << "[DEBUG] Exception: " << diagnostic_information(e);
    }
    return -1;
  }
  try
  {
    boost::asio::ip::udp::endpoint localEndpoint(boost::asio::ip::address::from_string(localHost), localPort);
    try
    {
      sendSocket_ = new boost::asio::ip::udp::socket(ioService_, localEndpoint);
      //mSendSocket->open(boost::asio::ip::udp::v4());
    } catch (boost::exception& e){
		  std::cout << "[ERROR] " << functionName << " - Unable to create the socket" << std::endl;
      errorMessage_ = "Unable to create the socket";
      if (debug_ > 0){
        std::cout << "[DEBUG] Exception: " << diagnostic_information(e);
      }
      return -1;
    }
  } catch (boost::exception& e){
		std::cout << "[ERROR] " << functionName << " - Unable to setup local endpoint" << std::endl;
    errorMessage_ = "Unable to setup local endpoint";
    if (debug_ > 0){
      std::cout << "[DEBUG] " << functionName << " - localHost [" << localHost << "]   localPort [" << localPort << "]" << std::endl;
      std::cout << "[DEBUG] Exception: " << diagnostic_information(e);
    }
    return -1;
  }

  //int nativeSocket = (int)(mSendSocket->native_handle());
  int nativeSocket = (int)(sendSocket_->native());
	int sndBufSize = 8388608;
	int rc = setsockopt(nativeSocket, SOL_SOCKET, SO_SNDBUF, (void*)&sndBufSize, sizeof(sndBufSize));
	if (rc != 0){
		std::cout << "[ERROR] " << functionName << " - setsockopt failed" << std::endl;
    errorMessage_ = "Setsockopt Failed";
    return -1;
	}

  running_ = true;

  //boost::array<char, 1> send_buf  = {{ 43 }};
  //sendSocket_->send_to(boost::asio::buffer(send_buf), remoteEndpoint_);

  return 0;
}

int DataSender::shutdownSocket()
{
  static const char *functionName = "DataSender::shutdownSocket";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }
  // Only shutdown if we started up OK
  if (running_){
    try
    {
      sendSocket_->close();
      delete(sendSocket_);
      ioService_.stop();    
    } catch (boost::exception& e){
		  std::cout << "[ERROR] " << functionName << " - Unable to shutdown socket" << std::endl;
      errorMessage_ = "Unable to shutdown socket";
      if (debug_ > 0){
        std::cout << "[DEBUG] Exception: " << diagnostic_information(e);
      }
      return -1;
    }
    running_ = false;
  }
  return 0;
}

int DataSender::sendImage(uint32_t dataSize, void *buffer, uint32_t size, uint32_t subFrames, uint32_t packetSize, uint32_t time)
{
  uint32_t subFrameSize = size / subFrames;
//  uint32_t totalBytes = size * sizeof(uint32_t);
  uint32_t subFrameBytes = subFrameSize * dataSize;
  uint32_t packetBytes = packetSize * dataSize;
  uint32_t packetNumber = 0;
  uint32_t bytesSent = 0;
  uint8_t *cBuffer = (uint8_t *)buffer;
  uint32_t packetTime = time * 90 / (100.0 * ((subFrameBytes / packetBytes) + 1) * subFrames);
std::cout << "Data bytes: " << dataSize << std::endl;
std::cout << "subFrameSize: " << subFrameSize << std::endl;
std::cout << "packetSize: " << packetSize << std::endl;
std::cout << "packetBytes: " << packetBytes << std::endl;
//std::cout << "* Packet Time: " << packetTime << std::endl;
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
  long startTime = now.time_of_day().total_microseconds();
  long sleepTime = 0;
  bool sof = false;
  // We need to loop over the frames and packets posting out messages for each
  for (uint32_t frame = 1; frame <= subFrames; frame++){
    bytesSent = 0;
    packetNumber = 0;
    while (bytesSent < subFrameBytes){
      startTime += packetTime;
      if (packetNumber == 0){
        sof = true;
      } else {
        sof = false;
      }
      // Check if there are more bytes still to send than in one packet
      if ((subFrameBytes - bytesSent) > packetBytes){
        send(frame, packetNumber, sof, false, (cBuffer + ((frame-1)*subFrameBytes) + bytesSent), packetBytes);
        bytesSent += packetBytes;
      } else {
        // In here there are less bytes left to send than in one packet
        send(frame, packetNumber, sof, true, (cBuffer + ((frame-1)*subFrameBytes) + bytesSent), (subFrameBytes - bytesSent));
        bytesSent += (subFrameBytes - bytesSent);
      }
      now = boost::posix_time::microsec_clock::local_time();
      sleepTime = startTime - now.time_of_day().total_microseconds();
//std::cout << "SleepTime [" << sleepTime << "]" << std::endl;
      packetNumber++;
      if (sleepTime > 100){
        usleep(sleepTime);
      }
//std::cout << "SubFrame [" << frame << "] Bytes sent [" << bytesSent << "] Packet number [" << packetNumber << "]" << std::endl;
    }
  }
  return 0;
}

void DataSender::send(uint32_t frameNumber, uint32_t packetNumber, bool sof, bool eof, uint8_t *payload, uint32_t payloadSize)
{
  // Create the packet header
  mPacketHeader.frameNumber = frameNumber;
  mPacketHeader.packetNumberFlags = (packetNumber & kPacketNumberMask);
  if (sof){
    mPacketHeader.packetNumberFlags += kStartOfFrameMarker;
  }
  if (eof){
    mPacketHeader.packetNumberFlags += kEndOfFrameMarker;
//std::cout << "FrameNumber: " << frameNumber << std::endl;
//std::cout << "PacketNumber: " << packetNumber << std::endl;
//std::cout << "PayloadSize: " << payloadSize << std::endl;
  }
//std::cout << "Header: " << mPacketHeader.packetNumberFlags << std::endl;

  uint16_t *test;
  test = (uint16_t *)payload;
  std::cout << test[0] << " " << test[1] << " " << test[2] << " " << test[3] << " " << test[4] << std::endl;

  try
  {
    boost::array<boost::asio::mutable_buffer, 3> sndBufs;
    if (headerPosition_ == headerAtStart){
      sndBufs[0] = boost::asio::buffer((void*)&mPacketHeader, sizeof(mPacketHeader));
      sndBufs[1] = boost::asio::buffer(payload, payloadSize);
      sndBufs[2] = boost::asio::buffer((void*)&mCurrentFrameNumber, sizeof(mCurrentFrameNumber));
    } else {
      sndBufs[0] = boost::asio::buffer(payload, payloadSize);
      sndBufs[1] = boost::asio::buffer((void*)&mPacketHeader, sizeof(mPacketHeader));
      sndBufs[2] = boost::asio::buffer((void*)&mCurrentFrameNumber, sizeof(mCurrentFrameNumber));
    }

    sendSocket_->send_to(sndBufs, remoteEndpoint_);

  } catch(boost::exception& e){
    // HERE we need to return an error so that the acquisition can be stopped gracefully
    std::cout << "[DEBUG] Exception: " << diagnostic_information(e);
    std::cout << "FrameNumber: " << frameNumber << std::endl;
    std::cout << "PacketNumber: " << packetNumber << std::endl;
    std::cout << "PayloadSize: " << payloadSize << std::endl;

  }

}

//void DataSender::setNumFrames(unsigned int numFrames)
//{
//	mNumFrames = numFrames;
//}

void DataSender::setFrameLength(unsigned int frameLength)
{
	mFrameLength = frameLength;
}

void DataSender::setAcquisitionPeriod(unsigned int periodMs)
{
	mAcquisitionPeriod = periodMs;
}

void DataSender::setAcquisitionTime(unsigned int timeMs)
{
	mAcquisitionTime = timeMs;
}

void DataSender::setFrameHeaderLength(unsigned int headerLength)
{
	mFrameHeaderLength = headerLength;
}

void DataSender::setFrameHeaderPosition(DataSenderHeaderPosition position)
{
	headerPosition_ = position;
}

void DataSender::setNumSubFrames(unsigned int numSubFrames)
{
	mNumSubFrames = numSubFrames;
}

void DataSender::enableFrameCounterCheck(bool enable)
{
	mEnableFrameCounterCheck = enable;
}

