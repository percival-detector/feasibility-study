/*
 * PercivalBufferPool.h
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 *
 *  This class is a low cost manager for buffers.
 */

#ifndef PERCIVALBUFFERPOOL_H_
#define PERCIVALBUFFERPOOL_H_

#include "PercivalBuffer.h"

#include <stack>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

class PercivalBufferPool
{
  public:

    PercivalBufferPool(uint32_t bufferSize);
    ~PercivalBufferPool();

    PercivalBuffer *allocate();
    void free(PercivalBuffer *buffer);

  private:

    boost::mutex                 access_;
    uint32_t                     bufferSize_;
    std::stack<PercivalBuffer *> freeBuffers_;

};

#endif /* PERCIVALBUFFERPOOL_H_ */

