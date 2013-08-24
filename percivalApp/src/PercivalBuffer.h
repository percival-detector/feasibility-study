/*
 * PercivalBuffer.h
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 *
 *  This class provides a simple wrapper around some memory.
 *  Used with a PercivalBufferPool
 */

#ifndef PERCIVALBUFFER_H_
#define PERCIVALBUFFER_H_

#include <boost/cstdint.hpp>

class PercivalBuffer
{
  public:
    PercivalBuffer(uint32_t bufferSize);
    ~PercivalBuffer();
    void *raw();

  private:

    void *buffer;

};

#endif /* PERCIVALBUFFER_H_ */

