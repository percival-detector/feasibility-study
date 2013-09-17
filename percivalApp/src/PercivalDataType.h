/*
 * PercivalDataType.h
 *
 *  Created on: 21st Aug 2013
 *      Author: gnx91527
 *
 *  This header simply defines the data type enumeration
 *  and the packet structure used by the sender and receiver
 */

#ifndef PERCIVALDATATYPE_H_
#define PERCIVALDATATYPE_H_

#include <boost/cstdint.hpp>

typedef enum
{
  UnsignedInt8,
  UnsignedInt16,
  UnsignedInt32
} DataType;

typedef struct packetHeader_t
{
	uint32_t frameNumber;
	uint32_t subFrameNumber;
	uint32_t packetNumberFlags;
} PacketHeader;

const uint32_t kStartOfFrameMarker = 1 << 31;
const uint32_t kEndOfFrameMarker   = 1 << 30;
const uint32_t kPacketNumberMask   = 0x3FFFFFFF;

typedef enum
{
	headerAtStart,
	headerAtEnd
} DataSenderHeaderPosition;

typedef uint32_t FrameNumber;

#endif // PERCIVALDATATYPE_H_

