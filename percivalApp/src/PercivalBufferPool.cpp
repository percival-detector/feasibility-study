/*
 * PercivalBufferPool.cpp
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 *
 *  This class is a low cost manager for buffers.
 */

#include "PercivalBufferPool.h"

PercivalBufferPool::PercivalBufferPool(uint32_t bufferSize)
{
  bufferSize_ = bufferSize;
}

PercivalBufferPool::~PercivalBufferPool()
{
  boost::lock_guard<boost::mutex> lock(access_);
  // Iterate over buffer stack and delete each buffer
}

PercivalBuffer *PercivalBufferPool::allocate()
{
  boost::lock_guard<boost::mutex> lock(access_);
  PercivalBuffer *ptr;
  if (freeBuffers_.empty()){
std::cout << "Freebuffers empty" << std::endl;
    ptr = new PercivalBuffer(bufferSize_);
  } else {
std::cout << "Freebuffers NOT empty, popping off the stack" << std::endl;
    ptr = freeBuffers_.top();
    freeBuffers_.pop();
  }
  return ptr;
}

void PercivalBufferPool::free(PercivalBuffer *buffer)
{
  boost::lock_guard<boost::mutex> lock(access_);
  freeBuffers_.push(buffer);
}

