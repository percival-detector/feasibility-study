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
    errorMessage_(""),
    acquiring_(false),
    serverMask_(0),
    serverReady_(0),
    callback_(0)
{
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
  // Iterate over sub-frames and set debug levels
  std::map<int, PercivalSubFrame *>::const_iterator iterator;
  for(iterator = subFrameMap_.begin(); iterator != subFrameMap_.end(); iterator++){
    iterator->second->setDebug(level);
  }
}

std::string PercivalServer::errorMessage()
{
  PercivalDebug dbg(debug_, "PercivalServer::errorMessage");
  dbg.log(1, "Message", errorMessage_);
  return errorMessage_;
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

int PercivalServer::setupSubFrame(uint32_t frameID,
                                  const std::string& host,
                                  unsigned short port,
                                  uint32_t topLeftX,
                                  uint32_t topLeftY,
                                  uint32_t bottomRightX,
                                  uint32_t bottomRightY,
                                  uint32_t subFrames)
{
  PercivalDebug dbg(debug_, "PercivalServer::setupSubFrame");
  subFrameMap_[frameID] = new PercivalSubFrame(this,
                                               frameID,
                                               host,
                                               port,
                                               type_,
                                               topLeftX,
                                               topLeftY,
                                               bottomRightX,
                                               bottomRightY,
                                               subFrames);
  subFrameMap_[frameID]->setDebug(debug_);
  serverMask_ = serverMask_ + (1 << frameID);
  dbg.log(1, "Server Mask", serverMask_);
  return 0;
}

int PercivalServer::releaseSubFrame(int frameID)
{
  PercivalDebug dbg(debug_, "PercivalServer::releaseSubFrame");
  PercivalSubFrame *sfPtr = subFrameMap_[frameID];
  subFrameMap_.erase(frameID);
  delete(sfPtr);
  serverMask_ = serverMask_ - (1 << frameID);
  dbg.log(1, "Server Mask", serverMask_);
  return 0;
}

int PercivalServer::startAcquisition()
{
  PercivalDebug dbg(debug_, "PercivalServer::startAcquisition");
  std::map<int, PercivalSubFrame *>::const_iterator iterator;
  for(iterator = subFrameMap_.begin(); iterator != subFrameMap_.end(); iterator++){
    iterator->second->startAcquisition();
  }
  return 0;
}

int PercivalServer::stopAcquisition()
{
  PercivalDebug dbg(debug_, "PercivalServer::stopAcquisition");
  return 0;
}

int PercivalServer::registerCallback(IPercivalCallback *callback)
{
  PercivalDebug dbg(debug_, "PercivalServer::registerCallback");
  callback_ = callback;
  return 0;
}

int PercivalServer::processSubFrame(uint32_t frameID, PercivalBuffer *buffer)
{
  PercivalDebug dbg(debug_, "PercivalServer::processSubFrame");
  dbg.log(1, "Frame ID", frameID);
  dbg.log(1, "Buffer Address", (int64_t)buffer);
  // LOCK
  {
    boost::lock_guard<boost::mutex> lock(access_);
    // Check mask
    if (serverReady_ == 0){
      // If first entry on mask call for allocation
      if (callback_){
        fullFrame_ = callback_->allocateBuffer();
      }
    } else if (serverReady_ & (1 << frameID)){
      // If this frame already in mask then serious error
      dbg.log(0, "Two subframes with same ID, subframes out of order");
    }
  // UNLOCK
  }
  // Process data (DANGER CALL, PROCESSING ON SAME BUFFER CONCURRENTLY)
  dbg.log(1, "Processing data...");
  PercivalSubFrame *sfPtr = subFrameMap_[frameID];
  unscramble(sfPtr->getNumberOfPixels(),
             (uint16_t *)buffer->raw(),
             NULL,
             (uint16_t *)fullFrame_->raw(),
             sfPtr->getTopLeftX(),
             sfPtr->getBottomRightX(),
             sfPtr->getTopLeftY());

  // LOCK
  {
    boost::lock_guard<boost::mutex> lock(access_);
    // Update mask
    serverReady_ = serverReady_ + (1 << frameID);
    // If last call (mask full) then notify and zero mask
    if (serverReady_ == serverMask_){
      dbg.log(2, "Notify frame complete");
      if (callback_){
        callback_->imageReceived(fullFrame_);
      }
      serverReady_ = 0;
    }
  // UNLOCK
  }
  return 0;
}

void PercivalServer::unscramble(int      numPts,       // Number of points to process
                                uint16_t *in_data,     // Input data
                                uint16_t *reset_data,  // Reset data
                                uint16_t *out_data,    // Output data
                                uint32_t x1,
                                uint32_t x2,
                                uint32_t y1)
{
  PercivalDebug dbg(debug_, "PercivalServer::unscramble");
  int index; // Index of point in subframe
  int xp;    // X prime, x coordinate within subframe
  int yp;    // Y prime, y coordinate within subframe
  int ip;    // Index prime, index of point within full frame
  //unsigned int gain, ADC_low, ADC_high, ADC;
  //float ADC_output;

  for (index = 0; index < numPts; index++ ){
    yp = index / (x2 - x1 + 1);
    xp = index - (yp * (x2 - x1 + 1));
    ip = (width_ * (yp + y1)) + x1 + xp;

//std::cout << "Calculated x [" << xp << "]  y [" << yp << "]  index [" << ip << "]" << std::endl;

    out_data[ip] = in_data[index];

/*
    gain     = in_data[ip] & 0x3;
    ADC_low  = ( in_data[ip] >> 2 ) & 0xFF;
    ADC_high = ( in_data[ip] >> 10 ) & 0x1F;
    ADC      = ADCIndex_[ip];
        
    ADC_output = (ADC_high * ADCHighGain_[ADC] + ADC_low * ADCLowGain_[ADC] + ADCOffset_[ADC]);
    out_data[dataIndex_[ip]] = ADC_output * stageGains_[gain][ip] + stageOffsets_[gain][ip];
        
    if (gain == 0) {
      // Subtract DCS Reset signal if in diode sampling mode 
      ADC_low  = ( reset_data[ip] >> 2 ) & 0xFF;
      ADC_high = ( reset_data[ip] >> 10 ) & 0x1F;
      ADC_output = (ADC_high * ADCHighGain_[ADC] + ADC_low * ADCLowGain_[ADC] + ADCOffset_[ADC]);
      out_data[dataIndex_[ip]] -= ADC_output * stageGains_[0][ip] + stageOffsets_[0][ip];
    }*/
  }
}


