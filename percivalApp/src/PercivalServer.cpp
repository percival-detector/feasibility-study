/*
 * PercivalServer.cpp
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 */

#include "PercivalServer.h"
#include "PercivalDebug.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#define MODE_SPATIAL  0
#define MODE_TEMPORAL 1

PercivalServer::PercivalServer()
	: debug_(0),
    watchdogTimeout_(2000),
    errorMessage_(""),
    acquiring_(false),
    descramble_(0),
    host_(""),
    port_(0),
    packetSize_(0),
//    mode_(MODE_SPATIAL),  // Init mode to spatial
    mode_(MODE_TEMPORAL),  // Init mode to temporal
    currentSubFrame_(0),  // Currently selected subframe 0
    cpuGroup_(-1),
    serverMask_(0),
    serverReady_(0),
    frameNumber_(0),
    resetFrameNumber_(0),
    resetFrameReady_(0),
    processTime_(0),
    serviceTime_(0),
    resetProcessTime_(0),
    callback_(0),
    buffers_(0)
{
  checker_ = new PercivalPacketChecker();
  resetChecker_ = new PercivalPacketChecker();

  // Init error stats
  errorStats_.duplicatePackets = 0;
  errorStats_.missingPackets = 0;
  errorStats_.latePackets = 0;
  errorStats_.incorrectSubFramePackets = 0;

  // Init error stats
  resetErrorStats_.duplicatePackets = 0;
  resetErrorStats_.missingPackets = 0;
  resetErrorStats_.latePackets = 0;
  resetErrorStats_.incorrectSubFramePackets = 0;

  // Init missing frame stats
  resetFramesMissing_ = 0;

  // Start the frame processing thread
  workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PercivalServer::processTemporalFrame, this)));
  // Start the reset frame processing thread
  resetThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PercivalServer::processTemporalResetFrame, this)));

}

PercivalServer::~PercivalServer()
{
  PercivalDebug dbg(debug_, "PercivalServer::~PercivalServer");
}

void PercivalServer::setDebug(uint32_t level)
{
  PercivalDebug dbg(debug_, "PercivalServer::setDebug");
  dbg.log(1, "Debug level", level);
  debug_ = level;
  // Set debug level of the checker classes
  checker_->setDebug(level);
  // Set debug level of the checker classes
  resetChecker_->setDebug(level);
  // Iterate over sub-frames and set debug levels
  std::map<int, PercivalSubFrame *>::const_iterator iterator;
  for(iterator = subFrameMap_.begin(); iterator != subFrameMap_.end(); iterator++){
    iterator->second->setDebug(level);
  }
}

void PercivalServer::setToSpatialMode(uint32_t subFrameID)
{
  PercivalDebug dbg(debug_, "PercivalServer::setToSpatialMode");
  dbg.log(1, "subFrameID", subFrameID);
  mode_ = MODE_SPATIAL;
  currentSubFrame_ = subFrameID;
}

void PercivalServer::setToTemporalMode()
{
  PercivalDebug dbg(debug_, "PercivalServer::setToTemporalMode");
  mode_ = MODE_TEMPORAL;
}

void PercivalServer::setWatchdogTimeout(uint32_t time)
{
  PercivalDebug dbg(debug_, "PercivalServer::setWatchdogTimeout");
  dbg.log(1, "Watchdog timeout", time);
  watchdogTimeout_ = time;
}

std::string PercivalServer::errorMessage()
{
  PercivalDebug dbg(debug_, "PercivalServer::errorMessage");
  dbg.log(1, "Message", errorMessage_);
  return errorMessage_;
}

void PercivalServer::setDescramble(uint32_t descramble)
{
  PercivalDebug dbg(debug_, "PercivalServer::errorMessage");
  dbg.log(1, "Descramble", descramble);
  descramble_ = descramble;
}

int PercivalServer::getCpuGroup()
{
    return cpuGroup_;
}

void PercivalServer::setCpuGroup(int cpuGroup)
{
  PercivalDebug dbg(debug_, "PercivalServer::setCpuGroup");
  dbg.log(1, "CPU group", (uint32_t)cpuGroup);
  cpuGroup_ = cpuGroup;

  // NOW PIN THE PROCESS THREAD
  if (cpuGroup_ != -1){
    cpu_set_t cpuset;
    pthread_t thread = workerThread_->native_handle();
    //Set thread priority to maximum
    pthread_attr_t thAttr;
    int policy = 0;
    int max_prio_for_policy = 0;
    pthread_attr_init(&thAttr);
    pthread_attr_getschedpolicy(&thAttr, &policy);
    max_prio_for_policy = sched_get_priority_max(policy);
    pthread_setschedprio(thread, max_prio_for_policy);
    pthread_attr_destroy(&thAttr);
    // Set affinity mask to be group*3
    CPU_ZERO(&cpuset);
    CPU_SET(cpuGroup_*3, &cpuset);
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
  }
  // END OF THREAD PINNING

  // NOW PIN THE RESET THREAD
  if (cpuGroup_ != -1){
    cpu_set_t cpuset;
    pthread_t thread = resetThread_->native_handle();
    //Set thread priority to maximum
    pthread_attr_t thAttr;
    int policy = 0;
    int max_prio_for_policy = 0;
    pthread_attr_init(&thAttr);
    pthread_attr_getschedpolicy(&thAttr, &policy);
    max_prio_for_policy = sched_get_priority_max(policy);
    pthread_setschedprio(thread, max_prio_for_policy);
    pthread_attr_destroy(&thAttr);
    // Set affinity mask to be group*3+1
    CPU_ZERO(&cpuset);
    CPU_SET(cpuGroup_*3+1, &cpuset);
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
  }
  // END OF THREAD PINNING


}

int PercivalServer::setupFullFrame(uint32_t width,              // Width of full frame in pixels
                                   uint32_t height,             // Height of full frame in pixels
                                   DataType type,               // Data type (unsigned 8, 16 or 32 bit)
                                   uint32_t *dataIndex,         // Index in output array of input data
                                   uint32_t *ADCIndex,          // Index of ADC to use for each input data point
//                                   uint32_t *ADCLowGain,        // Array of low gain ADC gains, one per ADC
//                                   uint32_t *ADCHighGain,       // Array of high gain ADC gains, one per ADC
//                                   uint32_t *ADCOffset,         // Combined offset for both ADC's
//                                   uint32_t *stageGains,      // Gain to apply for each of the output stages (in scrambled order)
//                                   uint32_t *stageOffsets)    // Offsets to apply for each of the output stages (in scrambled order)
                                   float    *ADCLowGain,        // Array of low gain ADC gains, one per ADC
                                   float    *ADCHighGain,       // Array of high gain ADC gains, one per ADC
                                   float    *ADCOffset,         // Combined offset for both ADC's
                                   float    *stageGains,      // Gain to apply for each of the output stages (in scrambled order)
                                   float    *stageOffsets)    // Offsets to apply for each of the output stages (in scrambled order)
{
  PercivalDebug dbg(debug_, "PercivalServer::setupFullFrame");
  width_ = width;
  height_ = height;
  type_ = type;
  pixelSize_ = width * height;
  byteSize_ = pixelSize_ * (uint32_t)pow(2.0, (double)type);
  dataIndex_ = dataIndex;
  ADCIndex_ = ADCIndex;
  ADCLowGain_ = ADCLowGain;
  ADCHighGain_ = ADCHighGain;
  ADCOffset_ = ADCOffset;
  stageGains_ = stageGains;
  stageOffsets_ = stageOffsets;
  return 0;
}

int PercivalServer::setupChannel(const std::string& host, unsigned short port, uint32_t packetSize)
{
  PercivalDebug dbg(debug_, "PercivalServer::setupChannel");
  dbg.log(1, "Server Host", host);
  dbg.log(1, "Server Port", port);
  dbg.log(1, "Packet Size", packetSize);
  host_ = host;
  port_ = port;

  // Create the Percival buffer pool from the size packet specified
  //if (buffers_){
  //  delete(buffers_);
  //}
  if (packetSize_ != packetSize){
    packetSize_ = packetSize;
    buffers_ = new PercivalBufferPool(packetSize);
  }
  return 0;
}


int PercivalServer::setupSubFrame(uint32_t subFrameID,
                                  uint32_t topLeftX,
                                  uint32_t topLeftY,
                                  uint32_t bottomRightX,
                                  uint32_t bottomRightY)
{
  PercivalDebug dbg(debug_, "PercivalServer::setupSubFrame");
  subFrameMap_[subFrameID] = new PercivalSubFrame(subFrameID,
                                                  topLeftX,
                                                  topLeftY,
                                                  bottomRightX,
                                                  bottomRightY);
  subFrameMap_[subFrameID]->setDebug(debug_);
  serverMask_ = serverMask_ + (1 << subFrameID);
  dbg.log(1, "Server Mask", serverMask_);
  return 0;
}

int PercivalServer::releaseSubFrame(int subFrameID)
{
  PercivalDebug dbg(debug_, "PercivalServer::releaseSubFrame");
  PercivalSubFrame *sfPtr = subFrameMap_[subFrameID];
  subFrameMap_.erase(subFrameID);
  delete(sfPtr);
  serverMask_ = serverMask_ - (1 << subFrameID);
  dbg.log(1, "Server Mask", serverMask_);
  return 0;
}

int PercivalServer::releaseAllSubFrames()
{
  PercivalDebug dbg(debug_, "PercivalServer::releaseAllSubFrames");
  if (serverMask_ & 1){
    releaseSubFrame(0);
  }
  if (serverMask_ & 2){
    releaseSubFrame(1);
  }
  if (serverMask_ & 4){
    releaseSubFrame(2);
  }
  if (serverMask_ & 8){
    releaseSubFrame(3);
  }
  if (serverMask_ & 16){
    releaseSubFrame(4);
  }
  if (serverMask_ & 32){
    releaseSubFrame(5);
  }
  if (serverMask_ & 64){
    releaseSubFrame(6);
  }
  if (serverMask_ & 128){
    releaseSubFrame(7);
  }
  return 0;
}

int PercivalServer::startAcquisition()
{
  PercivalDebug dbg(debug_, "PercivalServer::startAcquisition");
  // Reset the server ready flag
  serverReady_ = 0;

  // If first entry on mask call for allocation
  if (callback_){
    fullFrame_ = callback_->allocateBuffer();
    rawFrame_ = callback_->allocateBuffer();
    resetFrame1_ = callback_->allocateBuffer();
  }

  // If we are in temporal mode allocate each subframe buffer
  if (mode_ == MODE_TEMPORAL){
    for (uint32_t sfIndex = 0; sfIndex < 8; sfIndex++){
      subFrames_[sfIndex] = subFrameMap_[sfIndex]->allocate();
      resetSubFrames_[sfIndex] = subFrameMap_[sfIndex]->allocate();
    }
  }

  // We expect a new frame whenever we start
  expectNewFrame_ = 1;
  frameNumber_ = 0;
  // We expect a new frame whenever we start
  expectNewResetFrame_ = 1;
  resetFrameNumber_ = 0;

  // Reset error stats
  errorStats_.duplicatePackets = 0;
  errorStats_.missingPackets = 0;
  errorStats_.latePackets = 0;
  errorStats_.incorrectSubFramePackets = 0;

  // Reset error stats
  resetErrorStats_.duplicatePackets = 0;
  resetErrorStats_.missingPackets = 0;
  resetErrorStats_.latePackets = 0;
  resetErrorStats_.incorrectSubFramePackets = 0;

  // Reset error stats
  resetFramesMissing_ = 0;

  // Reset time stamp
  processTime_ = 0;
  serviceTime_ = 0;
  resetProcessTime_ = 0;

  // Calculate the number of expected packets
  uint32_t expectedPackets = byteSize_ / packetSize_;
  if (byteSize_ % packetSize_ != 0){
    expectedPackets++;
  }
  // if we are in temporal mode then use subframe 0 to work out
  // the number of packets per subframe
  if (mode_ == MODE_TEMPORAL){
    expectedPackets = (subFrameMap_[0]->getNumberOfPixels() * sizeof(uint16_t)) / packetSize_;
    if ((subFrameMap_[0]->getNumberOfPixels() * sizeof(uint16_t)) % packetSize_ != 0){
      expectedPackets++;
    }
  }
  if (mode_ == MODE_SPATIAL){
    // If we are in spatial mode then we expect only 1 subframe
    checker_->init(1, expectedPackets);
    resetChecker_->init(1, expectedPackets);
  } else if (mode_ == MODE_TEMPORAL){
    // If we are in temporal mode then we expect all 8 subframes
    checker_->init(8, expectedPackets);
    resetChecker_->init(8, expectedPackets);
  }

  receiver_ = new DataReceiver();
  // Set debug level of the receiver class
  receiver_->setDebug(debug_);
  receiver_->setCpu(cpuGroup_*3+2);
  receiver_->registerCallback(this);
  receiver_->setupSocket(host_, port_);
  receiver_->startAcquisition(packetSize_);
  return 0;
}

int PercivalServer::stopAcquisition()
{
  PercivalDebug dbg(debug_, "PercivalServer::stopAcquisition");
  receiver_->stopAcquisition();
  receiver_->shutdownSocket();
  delete receiver_;
  // Release the buffer
  //if (callback_){
  //  callback_->releaseBuffer(fullFrame_);
  //  callback_->releaseBuffer(resetFrame1_);
  //}

  // Log error statistics
  dbg.log(1, "Duplicate Packets    ", errorStats_.duplicatePackets);
  dbg.log(1, "Missing Packets      ", errorStats_.missingPackets);
  dbg.log(1, "Late Packets         ", errorStats_.latePackets);
  dbg.log(1, "Bad SubFrame ID      ", errorStats_.incorrectSubFramePackets);
  dbg.log(1, "[R] Duplicate Packets", resetErrorStats_.duplicatePackets);
  dbg.log(1, "[R] Missing Packets  ", resetErrorStats_.missingPackets);
  dbg.log(1, "[R] Late Packets     ", resetErrorStats_.latePackets);
  dbg.log(1, "[R] Bad SubFrame ID  ", resetErrorStats_.incorrectSubFramePackets);

  return 0;
}

int PercivalServer::readErrorStats(uint32_t *dupPkt,
                                   uint32_t *misPkt,
                                   uint32_t *ltePkt,
                                   uint32_t *incPkt,
                                   uint32_t *dupRPkt,
                                   uint32_t *misRPkt,
                                   uint32_t *lteRPkt,
                                   uint32_t *incRPkt,
                                   uint32_t *missingResetFrames)
{
  PercivalDebug dbg(debug_, "PercivalServer::readErrorStats");
  *dupPkt = errorStats_.duplicatePackets;
  *misPkt = errorStats_.missingPackets;
  *ltePkt = errorStats_.latePackets;
  *incPkt = errorStats_.incorrectSubFramePackets;
  *dupRPkt = resetErrorStats_.duplicatePackets;
  *misRPkt = resetErrorStats_.missingPackets;
  *lteRPkt = resetErrorStats_.latePackets;
  *incRPkt = resetErrorStats_.incorrectSubFramePackets;
  *missingResetFrames = resetFramesMissing_;
  return 0;
}

int PercivalServer::readProcessTime(uint32_t *processTime, uint32_t *resetProcessTime, uint32_t *serviceTime)
{
  *processTime = processTime_;
  *resetProcessTime = resetProcessTime_;
  *serviceTime = serviceTime_;
  return 0;
}

int PercivalServer::registerCallback(IPercivalCallback *callback)
{
  PercivalDebug dbg(debug_, "PercivalServer::registerCallback");
  callback_ = callback;
  return 0;
}

void PercivalServer::imageReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType)
{
// LOCK
  {
    boost::lock_guard<boost::mutex> lock(access_);
//    PercivalDebug dbg(debug_, "PercivalServer::imageReceived");
//    dbg.log(1, "Frame Number    ", frameNumber);
//    dbg.log(1, "Sub-Frame Number", (uint32_t)subFrameNumber);
//    dbg.log(1, "Packet Number   ", packetNumber);
//    dbg.log(1, "Packet Type     ", (uint32_t)packetType);
//    dbg.log(1, "Buffer Address  ", (int64_t)buffer);

    if (mode_ == MODE_SPATIAL){
      // Forward this packet onto the appropriate routine according to whether it is reset data or frame data
      if (packetType == 0){
        // This is frame data
        framePacketReceived(buffer, bytes, frameNumber, subFrameNumber, packetNumber, packetType);
      } else if (packetType == 1){
        // This is reset data
        resetPacketReceived(buffer, bytes, frameNumber, subFrameNumber, packetNumber, packetType);
      }
    } else if (mode_ == MODE_TEMPORAL){
      // Forward this packet onto the appropriate routine according to whether it is reset data or frame data
      if (packetType == 0){
        // This is frame data
        temporalFramePacketReceived(buffer, bytes, frameNumber, subFrameNumber, packetNumber, packetType);
      } else if (packetType == 1){
        // This is reset data
        temporalResetPacketReceived(buffer, bytes, frameNumber, subFrameNumber, packetNumber, packetType);
      }
    }
  } // UNLOCK
}

void PercivalServer::resetPacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType)
{
  PercivalDebug dbg(debug_, "PercivalServer::resetPacketReceived");
  dbg.log(3, "Reset Packet Number", packetNumber);
  int status = 0;

  // If we are in spatial mode check for an incorrect subframe number
  if (subFrameNumber != currentSubFrame_){
    resetErrorStats_.incorrectSubFramePackets++;
    return;
  }
  // Perform packet checks to record any errors
  if (expectNewResetFrame_ == 1){
    // Record the frame number and reset the flag
    resetFrameNumber_ = frameNumber;
    expectNewResetFrame_ = 0;
  } else {
    // In here we expect the frame number to remain constant
    if (frameNumber < resetFrameNumber_){
      // This is bad, late packet.  Record the error and return
      resetErrorStats_.latePackets++;
      dbg.log(1, "Late Reset Packet");
      dbg.log(1, "Current Frame", resetFrameNumber_);
      dbg.log(1, "New Frame    ", frameNumber);
      dbg.log(1, "Packet       ", packetNumber);
      // Release the Percival buffer back into the pool
//      buffers_->free(buffer);
      return;
    } else if (frameNumber > resetFrameNumber_){
      // We have a new frame before we expected it.  Notify immediately
      // And record the error as missing packets
      resetErrorStats_.missingPackets++;
      dbg.log(1, "Missing Reset Packet");
      dbg.log(1, "Current Frame", resetFrameNumber_);
      dbg.log(1, "New Frame    ", frameNumber);
      dbg.log(1, "Packet       ", packetNumber);

      resetChecker_->resetAll();
      // Reset Frame is ready (although missing packets)
      resetFrameReady_ = resetFrameNumber_;
      // Now set the new frame number
      resetFrameNumber_ = frameNumber;
    }
  }

  // Check for individual packets within subframe
  status = resetChecker_->set(0, packetNumber);
  if (status != 0){
    // Duplicate packet.  Record but use the duplicate
    resetErrorStats_.duplicatePackets++;
    dbg.log(1, "Duplicate Reset Packet");
    dbg.log(1, "Current Frame", resetFrameNumber_);
    dbg.log(1, "New Frame    ", frameNumber);
    dbg.log(1, "Packet       ", packetNumber);
  }

  // Descramble the packet as necessary
  int pixelsPerPacket = packetSize_ / 2;
  int offset = pixelsPerPacket * packetNumber;
  int numPts = bytes / 2;
  dbg.log(1, "Pts/Packet ", (uint32_t)pixelsPerPacket);
  dbg.log(1, "Offset     ", (uint32_t)offset);
  dbg.log(1, "Num Pts    ", (uint32_t)numPts);

  uint16_t *ptr1 = (uint16_t *)(resetFrame1_->raw()) + offset;
  memcpy(ptr1, buffer->raw(), bytes);

  // Release the Percival buffer back into the pool
//  buffers_->free(buffer);

  if (resetChecker_->checkAll()){
    dbg.log(2, "Notify reset frame complete");
    resetChecker_->resetAll();

    // Reset Frame is ready
    resetFrameReady_ = resetFrameNumber_;
    // Add the frame to the map ready for when it is required
    resetFrameMap_[resetFrameNumber_] = resetFrame1_;
    // Allocate the new reset buffer
    resetFrame1_ = callback_->allocateBuffer();
    // Set the flag to expect a new frame
    expectNewResetFrame_ = 1;
  }
}

void PercivalServer::framePacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType)
{
  PercivalDebug dbg(debug_, "PercivalServer::framePacketReceived");
  int status = 0;

  // If we are in spatial mode check for an incorrect subframe number
  if (subFrameNumber != currentSubFrame_){
    errorStats_.incorrectSubFramePackets++;
    return;
  }
    // Perform packet checks to record any errors
    if (expectNewFrame_ == 1){
        // Record the frame number and reset the flag
        frameNumber_ = frameNumber;
        expectNewFrame_ = 0;
    } else {
      // In here we expect the frame number to remain constant
      if (frameNumber < frameNumber_){
        // This is bad, late packet.  Record the error and return
        errorStats_.latePackets++;
        dbg.log(1, "Late packet");
        dbg.log(1, "Current Frame", frameNumber_);
        dbg.log(1, "New Frame    ", frameNumber);
        dbg.log(1, "Packet       ", packetNumber);
        // Release the Percival buffer back into the pool
//        buffers_->free(buffer);
        return;
      } else if (frameNumber > frameNumber_){
        // We have a new frame before we expected it.  Notify immediately
        // And record the error as missing packets
        errorStats_.missingPackets++;
        dbg.log(1, "Missing packet");
        dbg.log(1, "Current Frame", frameNumber_);
        dbg.log(1, "New Frame    ", frameNumber);
        dbg.log(1, "Packet       ", packetNumber);

        checker_->resetAll();
        {  // LOCK
          boost::lock_guard<boost::mutex> lock(frameAccess_);
          processFrame_ = rawFrame_;
        }  // UNLOCK
        workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PercivalServer::processFrame, this, frameNumber_, processFrame_)));
        // Set the flag to expect a new frame
        if (callback_){
          rawFrame_ = callback_->allocateBuffer();
        }
        // Now set the new frame number
        frameNumber_ = frameNumber;
      }
    }

    // Check for individual packets within subframe
    status = checker_->set(0, packetNumber);
    if (status != 0){
      // Duplicate packet.  Record but use the duplicate
      errorStats_.duplicatePackets++;
      dbg.log(1, "Duplicate packet");
      dbg.log(1, "Current Frame", frameNumber_);
      dbg.log(1, "New Frame    ", frameNumber);
      dbg.log(1, "Packet       ", packetNumber);
      //checker_->report();
    }

    // Descramble the packet as necessary
    //PercivalSubFrame *sfPtr = subFrameMap_[0];
    int pixelsPerPacket = packetSize_ / 2;
    int offset = pixelsPerPacket * packetNumber;
    int numPts = bytes / 2;
    dbg.log(1, "Pts/Packet ", (uint32_t)pixelsPerPacket);
    dbg.log(1, "Offset     ", (uint32_t)offset);
    dbg.log(1, "Num Pts    ", (uint32_t)numPts);

    uint16_t *ptr1 = (uint16_t *)(rawFrame_->raw()) + offset;
    memcpy(ptr1, buffer->raw(), bytes);

    // Release the Percival buffer back into the pool
//    buffers_->free(buffer);


    if (checker_->checkAll()){
      dbg.log(2, "Notify frame complete");
      checker_->resetAll();
      {  // LOCK
        boost::lock_guard<boost::mutex> lock(frameAccess_);
        processFrame_ = rawFrame_;
      }  // UNLOCK
      workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PercivalServer::processFrame, this, frameNumber_, processFrame_)));
      if (callback_){
        rawFrame_ = callback_->allocateBuffer();
      }
      // Set the flag to expect a new frame
      expectNewFrame_ = 1;
    }

}

void PercivalServer::processFrame(uint16_t frameNumber, PercivalBuffer *rawFrame)
{
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
  long startTime = now.time_of_day().total_microseconds();
  PercivalDebug dbg(debug_, "PercivalServer::processFrame");
  PercivalBuffer *resetBuffer = NULL;

  // Check we have the reset frame ready.  If not then we are
  // lost, log the error for now
  if (resetFrameMap_.count(frameNumber)){
    // Set the local pointer to the buffer and remove from the map
    resetBuffer = resetFrameMap_[frameNumber];
    resetFrameMap_.erase(frameNumber);
  } else {
    dbg.log(1, "ERROR. Couldn't find reset frame", frameNumber);
    resetFramesMissing_++;
  }

  // Descramble the packet as necessary
  PercivalSubFrame *sfPtr = subFrameMap_[currentSubFrame_];


// LOCK
  {
    boost::lock_guard<boost::mutex> lock(frameAccess_);

    if (resetBuffer != NULL){
      unscramble(pixelSize_,
                 (uint16_t *)rawFrame->raw(),
                 (uint16_t *)resetBuffer->raw(),
                 (float *)fullFrame_->raw(),
                 sfPtr->getTopLeftX(),
                 sfPtr->getBottomRightX(),
                 sfPtr->getTopLeftY());
    } else {
      unscramble(pixelSize_,
                 (uint16_t *)rawFrame->raw(),
                 NULL,
                 (float *)fullFrame_->raw(),
                 sfPtr->getTopLeftX(),
                 sfPtr->getBottomRightX(),
                 sfPtr->getTopLeftY());
    }
    if (callback_){
      // Free the reset buffer at this point
      if (resetBuffer != NULL){
        callback_->releaseBuffer(resetBuffer);
      }
      callback_->releaseBuffer(rawFrame);
      // Notify image ready
      callback_->imageReceived(fullFrame_, 0, frameNumber, 0, 0, 0);
      fullFrame_ = callback_->allocateBuffer();
    }
  } // UNLOCK
  now = boost::posix_time::microsec_clock::local_time();
  long duration = now.time_of_day().total_microseconds() - startTime;
  processTime_ = (uint32_t)duration;
}

void PercivalServer::temporalResetPacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType)
{
  PercivalDebug dbg(debug_, "PercivalServer::temporalResetPacketReceived");
  dbg.log(3, "Reset Packet Number", packetNumber);
  int status = 0;

  // Perform packet checks to record any errors
  if (expectNewResetFrame_ == 1){
    // Record the frame number and reset the flag
    resetFrameNumber_ = frameNumber;
    expectNewResetFrame_ = 0;
  } else {
    // In here we expect the frame number to remain constant
    if (frameNumber < resetFrameNumber_){
      // This is bad, late packet.  Record the error and return
      resetErrorStats_.latePackets++;
      dbg.log(1, "Late Reset Packet");
      dbg.log(1, "Current Frame", resetFrameNumber_);
      dbg.log(1, "New Frame    ", frameNumber);
      dbg.log(1, "Packet       ", packetNumber);
      // Release the Percival buffer back into the pool
      buffers_->free(buffer);
      return;
    } else if (frameNumber > resetFrameNumber_){
      // We have a new frame before we expected it.  Notify immediately
      // And record the error as missing packets
      resetErrorStats_.missingPackets++;

      // Before attempting to process this frame print the timing stats
receiver_->report();
      std::cout << "Missing Reset packet" << std::endl;
      std::cout << "Current Frame " << resetFrameNumber_  << std::endl;
      std::cout << "New Frame     " << frameNumber  << std::endl;
      std::cout << "Packet        " << packetNumber << std::endl;
      std::cout << "Last reset process time: " << resetProcessTime_ << " usec" << std::endl;
      std::cout << "Reset Report:" << std::endl;
      resetChecker_->report();
      std::cout << "End Of Reset Report" << std::endl;

      dbg.log(1, "Missing Reset Packet");
      dbg.log(1, "Current Frame", resetFrameNumber_);
      dbg.log(1, "New Frame    ", frameNumber);
      dbg.log(1, "Packet       ", packetNumber);

      raise(SIGINT);

      resetChecker_->resetAll();
      {  // LOCK
        boost::unique_lock<boost::mutex> lock(resetAccess_);
        processResetFrameNumber_ = resetFrameNumber_;
      }  // UNLOCK
      // Reset Frame is ready (although missing packets)
      // Notify the processing thread
      resetCondition_.notify_one();
      //workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PercivalServer::processTemporalResetFrame, this, resetFrameNumber_)));
      //resetFrameReady_ = resetFrameNumber_;
      // Now set the new frame number
      resetFrameNumber_ = frameNumber;
    }
  }

  // Check for individual packets
  status = resetChecker_->set(subFrameNumber, packetNumber);
  if (status != 0){
    // Duplicate packet.  Record but use the duplicate
    resetErrorStats_.duplicatePackets++;
    dbg.log(1, "Duplicate Reset Packet");
    dbg.log(1, "Current Frame", resetFrameNumber_);
    dbg.log(1, "New Frame    ", frameNumber);
    dbg.log(1, "Packet       ", packetNumber);
  }

  // Copy the packet into the correct subframe buffer
  int pixelsPerPacket = packetSize_ / 2;
  int offset = pixelsPerPacket * packetNumber;
  dbg.log(1, "Pts/Packet ", (uint32_t)pixelsPerPacket);
  dbg.log(1, "Offset     ", (uint32_t)offset);

  uint16_t *ptr1 = (uint16_t *)(resetSubFrames_[subFrameNumber]->raw()) + offset;
  memcpy(ptr1, buffer->raw(), bytes);

  // Release the Percival buffer back into the pool
  buffers_->free(buffer);

  if (resetChecker_->checkAll()){
    dbg.log(2, "Notify reset frame complete");
    std::cout << "Reset Packet count: " << resetChecker_->getCount() << std::endl;
    resetChecker_->resetAll();
    {  // LOCK
      boost::unique_lock<boost::mutex> lock(resetAccess_);
      processResetFrameNumber_ = resetFrameNumber_;
    }  // UNLOCK
    // Notify the processing thread
    resetCondition_.notify_one();

    //workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PercivalServer::processTemporalResetFrame, this, resetFrameNumber_)));
    // Set the flag to expect a new frame
    expectNewResetFrame_ = 1;
  }
}

//void PercivalServer::processTemporalResetFrame(uint16_t frameNumber)
void PercivalServer::processTemporalResetFrame()
{
  PercivalDebug dbg(debug_, "PercivalServer::processTemporalResetFrame");

  while (1){

    // LOCK
    {
      boost::unique_lock<boost::mutex> lock(resetAccess_);

      // Wait for the signal
      resetCondition_.wait(lock);

      // Timing statistics
      boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
      long startTime = now.time_of_day().total_microseconds();

      PercivalBuffer *resetFrameBuffers[8];
      // First record each pointer to the subframe buffer
      for (int subFrame = 0; subFrame < 8; subFrame++){
        resetFrameBuffers[subFrame] = resetSubFrames_[subFrame];
        // Allocate the next buffer for each subFrame
        resetSubFrames_[subFrame] = subFrameMap_[subFrame]->allocate();
      }

      // Descramble each subframe
      for (int subFrame = 0; subFrame < 8; subFrame++){
        PercivalSubFrame *sfPtr = subFrameMap_[subFrame];
        // Unscramble

        int index;     // Index of point in subframe
        int xp;        // X prime, x coordinate within subframe
        int yp;        // Y prime, y coordinate within subframe
        int ip;        // Index prime, index of point within full frame
        int sfWidth;
        int x1 = sfPtr->getTopLeftX();
        int x2 = sfPtr->getBottomRightX();
        int y1 = sfPtr->getTopLeftY();
        uint16_t *destFrame = (uint16_t *)resetFrame1_->raw();
        uint16_t *sourceFrame = (uint16_t *)resetFrameBuffers[subFrame]->raw();
        int noOfPx = (int)sfPtr->getNumberOfPixels();
        int noOfRows = noOfPx / (x2 - x1 + 1);
        for (index = 0; index < noOfRows; index++ ){
          yp = index; // Y point within subframe
          xp = 0;     // X point within subframe
          ip = (width_ * (yp + y1)) + x1 + xp;
          sfWidth = x2-x1+1;
          // OK, here we are just reconstructing the original raw frame
          memcpy(destFrame+ip, sourceFrame+(index*sfWidth), (sfWidth*sizeof(uint16_t)));
          //destFrame[ip] = sourceFrame[index];
        }
        // Now free up the buffer
        subFrameMap_[subFrame]->release(resetFrameBuffers[subFrame]);
      }
      // Reset Frame is ready
      resetFrameReady_ = processResetFrameNumber_;
      // Add the frame to the map ready for when it is required
      resetFrameMap_[processResetFrameNumber_] = resetFrame1_;
      // Allocate the new reset buffer
      if (resetFrameMap_.size() > 1){
        dbg.log(1, "Reset Frame Size", (uint32_t)resetFrameMap_.size());
      }
      resetFrame1_ = callback_->allocateBuffer();

      now = boost::posix_time::microsec_clock::local_time();
      long duration = now.time_of_day().total_microseconds() - startTime;
      resetProcessTime_ = (uint32_t)duration;
      //std::cout << "[DEBUG] duration: " << duration << " usec" << std::endl;

    } // UNLOCK

  } // END OF PROCESS LOOP

}

void PercivalServer::temporalFramePacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType)
{
//  PercivalDebug dbg(debug_, "PercivalServer::temporalFramePacketReceived");
  int status = 0;

  // Perform packet checks to record any errors
  if (expectNewFrame_ == 1){
boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
startTime_ = now.time_of_day().total_microseconds();
    // Record the frame number and reset the flag
    frameNumber_ = frameNumber;
    expectNewFrame_ = 0;
  } else {
    // In here we expect the frame number to remain constant
    if (frameNumber < frameNumber_){
      // This is bad, late packet.  Record the error and return
      errorStats_.latePackets++;
//      dbg.log(1, "Late packet");
//      dbg.log(1, "Current Frame", frameNumber_);
//      dbg.log(1, "New Frame    ", frameNumber);
//      dbg.log(1, "Packet       ", packetNumber);
      // Release the Percival buffer back into the pool
      buffers_->free(buffer);
      return;
    } else if (frameNumber > frameNumber_){
      // We have a new frame before we expected it.  Notify immediately
      // And record the error as missing packets
      errorStats_.missingPackets++;

      // Before attempting to process this frame print the timing stats
receiver_->report();
      std::cout << "Missing packet" << std::endl;
      std::cout << "Current Frame " << frameNumber_  << std::endl;
      std::cout << "New Frame     " << frameNumber  << std::endl;
      std::cout << "Packet        " << packetNumber << std::endl;
      std::cout << "Last process time: " << processTime_ << " usec" << std::endl;
      std::cout << "Frame Report:" << std::endl;
      checker_->report();
      std::cout << "End Of Frame Report" << std::endl;

      raise(SIGINT);

//      dbg.log(1, "Missing packet");
//      dbg.log(1, "Current Frame", frameNumber_);
//      dbg.log(1, "New Frame    ", frameNumber);
//      dbg.log(1, "Packet       ", packetNumber);

      checker_->resetAll();
      {  // LOCK
        boost::unique_lock<boost::mutex> lock(frameAccess_);
        // First record each pointer to the subframe buffer
        for (int subFrame = 0; subFrame < 8; subFrame++){
          subFrameBuffers_[subFrame] = subFrames_[subFrame];
          // Allocate the next buffer for each subFrame
          subFrames_[subFrame] = subFrameMap_[subFrame]->allocate();
        }
        processFrameNumber_ = frameNumber_;
      }  // UNLOCK
      //workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PercivalServer::processTemporalFrame, this, frameNumber_, subFrameBuffers_)));
      // Notify the processing thread
      frameCondition_.notify_one();
      // Now set the new frame number
      frameNumber_ = frameNumber;
    }
  }

  // Check for individual packets
  status = checker_->set(subFrameNumber, packetNumber);
  if (status != 0){
    // Duplicate packet.  Record but use the duplicate
    errorStats_.duplicatePackets++;
//    dbg.log(1, "Duplicate packet");
//    dbg.log(1, "Current Frame", frameNumber_);
//    dbg.log(1, "New Frame    ", frameNumber);
//    dbg.log(1, "Packet       ", packetNumber);
  }
  // Copy the packet into the correct subframe buffer
  int pixelsPerPacket = packetSize_ / 2;
  int offset = pixelsPerPacket * packetNumber;
//  dbg.log(1, "Pts/Packet ", (uint32_t)pixelsPerPacket);
//  dbg.log(1, "Offset     ", (uint32_t)offset);

  uint16_t *ptr1 = (uint16_t *)(subFrames_[subFrameNumber]->raw()) + offset;
  memcpy(ptr1, buffer->raw(), bytes);

  // Release the Percival buffer back into the pool
//  buffers_->free(buffer);

  if (checker_->checkAll()){
boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
long duration = now.time_of_day().total_microseconds() - startTime_;
serviceTime_ = (uint32_t)duration;
if (duration > 199999){
  std::cout << "Bad service time: " << duration << " usec" << std::endl;
}
//    dbg.log(1, "Notify temporal frame complete");
    std::cout << "Frame Packet count: " << checker_->getCount() << std::endl;
    checker_->resetAll();
    {  // LOCK
      boost::unique_lock<boost::mutex> lock(frameAccess_);
      // First record each pointer to the subframe buffer
      for (int subFrame = 0; subFrame < 8; subFrame++){
        subFrameBuffers_[subFrame] = subFrames_[subFrame];
        // Allocate the next buffer for each subFrame
        subFrames_[subFrame] = subFrameMap_[subFrame]->allocate();
      }
      processFrameNumber_ = frameNumber_;
    }  // UNLOCK
    //workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PercivalServer::processTemporalFrame, this, frameNumber_, subFrameBuffers_)));
    // Notify the processing thread
    frameCondition_.notify_one();
    // Set the flag to expect a new frame
    expectNewFrame_ = 1;
  }

}

//void PercivalServer::processTemporalFrame(uint16_t frameNumber, PercivalBuffer *subFrameBuffers[8])
void PercivalServer::processTemporalFrame()
{
  PercivalDebug dbg(debug_, "PercivalServer::processFrame");

  // THREAD PINNING
  // THREAD NOW PINNED IN CPU GROUP ROUTINE
  /*if (cpuGroup_ != -1){
    cpu_set_t cpuset;
    pthread_t thread = pthread_self();
    //Set thread priority to maximum
    pthread_attr_t thAttr;
    int policy = 0;
    int max_prio_for_policy = 0;
    pthread_attr_init(&thAttr);
    pthread_attr_getschedpolicy(&thAttr, &policy);
    max_prio_for_policy = sched_get_priority_max(policy);
    pthread_setschedprio(thread, max_prio_for_policy);
    pthread_attr_destroy(&thAttr);
    // Set affinity mask to be group*3
    CPU_ZERO(&cpuset);
    CPU_SET(cpuGroup_*3, &cpuset);
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
  }*/ 
  // END OF THREAD PINNING

  // Loop forever
  while (1){

    // LOCK
    {
      boost::unique_lock<boost::mutex> lock(frameAccess_);

      // Wait for the signal
      frameCondition_.wait(lock);

      // Record the frame processing time start
      boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
      long startTime = now.time_of_day().total_microseconds();

      //  PercivalBuffer *subFrameBuffers[8];
      PercivalBuffer *resetBuffer = NULL;
      // Check we have the reset frame ready.  If not then we are
      // lost, log the error for now
      if (resetFrameMap_.count(processFrameNumber_)){
        // Set the local pointer to the buffer and remove from the map
        resetBuffer = resetFrameMap_[processFrameNumber_];
        resetFrameMap_.erase(processFrameNumber_);
      } else {
        dbg.log(1, "ERROR. Couldn't find reset frame", processFrameNumber_);
        resetFramesMissing_++;
        // If we've lost a reset frame we should think about deleting any hanging around in here
      }
      // Descramble each subframe

      for (int subFrame = 0; subFrame < 8; subFrame++){
        PercivalSubFrame *sfPtr = subFrameMap_[subFrame];
        // Unscramble
        if (resetBuffer != NULL){
          unscrambleToFull(sfPtr->getNumberOfPixels(),
                           (uint16_t *)(subFrameBuffers_[subFrame]->raw()),
                           (uint16_t *)resetBuffer->raw(),
                           (float *)fullFrame_->raw(),
                           sfPtr->getTopLeftX(),
                           sfPtr->getBottomRightX(),
                           sfPtr->getTopLeftY());
        } else {
          unscrambleToFull(sfPtr->getNumberOfPixels(),
                           (uint16_t *)(subFrameBuffers_[subFrame]->raw()),
                           NULL,
                           (float *)fullFrame_->raw(),
                           sfPtr->getTopLeftX(),
                           sfPtr->getBottomRightX(),
                           sfPtr->getTopLeftY());
        }
        // Now free up the buffer
        dbg.log(2, "Sub Frame Release", (uint32_t)subFrame);
        subFrameMap_[subFrame]->release(subFrameBuffers_[subFrame]);
      }

      if (callback_){
        // Free the reset buffer at this point
        if (resetBuffer != NULL){
          callback_->releaseBuffer(resetBuffer);
        }
        // Notify frame complete
        callback_->imageReceived(fullFrame_, 0, processFrameNumber_, 0, 0, 0);
        fullFrame_ = callback_->allocateBuffer();
      }
      now = boost::posix_time::microsec_clock::local_time();
      long duration = now.time_of_day().total_microseconds() - startTime;
      processTime_ = (uint32_t)duration;
      //std::cout << "[DEBUG] duration: " << duration << " usec" << std::endl;

    } // UNLOCK

  } // END OF PROCESS LOOP

}

void PercivalServer::timeout()
{
  PercivalDebug dbg(debug_, "PercivalServer::timeout");
  // If we get into here we have lost communications for some period of time
  // Its possible the sender has been stopped, or cables unplugged
  // The best thing to do here currently is to reset frame counters and just
  // keep waiting....

  // LOCK
  {
    boost::lock_guard<boost::mutex> lock(access_);
    serverReady_ = 0;
    expectNewFrame_ = 1;
    checker_->resetAll();
    frameNumber_ = 0;
    if (callback_){
      callback_->timeout();
    }
  }
}

PercivalBuffer *PercivalServer::allocateBuffer()
{
  PercivalDebug dbg(debug_, "PercivalServer::allocateBuffer");
  PercivalBuffer *buffer = buffers_->allocate();
  return buffer;
}

void PercivalServer::releaseBuffer(PercivalBuffer *buffer)
{
  PercivalDebug dbg(debug_, "PercivalServer::releaseBuffer");
  buffers_->free(buffer);
}

void PercivalServer::unscramble(int      numPts,       // Number of points to process
                                uint16_t *in_data,     // Input data
                                uint16_t *reset_data,  // Reset data
                                float    *out_data,    // Output data
                                uint32_t x1,
                                uint32_t x2,
                                uint32_t y1)
{
  PercivalDebug dbg(debug_, "PercivalServer::unscramble");
  int index;     // Index of point in subframe
  uint32_t gain;
  uint32_t ADC_low;
  uint32_t ADC_high;
  uint32_t ADC;
  uint32_t stageIndex;
  float ADC_output;

  for (index = 0; index < numPts; index++ ){
    // Check if we are descrambling or simply reconstructing
    if (!descramble_){
      // OK, here we are just reconstructing the original raw frame
      out_data[index] = in_data[index];
    } else {
      // Here we are going to descramble

      gain       = in_data[index] & 0x3;
      ADC_low    = ( in_data[index] >> 2 ) & 0xFF;
      ADC_high   = ( in_data[index] >> 10 ) & 0x1F;
      ADC        = ADCIndex_[dataIndex_[index]];
      stageIndex = gain*pixelSize_+dataIndex_[index];

      ADC_output = (ADC_high * ADCHighGain_[ADC] + ADC_low * ADCLowGain_[ADC] + ADCOffset_[ADC]);
      out_data[dataIndex_[index]] = ADC_output * stageGains_[stageIndex] + stageOffsets_[stageIndex];

      if (reset_data != NULL){
        if (gain == 0) {
          // Subtract DCS Reset signal if in diode sampling mode
          ADC_low  = ( reset_data[index] >> 2 ) & 0xFF;
          ADC_high = ( reset_data[index] >> 10 ) & 0x1F;
          ADC_output = (ADC_high * ADCHighGain_[ADC] + ADC_low * ADCLowGain_[ADC] + ADCOffset_[ADC]);
          out_data[dataIndex_[index]] -= ADC_output * stageGains_[stageIndex] + stageOffsets_[stageIndex];
        }
      }
    }
  }
}

void PercivalServer::unscrambleToFull(int      numPts,       // Number of points to process
                                      uint16_t *in_data,     // Input data
                                      uint16_t *reset_data,  // Reset data
                                      float    *out_data,    // Output data
                                      uint32_t x1,
                                      uint32_t x2,
                                      uint32_t y1)
{
  //PercivalDebug dbg(debug_, "PercivalServer::unscramble");
  int index;     // Index of point in subframe
  uint32_t xp;        // X prime, x coordinate within subframe
  uint32_t yp;        // Y prime, y coordinate within subframe
  int ip;        // Index prime, index of point within full frame
  uint32_t gain;
//  uint32_t ADC_low;
//  uint32_t ADC_high;
//  uint32_t ADC_low_r;
//  uint32_t ADC_high_r;
  float ADC_low;
  float ADC_high;
  float ADC_low_r;
  float ADC_high_r;
  uint32_t ADC;
  uint32_t stageIndex;
//  uint32_t ADC_output;
  float ADC_output;

  uint32_t sfWidth = x2 - x1 + 1;
  uint32_t numRows = numPts / sfWidth;
  index = 0;

  // Check if we are descrambling or simply reconstructing
  if (descramble_ == 0){
    for (yp = 0; yp < numRows; yp++){
      ip = (width_ * (yp + y1)) + x1;
      for (xp = 0; xp < sfWidth; xp++ ){
        // OK, here we are just reconstructing the original raw frame
        out_data[ip] = in_data[index];
        index++;
        ip++;
      }
    }
  } else if (descramble_ == 1){
    for (yp = 0; yp < numRows; yp++){
      ip = (width_ * (yp + y1)) + x1;
      for (xp = 0; xp < sfWidth; xp++ ){
        // Here we are going to descramble
//        ip = (width_ * (yp + y1)) + x1 + xp;

        gain       = in_data[index] & 0x3;
        ADC_low    = ( in_data[index] >> 2 ) & 0xFF;
        ADC_high   = ( in_data[index] >> 10 ) & 0x1F;
        ADC        = ADCIndex_[dataIndex_[ip]];
        stageIndex = gain*pixelSize_+dataIndex_[ip];

        if (reset_data != NULL && gain == 0){
          // Subtract DCS Reset signal if in diode sampling mode
          ADC_low_r  = ( reset_data[ip] >> 2 ) & 0xFF;
          ADC_high_r = ( reset_data[ip] >> 10 ) & 0x1F;
          ADC_output = (ADC_high-ADC_high_r) * ADCHighGain_[ADC] + (ADC_low-ADC_low_r) * ADCLowGain_[ADC];
          out_data[dataIndex_[ip]] = ADC_output * stageGains_[stageIndex] + stageOffsets_[stageIndex];
        } else {
          ADC_output = (ADC_high * ADCHighGain_[ADC] + ADC_low * ADCLowGain_[ADC] + ADCOffset_[ADC]);
          out_data[dataIndex_[ip]] = ADC_output * stageGains_[stageIndex] + stageOffsets_[stageIndex];
        }
        index++;
        ip++;
      }
    }
  } else if (descramble_ == 2){
    for (yp = 0; yp < numRows; yp++){
      ip = (width_ * (yp + y1)) + x1;
      for (xp = 0; xp < sfWidth; xp++ ){
        // OK, here we are reordering pixels only, no gain calculations or DCS reset
        out_data[dataIndex_[ip]] = in_data[index];
        index++;
        ip++;
      }
    }
  }
}

/*
void PercivalServer::unscrambleToFull(int      numPts,       // Number of points to process
                                      uint16_t *in_data,     // Input data
                                      uint16_t *reset_data,  // Reset data
                                      uint16_t *out_data,    // Output data
                                      uint32_t x1,
                                      uint32_t x2,
                                      uint32_t y1)
{
  //PercivalDebug dbg(debug_, "PercivalServer::unscramble");
  int index;     // Index of point in subframe
  uint32_t xp;        // X prime, x coordinate within subframe
  uint32_t yp;        // Y prime, y coordinate within subframe
  int ip;        // Index prime, index of point within full frame
  uint32_t gain;
  float ADC_low;
  float ADC_high;
  uint32_t ADC;
  uint32_t stageIndex;
  float ADC_output;

  uint32_t sfWidth = x2 - x1 + 1;
  uint32_t numRows = numPts / sfWidth;
  index = 0;
  for (yp = 0; yp < numRows; yp++){
    for (xp = 0; xp < sfWidth; xp++ ){
      ip = (width_ * (yp + y1)) + x1 + xp;

      // Check if we are descrambling or simply reconstructing
      if (descramble_ == 0){
        // OK, here we are just reconstructing the original raw frame
        out_data[ip] = in_data[index];
      } else if (descramble_ == 1){
        // Here we are going to descramble

        gain       = in_data[index] & 0x3;
        ADC_low    = ( in_data[index] >> 2 ) & 0xFF;
        ADC_high   = ( in_data[index] >> 10 ) & 0x1F;
        ADC        = ADCIndex_[dataIndex_[ip]];
        stageIndex = gain*pixelSize_+dataIndex_[ip];

        ADC_output = (ADC_high * ADCHighGain_[ADC] + ADC_low * ADCLowGain_[ADC] + ADCOffset_[ADC]);
        out_data[dataIndex_[ip]] = ADC_output * stageGains_[stageIndex] + stageOffsets_[stageIndex];

        if (reset_data != NULL){
          if (gain == 0) {
            // Subtract DCS Reset signal if in diode sampling mode
            ADC_low  = ( reset_data[ip] >> 2 ) & 0xFF;
            ADC_high = ( reset_data[ip] >> 10 ) & 0x1F;
            ADC_output = (ADC_high * ADCHighGain_[ADC] + ADC_low * ADCLowGain_[ADC] + ADCOffset_[ADC]);
            out_data[dataIndex_[ip]] -= ADC_output * stageGains_[stageIndex] + stageOffsets_[stageIndex];
          }
        }
      } else if (descramble_ == 2){
        // OK, here we are reordering pixels only, no gain calculations or DCS reset
        out_data[dataIndex_[ip]] = in_data[index];
      }
      index++;
    }
  }
}
*/

