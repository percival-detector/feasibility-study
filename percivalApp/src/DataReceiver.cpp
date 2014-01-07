/*
 * DataReceiver.cpp
 *
 *  Created on: 19th Aug 2013
 *      Author: gnx91527
 */

#include "DataReceiver.h"
#include "PercivalDebug.h"

#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


// TEMPORARY: define a packet max size
#define PKT_MAX_SIZE 10000

DataReceiver::DataReceiver()
	: callback_(0),
    headerPosition_(headerAtStart),
    debug_(0),
    watchdogTimeout_(2000),
    errorMessage_(""),
    running_(false),
    acquiring_(false),
    frameHeaderLength_(sizeof(PacketHeader)),
    cpu_(-1)
{
  counter_ = new PercivalPacketCounter(8500);
  this->setSchedFifo();
}

DataReceiver::~DataReceiver()
{
  PercivalDebug dbg(debug_, "DataReceiver::~DataReceiver");
}

void DataReceiver::setDebug(uint32_t level)
{
  PercivalDebug dbg(debug_, "DataReceiver::setDebug");
  dbg.log(1, "Debug level", level);
  debug_ = level;
}

void DataReceiver::setCpu(int cpu)
{
  PercivalDebug dbg(debug_, "DataReceiver::setCpu");
  dbg.log(1, "CPU", (uint32_t)cpu);
  cpu_ = cpu;
}

void DataReceiver::setWatchdogTimeout(uint32_t time)
{
  PercivalDebug dbg(debug_, "DataReceiver::setWatchdogTimeout");
  dbg.log(1, "Watchdog timeout", time);
  watchdogTimeout_ = time;
}

std::string DataReceiver::errorMessage()
{
  PercivalDebug dbg(debug_, "DataReceiver::errorMessage");
  dbg.log(1, "Current error message", errorMessage_);
  return errorMessage_;
}

int DataReceiver::registerCallback(IPercivalCallback *callback)
{
  PercivalDebug dbg(debug_, "DataReceiver::registerCallback");
  callback_ = callback;
  return 0;
}

int DataReceiver::setupSocket(const std::string& host, unsigned short port)
{
  PercivalDebug dbg(debug_, "DataReceiver::setupSocket");
  dbg.log(1, "Host", host);
  dbg.log(1, "Port", (uint32_t)port);
  try
  {
    boost::asio::ip::udp::endpoint localEndpoint(boost::asio::ip::address::from_string(host), port);
    try
    {
      recvSocket_ = new boost::asio::ip::udp::socket(ioService_, localEndpoint);
    } catch (boost::exception& e){
      errorMessage_ = "Unable to create the socket";
      dbg.log(0, "Unable to create the socket");
      dbg.log(0, e);
      return -1;
    }
    try
    {
      watchdogTimer_ = new boost::asio::deadline_timer(ioService_);
    } catch (boost::exception& e){
      errorMessage_ = "Unable to create the watchdog timer";
      dbg.log(0, "Unable to create the watchdog timer");
      dbg.log(0, e);
      return -1;
    }
  } catch (boost::exception& e){
    errorMessage_ = "Unable to setup local endpoint";
    dbg.log(0, "Unable to setup local endpoint");
    dbg.log(0, e);
    return -1;
  }

  int nativeSocket = (int)(recvSocket_->native());
//	int rcvBufSize = 8388608;
//	int rcvBufSize = 16777216;
  int rcvBufSize = 67108864;
//  int rcvBufSize = 134217728;
	int rc = setsockopt(nativeSocket, SOL_SOCKET, SO_RCVBUF, (void*)&rcvBufSize, sizeof(rcvBufSize));
	if (rc != 0){
    dbg.log(0, "Setsockopt failed");
    errorMessage_ = "Setsockopt Failed";
    return -1;
	}
  running_ = true;

  return 0;
}

int DataReceiver::shutdownSocket()
{
  PercivalDebug dbg(debug_, "DataReceiver::shutdownSocket");
  // Only shutdown if we started up OK
  if (running_){
    try
    {
      recvSocket_->close();
      delete recvSocket_;
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

int DataReceiver::startAcquisition(uint32_t packetBytes)
{
  PercivalDebug dbg(debug_, "DataReceiver::startAcquisition");

  dbg.log(1, "Packet Bytes", packetBytes);
  // First check we have a registered callback
  if (!callback_){
    errorMessage_ = "No callback registered";
    dbg.log(0, "Cannot start acquisition without a registered callback");
  }

  dbg.log(1, "Acquiring  ", acquiring_);

  if (!acquiring_){
    dbg.log(1, "Starting acquisition loop");

    // Set acquisition flag
    acquiring_ = true;

    packetBytes_ = packetBytes;

    // If the IO service is stopped (i.e. the receiver has been run before), restart it
    // and check that it is running OK
    if (ioService_.stopped()){
      dbg.log(1, "Resetting IO service");
      ioService_.reset();
    }

    if (ioService_.stopped()){
      dbg.log(0, "Resetting IO service failed");
      errorMessage_ = "Resetting IO service failed";
      // Turn off acquiring
      acquiring_ = false;
      return -1;
    }

    // Pre-allocate an initial buffer via the callback
    currentBuffer_ = callback_->allocateBuffer();
    // Initialize the buffer
    memset(currentBuffer_->raw(), 0, packetBytes_);
    dbg.log(1, "Current buffer address", (int64_t)currentBuffer_);

    // Launch async receive on UDP socket
    boost::array<boost::asio::mutable_buffer, 2> rxBufs;
    if (headerPosition_ == headerAtStart){
      rxBufs[0] = boost::asio::buffer((void *)&packetHeader_, sizeof(packetHeader_));
      rxBufs[1] = boost::asio::buffer(currentBuffer_->raw(), packetBytes_);
    } else {
      rxBufs[0] = boost::asio::buffer(currentBuffer_->raw(), packetBytes_);
      rxBufs[1] = boost::asio::buffer((void *)&packetHeader_, sizeof(packetHeader_));
    }

    recvSocket_->async_receive_from(rxBufs, remoteEndpoint_,
                                    boost::bind(&DataReceiver::handleReceive, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred
                                               )
                                   );

    // Setup watchdog handler deadline actor to handle receive timeouts
    recvWatchdogCounter_ = 0;
    watchdogTimer_->expires_from_now(boost::posix_time::milliseconds(watchdogTimeout_));
    watchdogHandler();


    // Launch a thread to start the io_service for receiving data, running the watchdog etc
    receiverThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&boost::asio::io_service::run, &ioService_)));

    // THREAD PINNING
    if (cpu_ != -1){
      cpu_set_t cpuset;
      pthread_t thread = receiverThread_->native_handle();
      //Set thread priority to maximum
      pthread_attr_t thAttr;
      int policy = 0;
      int max_prio_for_policy = 0;
      pthread_attr_init(&thAttr);
      pthread_attr_getschedpolicy(&thAttr, &policy);
      max_prio_for_policy = sched_get_priority_max(policy);
      pthread_setschedprio(thread, max_prio_for_policy);
      pthread_attr_destroy(&thAttr);
      //thread = pthread_self();
      // Set affinity mask to be cpu_
      CPU_ZERO(&cpuset);
      CPU_SET(cpu_, &cpuset);
      pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    }
    // END OF THREAD PINNING

  } else {
    dbg.log(0, "Acquisition already running");
    errorMessage_ = "Acquisition already running";
    return -1;
  }

  return 0;
}

int DataReceiver::stopAcquisition()
{
  PercivalDebug dbg(debug_, "DataReceiver::stopAcquisition");

  if (!acquiring_){
    dbg.log(1, "Called when no acquisition is taking place");
    errorMessage_ = "No acquisition running";
    return -1;
  } else {
    // Set acquiring flag to false
    acquiring_ = false;

    // Stop the IO service to allow the receive thread to terminate gracefully
    if (!ioService_.stopped()){
      ioService_.stop();
    }
  }

  return 0;
}

void DataReceiver::handleReceive(const boost::system::error_code& errorCode, std::size_t bytesReceived)
{
//  PercivalDebug dbg(debug_, "DataReceiver::handleReceive");
  uint16_t frameNumber    = packetHeader_.frameNumber;
  uint8_t  subFrameNumber = packetHeader_.subFrameNumber;
  uint16_t packetNumber   = packetHeader_.packetNumber;
  uint8_t  packetType     = packetHeader_.packetType;

//  counter_->addPacket(boost::posix_time::microsec_clock::local_time(),
  counter_->addPacket(
                      //bytesReceived,
                      frameNumber,
                      subFrameNumber,
                      packetNumber,
                      packetType);

//  dbg.log(1, "Thread ID", boost::this_thread::get_id());
//  dbg.log(1, "Bytes received", (uint32_t)bytesReceived);

//  dbg.log(1, "****************************");
//  dbg.log(1, "Frame   ", frameNumber);
//  dbg.log(1, "SubFrame", (uint32_t)subFrameNumber);
//  dbg.log(1, "Packet  ", packetNumber);
//  dbg.log(1, "Type    ", (uint32_t)packetType);
//  dbg.log(1, "Error   ", errorCode.message());

if (errorCode.value() == boost::system::errc::success){
  if (callback_){
    // Launch a thread to start the io_service for receiving data, running the watchdog etc
//    workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IPercivalCallback::imageReceived, callback_, currentBuffer_, (uint32_t)(bytesReceived - sizeof(packetHeader_)), frameNumber, subFrameNumber, packetNumber, packetType)));

    callback_->imageReceived(currentBuffer_, (uint32_t)(bytesReceived - sizeof(packetHeader_)), frameNumber, subFrameNumber, packetNumber, packetType);

    // Allocate new buffer
//    currentBuffer_ = callback_->allocateBuffer();
    // Initialize the buffer
//    memset(currentBuffer_->raw(), 0, packetBytes_);
  }
} else {
  // Print out the error if there was one
  std::cout << "Receive error: " << errorCode.value() << std::endl;
}

  if (acquiring_){
    // Construct buffer sequence for reception of next packet header and payload. Payload buffer
    // points to the next position in the current buffer
    boost::array<boost::asio::mutable_buffer, 2> rxBufs;
    if (headerPosition_ == headerAtStart){
      rxBufs[0] = boost::asio::buffer((void *)&packetHeader_, sizeof(packetHeader_));
      rxBufs[1] = boost::asio::buffer(currentBuffer_->raw(), packetBytes_);
    } else {
      rxBufs[0] = boost::asio::buffer(currentBuffer_->raw(), packetBytes_);
      rxBufs[1] = boost::asio::buffer((void *)&packetHeader_, sizeof(packetHeader_));
    }

    recvSocket_->async_receive_from(rxBufs, remoteEndpoint_,
                                    boost::bind(&DataReceiver::handleReceive, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred
                                               )
                                   );
  }

  // Reset receive watchdog counter
  recvWatchdogCounter_ = 0;

}

void DataReceiver::watchdogHandler(void)
{
  PercivalDebug dbg(debug_, "DataReceiver::watchdogHandler");
  if (watchdogTimer_->expires_at() <= boost::asio::deadline_timer::traits_type::now()){
    // Check if receive watchdog counter has been not been cleared
    if (recvWatchdogCounter_ > 0){
      // In here we have watchdogged, signal to the callback we have timed out and reset data variables
      dbg.log(1, "Watchdog time out");

      // Now signal to the callback that we have timed out
      if (callback_){
        callback_->timeout();
      }
    }

    // Increment watchdog counter - this will be reset to zero by the
    // receive handler every time a receive occurs
    recvWatchdogCounter_++;

    // Reset deadline timer
    watchdogTimer_->expires_from_now(boost::posix_time::milliseconds(watchdogTimeout_));
  }

  if (acquiring_){
    watchdogTimer_->async_wait(boost::bind(&DataReceiver::watchdogHandler, this));
  }
}

void DataReceiver::setSchedFifo(void)
{
    struct sched_param param;
    int status;
    printf("Setting scheduling policy to SCHED_FIFO\n");
    param.__sched_priority = 10;
    status = sched_setscheduler(0 /*this process*/, SCHED_FIFO, &param);
    if(status != 0)
    {
        perror("sched_setscheduler");
    }
}

void DataReceiver::report(void)
{
  counter_->report();
}

