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
  // Set the debug level
  server_->setDebug(5);
  // Register this class as the callback
  server_->registerCallback(this);
  // Setup the socket for receiving
  server_->setupSocket(host, port);
}

PercivalSubFrame::~PercivalSubFrame()
{

}

uint32_t PercivalSubFrame::getNumberOfPixels()
{
  return pixelSize_;
}

uint32_t PercivalSubFrame::getTopLeftX()
{
  return topLeftX_;
}

uint32_t PercivalSubFrame::getTopLeftY()
{
  return topLeftY_;
}

uint32_t PercivalSubFrame::getBottomRightX()
{
  return bottomRightX_;
}

uint32_t PercivalSubFrame::getBottomRightY()
{
  return bottomRightY_;
}

void PercivalSubFrame::startAcquisition()
{
  server_->startAcquisition(byteSize_, subFrames_);
}

void PercivalSubFrame::imageReceived(PercivalBuffer *buffer)
{
  std::cout << "SubFrame worker Thread ID: " << boost::this_thread::get_id() << std::endl;
  owner_->processSubFrame(frameID_, buffer);
  buffers_->free(buffer);
}

PercivalBuffer *PercivalSubFrame::allocateBuffer()
{
  PercivalBuffer *buffer = buffers_->allocate();
std::cout << "Allocated buffer address: " << buffer << std::endl;
  return buffer;
}



