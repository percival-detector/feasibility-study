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

PercivalSubFrame::PercivalSubFrame(uint32_t subFrameID,
                                   uint32_t topLeftX,
                                   uint32_t topLeftY,
                                   uint32_t bottomRightX,
                                   uint32_t bottomRightY)
	: debug_(0)
{
  subFrameID_ = subFrameID;
  topLeftX_ = topLeftX;
  topLeftY_ = topLeftY;
  bottomRightX_ = bottomRightX;
  bottomRightY_ = bottomRightY;
  width_ = bottomRightX - topLeftX + 1;
  height_ = bottomRightY - topLeftY + 1;
  pixelSize_ = width_ * height_;
  buffers_ = new PercivalBufferPool(pixelSize_ * sizeof(uint16_t));
}

PercivalSubFrame::~PercivalSubFrame()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::~PercivalSubFrame");
  delete buffers_;
}

void PercivalSubFrame::setDebug(uint32_t level)
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::setDebug");
  dbg.log(1, "Debug level", level);
  debug_ = level;
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

PercivalBuffer *PercivalSubFrame::allocate()
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::allocate");
  return buffers_->allocate();
}

void PercivalSubFrame::release(PercivalBuffer *buffer)
{
  PercivalDebug dbg(debug_, "PercivalSubFrame::release");
  buffers_->free(buffer);
}

