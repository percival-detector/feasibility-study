/*
 * DataReceiver.cpp
 *
 *  Created on: 19th Aug 2013
 *      Author: gnx91527
 */

#include "DataReceiver.h"


DataReceiver::DataReceiver()
	: callback_(0),
    headerPosition_(headerAtStart),
    debug_(0),
    errorMessage_(""),
    running_(false),
    frameHeaderLength_(sizeof(PacketHeader))
{
  static const char *functionName = "DataReceiver::DataReceiver";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }

}

DataReceiver::~DataReceiver()
{
  static const char *functionName = "DataReceiver::~DataReceiver";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }

}

void DataReceiver::setDebug(uint32_t level)
{
  static const char *functionName = "DataReceiver::setDebug";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
    std::cout << "[DEBUG] debug level set to " << level << std::endl;
  }
  debug_ = level;
}

std::string DataReceiver::errorMessage()
{
  static const char *functionName = "DataReceiver::errorMessage";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
    std::cout << "[DEBUG] message: " << errorMessage_ << std::endl;
  }
  return errorMessage_;
}

int DataReceiver::registerCallback(IPercivalCallback *callback)
{
  static const char *functionName = "DataReceiver::registerCallback";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }
  callback_ = callback;
  return 0;
}

int DataReceiver::setupSocket(const std::string& host, unsigned short port)
{
  static const char *functionName = "DataReceiver::setupSocket";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }
  try
  {
    boost::asio::ip::udp::endpoint localEndpoint(boost::asio::ip::address::from_string(host), port);
    try
    {
      recvSocket_ = new boost::asio::ip::udp::socket(ioService_, localEndpoint);
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
      std::cout << "[DEBUG] " << functionName << " - host [" << host << "]   port [" << port << "]" << std::endl;
      std::cout << "[DEBUG] Exception: " << diagnostic_information(e);
    }
    return -1;
  }

  int nativeSocket = (int)(recvSocket_->native());
	int rcvBufSize = 8388608;
	int rc = setsockopt(nativeSocket, SOL_SOCKET, SO_RCVBUF, (void*)&rcvBufSize, sizeof(rcvBufSize));
	if (rc != 0){
		std::cout << "[ERROR] " << functionName << " - setsockopt failed" << std::endl;
    errorMessage_ = "Setsockopt Failed";
    return -1;
	}

  return 0;
}

int DataReceiver::shutdownSocket()
{
  static const char *functionName = "DataReceiver::shutdownSocket";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }
  // Only shutdown if we started up OK
  if (running_){
    try
    {
      recvSocket_->close();
      delete(recvSocket_);
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

int DataReceiver::startAcquisition(uint32_t frameBytes, uint32_t subFrames)
{
  static const char *functionName = "DataReceiver::startAcquisition";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }

  if (debug_ > 1){
    std::cout << "[DEBUG] " << functionName << " - frame size [" << frameBytes << "]" << std::endl;
  }
  // First check we have a registered callback
  if (!callback_){
    errorMessage_ = "No callback registered";
    std::cout << "[ERROR] " << functionName << " - cannot start acquisition without a registered callback" << std::endl;
  }

  if (!acquiring_){
    if (debug_ > 1){
      std::cout << "[DEBUG] " << functionName << " - starting acquisition loop" << std::endl;
    }

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
      if (debug_ > 1){
        std::cout << "[DEBUG] " << functionName << " - resetting IO service" << std::endl;
      }
      ioService_.reset();
    }

    if (ioService_.stopped()){
      if (debug_ > 0){
        std::cout << "[ERROR] " << functionName << " - resetting IO service failed" << std::endl;
      }
      errorMessage_ = "Resetting IO service failed";
      // Turn off acquiring
      acquiring_ = false;
      return -1;
    }

    // Pre-allocate an initial buffer via the callback
    currentBuffer_ = callback_->allocateBuffer();
std::cout << "Current buffer address: " << currentBuffer_ << std::endl;

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
    if (debug_ > 0){
      std::cout << "[ERROR] " << functionName << " - resetting IO service failed" << std::endl;
    }
    errorMessage_ = "Acquisition already running";
    return -1;
  }

  return 0;
}

int DataReceiver::stopAcquisition()
{
  static const char *functionName = "DataReceiver::stopAcquisition";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }

  if (!acquiring_){
    if (debug_ > 1){
      std::cout << "[DEBUG] " << functionName << " - called when no acquisition is taking place" << std::endl;
    }
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
  static const char *functionName = "DataReceiver::handleReceive";
  if (debug_ > 2){
    std::cout << "[DEBUG] " << functionName << std::endl;
  }
  std::cout << "Thread ID: " << boost::this_thread::get_id() << std::endl;
  std::cout << "Bytes received: " << bytesReceived << std::endl;

	//time_t recvTime;
	//FemDataReceiverSignal::FemDataReceiverSignals errorSignal = FemDataReceiverSignal::femAcquisitionNullSignal;

  if (!errorCode && bytesReceived > 0){
    unsigned int payloadBytesReceived = bytesReceived - frameHeaderLength_ - sizeof(FrameNumber);

    // Update Total amount of data received in this frame so far including headers
    frameTotalBytesReceived_   += bytesReceived;

    // Update total payload data received in this subframe so far
    subFrameBytesReceived_     += payloadBytesReceived;

    // Update total payload data received in this subframe so far, minus packet headers and any frame counters recevied
    // at the end of each subframe
    framePayloadBytesReceived_ += payloadBytesReceived;

  std::cout << "payloadBytesRecevied: " << payloadBytesReceived << std::endl;
  std::cout << "frameTotalBytesReceived_: " << frameTotalBytesReceived_ << std::endl;
  std::cout << "subFrameBytesReceived_: " << subFrameBytesReceived_ << std::endl;
  std::cout << "framePayloadBytesRecevied_: " << framePayloadBytesReceived_ << std::endl;
    // If this is the first packet in a sub-frame, we expect packet header to have SOF marker and packet number to
    // be zero. Otherwise, check that the packet number is incrementing correctly, i.e. we have not dropped any packets
    if (subFramePacketsReceived_ == 0){
      if (!(packetHeader_.packetNumberFlags & kStartOfFrameMarker)){
        std::cout << "Missing SOF marker" << std::endl;
        //errorSignal = FemDataReceiverSignal::femAcquisitionCorruptImage;
      } else {
        subFramePacketsReceived_++;
      }
    } else {
      // Check packet number is incrementing as expected within subframe
      if ((packetHeader_.packetNumberFlags & kPacketNumberMask) != subFramePacketsReceived_){
        std::cout << "Incorrect packet number sequence, got: " << (packetHeader_.packetNumberFlags & kPacketNumberMask);
        std::cout << " expected: " << subFramePacketsReceived_ << std::endl;
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
            currentFrameNumber_ = latchedFrameNumber_+1;
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
            std::cout << "Received complete frame with incorrect size, got " << framePayloadBytesReceived_;
            std::cout << " expected " << frameBytes_ << std::endl;
            //errorSignal = FemDataReceiverSignal::femAcquisitionCorruptImage;
          } else {
            std::cout << "Frame completed OK counter = " << currentFrameNumber_ << std::endl;
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
      std::cout << "Buffer overrun detected in receive of frame number " << currentFrameNumber_;
      std::cout << " subframe " << subFramesReceived_ << " packet " << subFramePacketsReceived_ << std::endl;
      //errorSignal = FemDataReceiverSignal::femAcquisitionCorruptImage;
    }

    // Signal current buffer as received if completed, and request a new one unless
    // we are on the last frame, in which case signal acquisition complete
    if (framePayloadBytesReceived_ >= frameBytes_){
      if (callback_){
        // HERE MAKE THE CALLBACK FOR THIS FRAME IN A SEPERATE THREAD
//        callback_->imageReceived(currentBuffer_);

        // Launch a thread to start the io_service for receiving data, running the watchdog etc
        workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IPercivalCallback::imageReceived, callback_, currentBuffer_)));
         

      }

//      if (remainingFrames_ == 1){
//        // On last frame, stop acquisition loop and signal completion
//        acquiring_ = false;
//        //callbacks.signal(FemDataReceiverSignal::femAcquisitionComplete);
//      } else if (remainingFrames_ == 0) {
//        // Do nothing, running continuously
//      } else {
        // Allocate new buffer
        if (callback_){
          currentBuffer_ = callback_->allocateBuffer();
        }

//        // Decrement remaining frame counter
//        remainingFrames_--;
//      }

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
    std::cout << "Got error during receive: " << errorCode.value() << " : " << errorCode.message() << " recvd=" << bytesReceived << std::endl;
    //errorSignal = FemDataReceiverSignal::femAcquisitionCorruptImage;
  }

  // If an error condition has been set during packet decoding, signal it through the callback only if the
  // value has changed, so that this is only done once per frame
  //if ((errorSignal != FemDataReceiverSignal::femAcquisitionNullSignal) && (errorSignal != mLatchedErrorSignal)){
  //  mCallbacks.signal(errorSignal);
  //  mLatchedErrorSignal = errorSignal;
  //}

  if (acquiring_){
    // Construct buffer sequence for reception of next packet header and payload. Payload buffer
    // points to the next position in the current buffer
    boost::array<boost::asio::mutable_buffer, 3> rxBufs;
    if (headerPosition_ == headerAtStart){
std::cout << "Buff size: " << (subFrameLength_ - framePayloadBytesReceived_) << std::endl;
      rxBufs[0] = boost::asio::buffer((void *)&packetHeader_, sizeof(packetHeader_));
      //rxBufs[1] = boost::asio::buffer(((uint8_t *)currentBuffer_->raw()) + framePayloadBytesReceived_, subFrameLength_);
      rxBufs[1] = boost::asio::buffer(((uint8_t *)currentBuffer_->raw()) + framePayloadBytesReceived_, (subFrameLength_ - framePayloadBytesReceived_ + 4));
      //rxBufs[1] = boost::asio::buffer(currentBuffer_->raw(), subFrameLength_);
      rxBufs[2] = boost::asio::buffer((void *)&currentFrameNumber_, sizeof(currentFrameNumber_));
    } else {
      rxBufs[0] = boost::asio::buffer(((uint8_t *)currentBuffer_->raw()) + framePayloadBytesReceived_, subFrameLength_ - framePayloadBytesReceived_);
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



