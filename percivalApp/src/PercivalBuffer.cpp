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
}

PercivalBuffer::~PercivalBuffer()
{
  free(buffer_);
}

void *PercivalBuffer::raw()
{
  return buffer_;
}

uint32_t PercivalBuffer::size()
{
  return bufferSize_;
}
