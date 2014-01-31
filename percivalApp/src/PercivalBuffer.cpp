/*
 * PercivalBuffer.cpp
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 *
 *  This class provides a simple wrapper around some memory.
 *  Used with a PercivalBufferPool
 */

#include "PercivalBuffer.h"

PercivalBuffer::PercivalBuffer(uint32_t bufferSize)
{
  buffer_ = malloc(bufferSize);
  bufferSize_ = bufferSize;
  allocated_ = true;
}

PercivalBuffer::PercivalBuffer(uint32_t bufferSize, void *bufferPtr)
{
  buffer_ = bufferPtr;
  bufferSize_ = bufferSize;
  allocated_ = false;
}

PercivalBuffer::~PercivalBuffer()
{
  if (allocated_){
    free(buffer_);
  }
}

void *PercivalBuffer::raw()
{
  return buffer_;
}

uint32_t PercivalBuffer::size()
{
  return bufferSize_;
}

void PercivalBuffer::setUserPtr(void *userPtr)
{
  userPtr_ = userPtr;
}

void *PercivalBuffer::getUserPtr()
{
  return userPtr_;
}

