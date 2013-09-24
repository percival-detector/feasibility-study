/*
 * PercivalServer.cpp
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 */

#include "PercivalServer.h"
#include "PercivalDebug.h"


PercivalServer::PercivalServer()
	: debug_(0),
    watchdogTimeout_(2000),
    errorMessage_(""),
    acquiring_(false),
    descramble_(false),
    host_(""),
    port_(0),
    packetSize_(0),
    mode_(0),             // Init mode to spatial
    currentSubFrame_(0),  // Currently selected subframe 0
    serverMask_(0),
    serverReady_(0),
    frameNumber_(0),
    resetFrameNumber_(0),
    resetFrameReady_(0),
    callback_(0),
    buffers_(0)
{
  checker_ = new PercivalPacketChecker();
  resetChecker_ = new PercivalPacketChecker();
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
  mode_ = 0;
  currentSubFrame_ = subFrameID;
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

void PercivalServer::setDescramble(bool descramble)
{
  PercivalDebug dbg(debug_, "PercivalServer::errorMessage");
  dbg.log(1, "Descramble", descramble);
  descramble_ = descramble;
}

int PercivalServer::setupFullFrame(uint32_t width,              // Width of full frame in pixels
                                   uint32_t height,             // Height of full frame in pixels
                                   DataType type,               // Data type (unsigned 8, 16 or 32 bit)
                                   uint32_t *dataIndex,         // Index in output array of input data
                                   uint32_t *ADCIndex,          // Index of ADC to use for each input data point
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
    resetFrame2_ = callback_->allocateBuffer();
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

  // Reset error stats
  resetErrorStats_.duplicatePackets = 0;
  resetErrorStats_.missingPackets = 0;
  resetErrorStats_.latePackets = 0;

  // Calculate the number of expected packets
  uint32_t expectedPackets = byteSize_ / packetSize_;
  if (byteSize_ % packetSize_ != 0){
    expectedPackets++;
  }
  checker_->init(1, expectedPackets);
  resetChecker_->init(1, expectedPackets);
  receiver_ = new DataReceiver();
  // Set debug level of the receiver class
  receiver_->setDebug(debug_);
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
  if (callback_){
    callback_->releaseBuffer(fullFrame_);
    callback_->releaseBuffer(resetFrame1_);
    callback_->releaseBuffer(resetFrame2_);
  }

  // Log error statistics
  dbg.log(0, "Duplicate Packets    ", errorStats_.duplicatePackets);
  dbg.log(0, "Missing Packets      ", errorStats_.missingPackets);
  dbg.log(0, "Late Packets         ", errorStats_.latePackets);
  dbg.log(0, "[R] Duplicate Packets", resetErrorStats_.duplicatePackets);
  dbg.log(0, "[R] Missing Packets  ", resetErrorStats_.missingPackets);
  dbg.log(0, "[R] Late Packets     ", resetErrorStats_.latePackets);

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
    PercivalDebug dbg(debug_, "PercivalServer::imageReceived");
    dbg.log(1, "Frame Number    ", frameNumber);
    dbg.log(1, "Sub-Frame Number", (uint32_t)subFrameNumber);
    dbg.log(1, "Packet Number   ", packetNumber);
    dbg.log(1, "Packet Type     ", (uint32_t)packetType);
    dbg.log(1, "Buffer Address  ", (int64_t)buffer);

    // Forward this packet onto the appropriate routine according to whether it is reset data or frame data
    if (packetType == 0){
      // This is frame data
      framePacketReceived(buffer, bytes, frameNumber, subFrameNumber, packetNumber, packetType);
    } else if (packetType == 1){
      // This is reset data
      resetPacketReceived(buffer, bytes, frameNumber, subFrameNumber, packetNumber, packetType);
    }
  } // UNLOCK
}

void PercivalServer::resetPacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType)
{
  PercivalDebug dbg(debug_, "PercivalServer::resetPacketReceived");
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
      resetChecker_->report();
    }

    // Descramble the packet as necessary
    //PercivalSubFrame *sfPtr = subFrameMap_[0];
    int pixelsPerPacket = packetSize_ / 2;
    int offset = pixelsPerPacket * packetNumber;
    int numPts = bytes / 2;
    dbg.log(1, "Pts/Packet ", (uint32_t)pixelsPerPacket);
    dbg.log(1, "Offset     ", (uint32_t)offset);
    dbg.log(1, "Num Pts    ", (uint32_t)numPts);

    uint16_t *ptr1 = (uint16_t *)(resetFrame1_->raw()) + offset;
    memcpy(ptr1, buffer->raw(), bytes);

    // Release the Percival buffer back into the pool
    buffers_->free(buffer);


    if (resetChecker_->checkAll()){
      dbg.log(2, "Notify reset frame complete");
      resetChecker_->resetAll();
      // Reset Frame is ready
      resetFrameReady_ = resetFrameNumber_;
      // Set the flag to expect a new frame
      expectNewResetFrame_ = 1;
    }


}

void PercivalServer::framePacketReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType)
{
  PercivalDebug dbg(debug_, "PercivalServer::framePacketReceived");
  int status = 0;

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
dbg.log(0, "Late packet");
dbg.log(0, "Current Frame", frameNumber_);
dbg.log(0, "New Frame    ", frameNumber);
dbg.log(0, "Packet       ", packetNumber);
        // Release the Percival buffer back into the pool
        buffers_->free(buffer);
        return;
      } else if (frameNumber > frameNumber_){
        // We have a new frame before we expected it.  Notify immediately
        // And record the error as missing packets
        errorStats_.missingPackets++;
dbg.log(0, "Missing packet");
dbg.log(0, "Current Frame", frameNumber_);
dbg.log(0, "New Frame    ", frameNumber);
dbg.log(0, "Packet       ", packetNumber);

        checker_->resetAll();
        if (callback_){
          callback_->imageReceived(fullFrame_, 0, frameNumber_, 0, 0, 0);
          fullFrame_ = callback_->allocateBuffer();
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
dbg.log(0, "Duplicate packet");
dbg.log(0, "Current Frame", frameNumber_);
dbg.log(0, "New Frame    ", frameNumber);
dbg.log(0, "Packet       ", packetNumber);
        checker_->report();
    }

    // Descramble the packet as necessary
    //PercivalSubFrame *sfPtr = subFrameMap_[0];
    int pixelsPerPacket = packetSize_ / 2;
    int offset = pixelsPerPacket * packetNumber;
    int numPts = bytes / 2;
    dbg.log(1, "Pts/Packet ", (uint32_t)pixelsPerPacket);
    dbg.log(1, "Offset     ", (uint32_t)offset);
    dbg.log(1, "Num Pts    ", (uint32_t)numPts);

/*
    unscramble(offset,
               numPts,
               (uint16_t *)buffer->raw(),
               NULL,
               (uint16_t *)fullFrame_->raw(),
               sfPtr->getTopLeftX(),
               sfPtr->getBottomRightX(),
               sfPtr->getTopLeftY());
*/
uint16_t *ptr1 = (uint16_t *)(rawFrame_->raw()) + offset;
memcpy(ptr1, buffer->raw(), bytes);

    // Release the Percival buffer back into the pool
    buffers_->free(buffer);


    if (checker_->checkAll()){
      dbg.log(2, "Notify frame complete");
      checker_->resetAll();
      workerThread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PercivalServer::processFrame, this, frameNumber_)));
      // Set the flag to expect a new frame
      expectNewFrame_ = 1;
    }

}

void PercivalServer::processFrame(uint16_t frameNumber)
{
  PercivalDebug dbg(debug_, "PercivalServer::processFrame");

    // Descramble the packet as necessary
    PercivalSubFrame *sfPtr = subFrameMap_[currentSubFrame_];

    unscramble(0,
               pixelSize_,
               (uint16_t *)rawFrame_->raw(),
               (uint16_t *)resetFrame2_->raw(),
               (uint16_t *)fullFrame_->raw(),
               sfPtr->getTopLeftX(),
               sfPtr->getBottomRightX(),
               sfPtr->getTopLeftY());
    if (callback_){
      callback_->imageReceived(fullFrame_, 0, frameNumber, 0, 0, 0);
      fullFrame_ = callback_->allocateBuffer();
      // At this point copy reset frame 1 into reset frame 2 ready for use
      // After processing is complete we should have the latest reset frame
      // present in reset buffer 1.
      // Verify we have a reset frame ready with the same frame number
      if (resetFrameReady_ != (uint32_t)(frameNumber+1)){
        // The was a problem with the reset frame, for now just log
        dbg.log(0, "ERROR: Reset frame was not ready", frameNumber);
        dbg.log(0, "Reset frame number", resetFrameReady_);
      }
      memcpy(resetFrame2_->raw(), resetFrame1_->raw(), byteSize_);
    }

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

void PercivalServer::unscramble(int      offset,      // Offset from index 0
                                int      numPts,      // Number of points to process
                                uint8_t *in_data,     // Input data
                                uint8_t *reset_data,  // Reset data
                                uint8_t *out_data,    // Output data
                                uint32_t x1,
                                uint32_t x2,
                                uint32_t y1)
{
  PercivalDebug dbg(debug_, "PercivalServer::unscramble");
  int index;  // Index of point in subframe
  int xp;     // X prime, x coordinate within subframe
  int yp;     // Y prime, y coordinate within subframe
  int ip;     // Index prime, index of point within full frame

  for (index = offset; index < (numPts+offset); index++ ){
    yp = index / (x2 - x1 + 1);
    xp = index - (yp * (x2 - x1 + 1));
    ip = (width_ * (yp + y1)) + x1 + xp;

    // Check if we are descrambling or simply reconstructing
    if (!descramble_){
      // OK, here we are just reconstructing the original raw frame
      out_data[ip] = in_data[index];
    } else {
      // Here we are going to descramble
      out_data[dataIndex_[ip]] = in_data[index];
    }
  }
}

void PercivalServer::unscramble(int      offset,       // Offset from index 0
                                int      numPts,       // Number of points to process
                                uint16_t *in_data,     // Input data
                                uint16_t *reset_data,  // Reset data
                                uint16_t *out_data,    // Output data
                                uint32_t x1,
                                uint32_t x2,
                                uint32_t y1)
{
  PercivalDebug dbg(debug_, "PercivalServer::unscramble");
  int index;     // Index of point in subframe
  int rawindex;  // Index of point in packet
  int xp;        // X prime, x coordinate within subframe
  int yp;        // Y prime, y coordinate within subframe
  int ip;        // Index prime, index of point within full frame
  uint32_t gain;
  uint32_t ADC_low;
  uint32_t ADC_high;
  uint32_t ADC;
  float ADC_output;

  for (index = offset; index < (numPts+offset); index++ ){
//    yp = index / (x2 - x1 + 1);            // Y point within subframe
//    xp = index - (yp * (x2 - x1 + 1));     // X point within subframe
//    ip = (width_ * (yp + y1)) + x1 + xp;
//    rawindex = index - offset;

      rawindex = index;
      ip = index;

//std::cout << "Calculated x [" << xp << "]  y [" << yp << "]  index [" << ip << "]  rawindex [" << rawindex << "]  value [" << in_data[rawindex] << "]" << std::endl;

    // Check if we are descrambling or simply reconstructing
    if (!descramble_){
      // OK, here we are just reconstructing the original raw frame
//      out_data[ip] = in_data[rawindex];
      out_data[rawindex] = in_data[rawindex];
    } else {
      // Here we are going to descramble

      gain     = in_data[rawindex] & 0x3;
      ADC_low  = ( in_data[rawindex] >> 2 ) & 0xFF;
      ADC_high = ( in_data[rawindex] >> 10 ) & 0x1F;
      ADC      = ADCIndex_[dataIndex_[ip]];
      
      ADC_output = (ADC_high * ADCHighGain_[ADC] + ADC_low * ADCLowGain_[ADC] + ADCOffset_[ADC]);
      out_data[dataIndex_[ip]] = ADC_output * stageGains_[gain*pixelSize_+dataIndex_[ip]] + stageOffsets_[gain*pixelSize_+dataIndex_[ip]];

      if (gain == 0) {
        // Subtract DCS Reset signal if in diode sampling mode 
        ADC_low  = ( reset_data[ip] >> 2 ) & 0xFF;
        ADC_high = ( reset_data[ip] >> 10 ) & 0x1F;
        ADC_output = (ADC_high * ADCHighGain_[ADC] + ADC_low * ADCLowGain_[ADC] + ADCOffset_[ADC]);
        out_data[dataIndex_[ip]] -= ADC_output * stageGains_[gain*pixelSize_+dataIndex_[ip]] + stageOffsets_[gain*pixelSize_+dataIndex_[ip]];
      }
    }
  }
}

void PercivalServer::unscramble(int      offset,       // Offset from index 0
                                int      numPts,       // Number of points to process
                                uint32_t *in_data,     // Input data
                                uint32_t *reset_data,  // Reset data
                                uint32_t *out_data,    // Output data
                                uint32_t x1,
                                uint32_t x2,
                                uint32_t y1)
{
  PercivalDebug dbg(debug_, "PercivalServer::unscramble");
  int index;  // Index of point in subframe
  int xp;     // X prime, x coordinate within subframe
  int yp;     // Y prime, y coordinate within subframe
  int ip;     // Index prime, index of point within full frame

  for (index = offset; index < (numPts+offset); index++ ){
    yp = index / (x2 - x1 + 1);
    xp = index - (yp * (x2 - x1 + 1));
    ip = (width_ * (yp + y1)) + x1 + xp;

    // Check if we are descrambling or simply reconstructing
    if (!descramble_){
      // OK, here we are just reconstructing the original raw frame
      out_data[ip] = in_data[index];
    } else {
      // Here we are going to descramble
      out_data[dataIndex_[ip]] = in_data[index];
    }
  }
}


