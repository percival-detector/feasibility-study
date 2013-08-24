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
  buffer = malloc(bufferSize);
}

PercivalBuffer::~PercivalBuffer()
{
  free(buffer);
}

void *PercivalBuffer::raw()
{
  return buffer;
}

