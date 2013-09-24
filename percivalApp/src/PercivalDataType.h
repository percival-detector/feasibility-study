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
	uint8_t  packetType;
	uint8_t  subFrameNumber;
	uint16_t frameNumber;
	uint16_t packetNumber;
} PacketHeader;

typedef enum
{
	headerAtStart,
	headerAtEnd
} DataSenderHeaderPosition;

typedef uint32_t FrameNumber;

#endif // PERCIVALDATATYPE_H_

