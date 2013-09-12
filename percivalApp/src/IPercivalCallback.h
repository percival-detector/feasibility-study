/*
 * IPercivalCallback.h
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 */

#ifndef IPERCIVALCALLBACK_H_
#define IPERCIVALCALLBACK_H_

#include "PercivalBuffer.h"

#include <boost/cstdint.hpp>

typedef enum
{
  imageComplete,
  imageCorrupt
} CallbackType;


class IPercivalCallback
{
  public:
    virtual ~IPercivalCallback() {};
    
    virtual void imageReceived(PercivalBuffer *buffer, uint32_t frameNumber) = 0;

    virtual PercivalBuffer *allocateBuffer() = 0;
};






#endif /* IPERCIVALCALLBACK_H_ */

