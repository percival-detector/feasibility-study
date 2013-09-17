/*
 * PercivalSubFrame.h
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 *
 *  This class contains one UDP server and associated data to
 *  simplfy managing multiple servers.
 */

#include "PercivalSubFrame.h"
#include "PercivalDebug.h"

PercivalSubFrame::PercivalSubFrame(PercivalServer *owner,
                                   uint32_t frameID,
                                   const std::string& host,
                                   unsigned short port,
                                   DataType type,
                                   uint32_t topLeftX,
                                   uint32_t topLeftY,
                                   uint32_t bottomRightX,
                                   uint32_t bottomRightY,
                                   uint32_t subFrames)
	: debug_(0),
    watchdogTimeout_(2000)
{
  owner_ = owner;
  frameID_ = frameID;
  host_ = host;
  port_ = port;
  topLeftX_ = topLeftX;
  topLeftY_ = topLeftY;
  bottomRightX_ = bottomRightX;
  bottomRightY_ = bottomRightY;
  subFrames_ = subFrames;
  width_ = bottomRightX - topLeftX + 1;
  height_ = bottomRightY - topLeftY + 1;
  type_ = type;
  pixelSize_ = width_ * height_;
  byteSize_ = pixelSize_ * (uint32_t)pow(2.0, (double)type);

  // Create the buffer pool
  buffers_ = new PercivalBufferPool(byteSize_);
  // Create the data receiver
  server_ = new DataReceiver();
  // Register this class as the callback
  server_->registerCallback(this);
  // Setup the socket for receiving
  server_->setupSocket(host, port);
}

PercivalSubFrame::~PercivalSubFrame()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::~PercivalSubFrame");
}

void PercivalSubFrame::setDebug(uint32_t level)
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::setDebug");
  dbg.log(1, "Debug level", level);
  debug_ = level;
  // Set the debug level
  server_->setDebug(level);
}

void PercivalSubFrame::setWatchdogTimeout(uint32_t time)
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::setWatchdogTimeout");
  dbg.log(1, "Watchdog timeout", time);
  watchdogTimeout_ = time;
  // Set the watchdog timeout
  server_->setWatchdogTimeout(time);
}

uint32_t PercivalSubFrame::getNumberOfPixels()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::getNumberOfPixels");
  return pixelSize_;
}

uint32_t PercivalSubFrame::getTopLeftX()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::getTopLeftX");
  return topLeftX_;
}

uint32_t PercivalSubFrame::getTopLeftY()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::getTopLeftY");
  return topLeftY_;
}

uint32_t PercivalSubFrame::getBottomRightX()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::getBottomRightX");
  return bottomRightX_;
}

uint32_t PercivalSubFrame::getBottomRightY()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::getBottomRightY");
  return bottomRightY_;
}

void PercivalSubFrame::startAcquisition()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::startAcquisition");
  server_->startAcquisition(byteSize_, subFrames_);
}

void PercivalSubFrame::stopAcquisition()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::stopAcquisition");
  server_->stopAcquisition();
  server_->shutdownSocket();
}

void PercivalSubFrame::imageReceived(PercivalBuffer *buffer, uint32_t frameNumber)
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::imageReceived");
  dbg.log(1, "Thread ID", boost::this_thread::get_id());
  owner_->processSubFrame(frameID_, buffer, frameNumber);
  buffers_->free(buffer);
}

void PercivalSubFrame::timeout()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::timeout");
  dbg.log(1, "Thread ID", boost::this_thread::get_id());
  owner_->timeout(frameID_);
}


PercivalBuffer *PercivalSubFrame::allocateBuffer()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::allocateBuffer");
  PercivalBuffer *buffer = buffers_->allocate();
  dbg.log(1, "Allocated buffer address", (int64_t)buffer);
  return buffer;
}



