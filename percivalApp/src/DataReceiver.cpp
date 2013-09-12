/*
 * DataReceiver.cpp
 *
 *  Created on: 19th Aug 2013
 *      Author: gnx91527
 */

#include "DataReceiver.h"
#include "PercivalDebug.h"


DataReceiver::DataReceiver()
	: callback_(0),
    headerPosition_(headerAtStart),
    debug_(0),
    errorMessage_(""),
    running_(false),
    frameHeaderLength_(sizeof(PacketHeader))
{
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
  } catch (boost::exception& e){
    errorMessage_ = "Unable to setup local endpoint";
    dbg.log(0, "Unable to setup local endpoint");
    dbg.log(0, e);
    return -1;
  }

  int nativeSocket = (int)(recvSocket_->native());
	int rcvBufSize = 8388608;
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
      delete(recvSocket_);
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

int DataReceiver::startAcquisition(uint32_t frameBytes, uint32_t subFrames)
{
  PercivalDebug dbg(debug_, "DataReceiver::startAcquisition");

  dbg.log(1, "Frame size", frameBytes, "Subframes", subFrames);
  // First check we have a registered callback
  if (!callback_){
    errorMessage_ = "No callback registered";
    dbg.log(0, "Cannot start acquisition without a registered callback");
  }

  if (!acquiring_){
    dbg.log(1, "Starting acquisition loop");

    // Set acquisition flag
    acquiring_ = true;

//		// Initialise current frame counter to number of frames to be acquired
//		mRemainingFrames = mNumFrames;

    // Initialise counters for next frame acquisition sequence
    framePayloadBytesReceived_ = 0;
    frameTotalBytesReceived_   = 0;
    subFramesReceived_         = 0;
    subFramePacketsReceived_   = 0;
    subFrameBytesReceived_     = 0;
    framesReceived_            = 0;
    latchedFrameNumber_        = 0;

    // Initialise subframe length
    frameBytes_ = frameBytes;
    subFrames_ = subFrames;
    subFrameLength_ = frameBytes / subFrames;

		// Initialise latched error signal
		//mLatchedErrorSignal = FemDataReceiverSignal::femAcquisitionNullSignal;

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
    dbg.log(1, "Current buffer address", (int64_t)currentBuffer_);

    // Launch async receive on UDP socket
    boost::array<boost::asio::mutable_buffer, 3> rxBufs;
    if (headerPosition_ == headerAtStart){
      rxBufs[0] = boost::asio::buffer((void *)&packetHeader_, sizeof(packetHeader_));
      rxBufs[1] = boost::asio::buffer(currentBuffer_->raw(), subFrameLength_);
      rxBufs[2] = boost::asio::buffer((void *)&currentFrameNumber_, sizeof(currentFrameNumber_));
    } else {
      rxBufs[0] = boost::asio::buffer(currentBuffer_->raw(), subFrameLength_);
      rxBufs[1] = boost::asio::buffer((void *)&packetHeader_, sizeof(packetHeader_));
      rxBufs[2] = boost::asio::buffer((void *)&currentFrameNumber_, sizeof(currentFrameNumber_));
    }

    recvSocket_->async_receive_from(rxBufs, remoteEndpoint_,
                                    boost::bind(&DataReceiver::handleReceive, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred
                                               )
                                   );

    // Setup watchdog handler deadline actor to handle receive timeouts
//    mRecvWatchdogCounter = 0;
//    mWatchdogTimer.expires_from_now(boost::posix_time::milliseconds(kWatchdogHandlerIntervalMs));
//    watchdogHandler();



    // Launch a thread to start the io_service for receiving data, running the watchdog etc
    receiverThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&boost::asio::io_service::run, &ioService_)));

  } else {
    dbg.log(0, "Resetting IO service failed");
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
  PercivalDebug dbg(debug_, "DataReceiver::handleReceive");
  uint32_t errorSignal = 0;


  dbg.log(1, "Thread ID", boost::this_thread::get_id());
  dbg.log(2, "Bytes received", (uint32_t)bytesReceived);

	//time_t recvTime;
	//FemDataReceiverSignal::FemDataReceiverSignals errorSignal = FemDataReceiverSignal::femAcquisitionNullSignal;
  dbg.log(2, "****************************");
  dbg.log(2, "Frame   ", packetHeader_.frameNumber);
  dbg.log(2, "SubFrame", packetHeader_.subFrameNumber);
  dbg.log(2, "Packet  ", packetHeader_.packetNumberFlags & kPacketNumberMask);
  dbg.log(2, "SOF", (packetHeader_.packetNumberFlags & kStartOfFrameMarker) >> 31);
  dbg.log(2, "EOF", (packetHeader_.packetNumberFlags & kEndOfFrameMarker) >> 30);

  if (!errorCode && bytesReceived > 0){
    unsigned int payloadBytesReceived = bytesReceived - frameHeaderLength_ - sizeof(FrameNumber);

    // Update Total amount of data received in this frame so far including headers
    frameTotalBytesReceived_   += bytesReceived;

    // Update total payload data received in this subframe so far
    subFrameBytesReceived_     += payloadBytesReceived;

    // Update total payload data received in this subframe so far, minus packet headers and any frame counters recevied
    // at the end of each subframe
    framePayloadBytesReceived_ += payloadBytesReceived;

    dbg.log(3, "payloadBytesReceived", payloadBytesReceived);
    dbg.log(3, "frameTotalBytesReceived_", frameTotalBytesReceived_);
    dbg.log(3, "subFrameBytesReceived_", subFrameBytesReceived_);
    dbg.log(3, "framePayloadBytesRecevied_", framePayloadBytesReceived_);
    // If this is the first packet in a sub-frame, we expect packet header to have SOF marker and packet number to
    // be zero. Otherwise, check that the packet number is incrementing correctly, i.e. we have not dropped any packets
    if (subFramePacketsReceived_ == 0){
      if (!(packetHeader_.packetNumberFlags & kStartOfFrameMarker)){
        dbg.log(0, "Missing SOF marker");
        errorSignal = 1;
      } else {
        subFramePacketsReceived_++;
      }
    } else {
      // Check packet number is incrementing as expected within subframe
      if ((packetHeader_.packetNumberFlags & kPacketNumberMask) != subFramePacketsReceived_){
        dbg.log(0, "Incorrect packet number sequence", (packetHeader_.packetNumberFlags & kPacketNumberMask), "Expected", subFramePacketsReceived_);
        //errorSignal = FemDataReceiverSignal::femAcquisitionCorruptImage;
      }

      // Check for EOF marker in packet header, if so, test sanity of frame counters, handle
      // end-of-subframe bookkeeping, reset subframe packet and data counters and increment
      // number of subframes received
      if (packetHeader_.packetNumberFlags & kEndOfFrameMarker){

        // Timestamp reception of last packet of frame
        //time(&recvTime);

        // If this is the first subframe, store the frame counter from the end
        // of the frame and check it is incrementing correctly from the last
        // frame, otherwise check that they agree across subframes
        if (subFramesReceived_ == 0){
//          if (enableFrameCounterCheck_){
//            if (currentFrameNumber_ != (latchedFrameNumber_+1)){
//              std::cout << "Incorrect frame counter on first subframe, got: " << currentFrameNumber_;
//              std::cout << " expected: " << latchedFrameNumber_+1 << std::endl;
              //errorSignal = FemDataReceiverSignal::femAcquisitionCorruptImage;
//            }
//          } else {
            currentFrameNumber_ = packetHeader_.frameNumber;  //latchedFrameNumber_+1;
//          }
          latchedFrameNumber_ = currentFrameNumber_;
        } else {
//          if (enableFrameCounterCheck_) {
//            if (currentFrameNumber_ != latchedFrameNumber_){
//              std::cout << "Incorrect frame counter in subframe, got: " << currentFrameNumber_;
//              std::cout << " expected: " << latchedFrameNumber_ << std::endl;
              //errorSignal = FemDataReceiverSignal::femAcquisitionCorruptImage;
//            }
//          }
        }

        // Decrement subframe and frame payload bytes received to account for frame counter
        // appended to last packet
//        subFrameBytesReceived_ -= sizeof(currentFrameNumber_);
//        framePayloadBytesReceived_ -= sizeof(currentFrameNumber_);

        // Increment number of subframes received
        subFramesReceived_++;

        // If the number of subframes received matches the number expected for the
        // frame, check that we have received the correct amount of data
        if (subFramesReceived_ == subFrames_){
          if (framePayloadBytesReceived_ != frameBytes_){
            dbg.log(0, "Received complete frame with incorrect size", framePayloadBytesReceived_, "Expected", frameBytes_);
            errorSignal = 1;
          } else {
            dbg.log(1, "Frame completed OK, counter", currentFrameNumber_);
          }
          framesReceived_++;
        }

        // Reset subframe counters
        subFramePacketsReceived_ = 0;
        subFrameBytesReceived_   = 0;
      } else {
        subFramePacketsReceived_++;
      }
    }

    if (framePayloadBytesReceived_ > frameBytes_){
      dbg.log(0, "Buffer overrun detected in receive of frame number", currentFrameNumber_);
      dbg.log(0, "Subframe", subFramesReceived_, "Packet", subFramePacketsReceived_);
      errorSignal = 1;
    }

    // Signal current buffer as received if completed, and request a new one unless
    // we are on the last frame, in which case signal acquisition complete
    if (framePayloadBytesReceived_ >= frameBytes_){
      if (callback_){
        // HERE MAKE THE CALLBACK FOR THIS FRAME IN A SEPERATE THREAD
//        callback_->imageReceived(currentBuffer_);

        // Launch a thread to start the io_service for receiving data, running the watchdog etc
        workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IPercivalCallback::imageReceived, callback_, currentBuffer_, currentFrameNumber_)));
         

      }

      // Allocate new buffer
      if (callback_){
        currentBuffer_ = callback_->allocateBuffer();
      }

      // Reset frame counters
      framePayloadBytesReceived_ = 0;
      frameTotalBytesReceived_   = 0;
      subFramePacketsReceived_   = 0;
      subFramesReceived_         = 0;
      subFrameBytesReceived_     = 0;

      // Reset latched error signal
      //mLatchedErrorSignal = FemDataReceiverSignal::femAcquisitionNullSignal;
    }
  } else {
    dbg.log(0, "Got error during receive", (uint32_t)errorCode.value());
    dbg.log(0, errorCode.message() + " recvd", (uint32_t)bytesReceived);
    errorSignal = 1;
  }

  // If an error condition has been set during packet decoding, signal it through the callback
  if (errorSignal){
    // First callback with an error to notify the caller
    // Reset all counters, we are going to try and recover
    framePayloadBytesReceived_ = 0;
    frameTotalBytesReceived_   = 0;
    subFramePacketsReceived_   = 0;
    subFramesReceived_         = 0;
    subFrameBytesReceived_     = 0;
  }

  if (acquiring_){
    // Construct buffer sequence for reception of next packet header and payload. Payload buffer
    // points to the next position in the current buffer
    boost::array<boost::asio::mutable_buffer, 3> rxBufs;
    if (headerPosition_ == headerAtStart){
      dbg.log(2, "Buff size", (frameBytes_ - framePayloadBytesReceived_));
      rxBufs[0] = boost::asio::buffer((void *)&packetHeader_, sizeof(packetHeader_));
      rxBufs[1] = boost::asio::buffer(((uint8_t *)currentBuffer_->raw()) + framePayloadBytesReceived_, (frameBytes_ - framePayloadBytesReceived_ + 4));
      rxBufs[2] = boost::asio::buffer((void *)&currentFrameNumber_, sizeof(currentFrameNumber_));
    } else {
      rxBufs[0] = boost::asio::buffer(((uint8_t *)currentBuffer_->raw()) + framePayloadBytesReceived_, frameBytes_ - framePayloadBytesReceived_);
      rxBufs[1] = boost::asio::buffer((void *)&packetHeader_, sizeof(packetHeader_));
      rxBufs[2] = boost::asio::buffer((void *)&currentFrameNumber_, sizeof(currentFrameNumber_));
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



