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
//std::cout << "Size: " << freeBuffers_.size() << std::endl;
//  while (!freeBuffers_.empty()){
//    PercivalBuffer *ptr = freeBuffers_.top();
//    freeBuffers_.pop();
//    delete ptr;
//  }
}

PercivalBuffer *PercivalBufferPool::allocate()
{
  boost::lock_guard<boost::mutex> lock(access_);
  PercivalBuffer *ptr;
  if (freeBuffers_.empty()){
    ptr = new PercivalBuffer(bufferSize_);
  } else {
    ptr = freeBuffers_.top();
    freeBuffers_.pop();
  }
  return ptr;
}

PercivalBuffer *PercivalBufferPool::allocateClean()
{
  boost::lock_guard<boost::mutex> lock(access_);
  PercivalBuffer *ptr;
  if (freeBuffers_.empty()){
    ptr = new PercivalBuffer(bufferSize_);
  } else {
    ptr = freeBuffers_.top();
    freeBuffers_.pop();
  }
  memset(ptr->raw(), 0, bufferSize_);
  return ptr;
}

void PercivalBufferPool::free(PercivalBuffer *buffer)
{
  boost::lock_guard<boost::mutex> lock(access_);
  memset(buffer->raw(), 0, bufferSize_);
  freeBuffers_.push(buffer);
}

int PercivalBufferPool::size()
{
  boost::lock_guard<boost::mutex> lock(access_);
  return freeBuffers_.size();
}


