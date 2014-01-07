/*
 * DataSender.cpp
 *
 *  Created on: 5th Aug 2013
 *      Author: gnx91527
 */

#include "DataSender.h"
#include "PercivalDebug.h"
#include <iostream>
#include <time.h>
#include <boost/chrono.hpp>

DataSender::DataSender()
	: headerPosition_(headerAtStart),
    debug_(0),
    errorMessage_(""),
    running_(false)
{
  counter_ = new PercivalPacketCounter(16000);
}

DataSender::~DataSender()
{
  PercivalDebug dbg(debug_, "DataSender::~DataSender");
}

void DataSender::setDebug(uint32_t level)
{
  PercivalDebug dbg(debug_, "DataSender::setDebug");
  dbg.log(1, "Debug level", level);
  debug_ = level;
}

std::string DataSender::errorMessage()
{
  PercivalDebug dbg(debug_, "DataSender::errorMessage");
  dbg.log(1, "Current error message", errorMessage_);
  return errorMessage_;
}

void DataSender::report()
{
  counter_->report();
}

int DataSender::setupSocket(const std::string& localHost,
                            unsigned short localPort,
                            const std::string& remoteHost,
                            unsigned short remotePort)
{
  PercivalDebug dbg(debug_, "DataSender::setupSocket");
  dbg.log(1, "Local Host", localHost);
  dbg.log(1, "Local Port", (uint32_t)localPort);
  dbg.log(1, "Remote Host", remoteHost);
  dbg.log(1, "Remote Port", (uint32_t)remotePort);
  try
  {
    boost::asio::ip::udp::resolver resolver(ioService_);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), remoteHost, "0");
    remoteEndpoint_ = *resolver.resolve(query);
    remoteEndpoint_.port(remotePort);
  } catch (boost::exception& e){
    errorMessage_ = "Unable to resolve remote endpoint";
    dbg.log(0, "Unable to setup and resolve remote endpoint");
    dbg.log(0, e);
    return -1;
  }
  try
  {
    boost::asio::ip::udp::endpoint localEndpoint(boost::asio::ip::address::from_string(localHost), localPort);
    try
    {
      //sendSocket_ = new boost::asio::ip::udp::socket(ioService_, localEndpoint);
      sendSocket_ = new boost::asio::ip::udp::socket(ioService_);
      sendSocket_->open(boost::asio::ip::udp::v4());
    } catch (boost::exception& e){
      errorMessage_ = "Unable to create the socket";
      dbg.log(0, "Unable to create the socket");
      dbg.log(0, e);
      return -1;
    }
  } catch (boost::exception& e){
    errorMessage_ = "Unable to setup local endpoint";
    dbg.log(0, "Unable to setup local endpoint");
    dbg.log(0, e);
    return -1;
  }

  //int nativeSocket = (int)(mSendSocket->native_handle());
  int nativeSocket = (int)(sendSocket_->native());
//	int sndBufSize = 8388608;
//	int sndBufSize = 16777216;
  int sndBufSize = 67108864;
	int rc = setsockopt(nativeSocket, SOL_SOCKET, SO_SNDBUF, (void*)&sndBufSize, sizeof(sndBufSize));
	if (rc != 0){
    dbg.log(0, "Setsockopt failed");
    errorMessage_ = "Setsockopt Failed";
    return -1;
	}

  running_ = true;

  return 0;
}

int DataSender::shutdownSocket()
{
  PercivalDebug dbg(debug_, "DataSender::shutdownSocket");
  // Only shutdown if we started up OK
  if (running_){
    try
    {
      sendSocket_->close();
      delete(sendSocket_);
      ioService_.stop();
    } catch (boost::exception& e){
      errorMessage_ = "Unable to shutdown socket";
      dbg.log(0, "Unable to shutdown socket");
      dbg.log(0, e);
      return -1;
    }
    running_ = false;
  }
  return 0;
}

int DataSender::sendImage(void     *buffer,
                          uint32_t bufferSize,
                          uint8_t  subFrameNumber,
                          uint32_t packetSize,
                          uint16_t frameNumber,
                          uint8_t  reset)
{
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
  long startTime = now.time_of_day().total_microseconds();
  //PercivalDebug dbg(debug_, "DataSender::sendImage");
  uint16_t packetNumber = 0;
  uint32_t bytesSent = 0;
  uint32_t timePerPacket = 0;
  uint8_t *cBuffer = (uint8_t *)buffer;

//  timePerPacket = 4000 / ((bufferSize / packetSize) + 1);
//  std::cout << "[DEBUG] packets: " << (bufferSize/packetSize) << std::endl;
  //dbg.log(1, "Time Per Packet (us)", timePerPacket);

  //dbg.log(1, "Buffer Size (bytes)", bufferSize);
  //dbg.log(1, "Packet Size (bytes)", packetSize);
  //dbg.log(0, "Frame Number", frameNumber);
  //dbg.log(1, "Sub-Frame Number", (uint32_t)subFrameNumber);

  bytesSent = 0;
  packetNumber = 0;
  while (bytesSent < bufferSize){
    // Check if there are more bytes still to send than in one packet
    if ((bufferSize - bytesSent) > packetSize){
      send(frameNumber, subFrameNumber, packetNumber, reset, (cBuffer + bytesSent), packetSize);
      bytesSent += packetSize;
    } else {
      // In here there are less bytes left to send than in one packet
      send(frameNumber, subFrameNumber, packetNumber, reset, (cBuffer + bytesSent), (bufferSize - bytesSent));
      bytesSent += (bufferSize - bytesSent);
    }
    packetNumber++;

    if (packetNumber % 50 == 0){
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
//std::cout << "SubFrame [" << frame << "] Bytes sent [" << bytesSent << "] Packet number [" << packetNumber << "]" << std::endl;
  }
  // Final check that we sent the correct number of bytes
  if (bytesSent != bufferSize){
    //dbg.log(0, "ERROR, bytes sent not equal to total bytes to send");
  }
  now = boost::posix_time::microsec_clock::local_time();
  long duration = now.time_of_day().total_microseconds() - startTime;
//  std::cout << "[DEBUG] duration: " << duration << " usec" << std::endl;
  return 0;
}

void DataSender::send(uint16_t frameNumber,
                      uint8_t subFrameNumber,
                      uint16_t packetNumber,
                      uint8_t reset,
                      uint8_t *payload,
                      uint32_t payloadSize)
{
  //PercivalDebug dbg(debug_, "DataSender::send");
  // Create the packet header
  packetHeader_.frameNumber    = frameNumber;
  packetHeader_.subFrameNumber = subFrameNumber;
  packetHeader_.packetNumber   = packetNumber;
  packetHeader_.packetType     = reset;
  //dbg.log(2, "Frame Number", frameNumber);
  //dbg.log(2, "Sub-Frame Number", (uint32_t)subFrameNumber);
  //dbg.log(2, "Packet Number", packetNumber);
  //dbg.log(2, "Packet Type", (uint32_t)reset);
  //dbg.log(2, "Payload Size", payloadSize);

  counter_->addPacket(
                      //bytesReceived,
                      frameNumber,
                      subFrameNumber,
                      packetNumber,
                      reset);

  //uint16_t *test;
  //test = (uint16_t *)payload;

  try
  {
    boost::array<boost::asio::mutable_buffer, 2> sndBufs;
    if (headerPosition_ == headerAtStart){
      sndBufs[0] = boost::asio::buffer((void*)&packetHeader_, sizeof(packetHeader_));
      sndBufs[1] = boost::asio::buffer(payload, payloadSize);
    } else {
      sndBufs[0] = boost::asio::buffer(payload, payloadSize);
      sndBufs[1] = boost::asio::buffer((void*)&packetHeader_, sizeof(packetHeader_));
    }

    std::size_t bytes = sendSocket_->send_to(sndBufs, remoteEndpoint_);
    if (bytes != 8006 && bytes != 2310){
      std::cout << "Bytes Sent: " << bytes << std::endl;
    }
  } catch(boost::exception& e){
    std::cout << "[ERROR] send failed: " << diagnostic_information(e) << std::endl;
    // HERE we need to return an error so that the acquisition can be stopped gracefully
    //dbg.log(0, e);
    //dbg.log(0, "Frame Number", frameNumber);
    //dbg.log(0, "Sub-Frame Number", subFrameNumber);
    //dbg.log(0, "Packet Number", packetNumber);
    //dbg.log(0, "Payload Size", payloadSize);
  }

}

void DataSender::setFrameHeaderPosition(DataSenderHeaderPosition position)
{
	headerPosition_ = position;
}

