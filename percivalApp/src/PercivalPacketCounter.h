/*
 * PercivalPacketCounter.h
 *
 *  Created on: 13th Nov 2013
 *      Author: gnx91527
 */

#ifndef PERCIVAL_PACKET_COUNTER_H_
#define PERCIVAL_PACKET_COUNTER_H_

#include <time.h>
#include <list>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/exception/diagnostic_information.hpp>

class PercivalPacketCounter
{
  public:

	  PercivalPacketCounter(uint32_t history);
    virtual ~PercivalPacketCounter();

    void addPacket(//boost::posix_time::ptime ts,
                   //std::size_t bytesReceived,
                   uint16_t frameNumber,
                   uint8_t  subFrameNumber,
                   uint16_t packetNumber,
                   uint8_t  packetType);

    void report();

  private:

    uint32_t                            history_;
    uint32_t                            index_;
    uint32_t                            wrap_;

    //std::list<boost::posix_time::ptime> ts_;
//    std::list<std::size_t>              bytes_;
//    std::list<uint16_t>                 frameNumber_;
//    std::list<uint8_t>                  subFrameNumber_;
//    std::list<uint16_t>                 packetNumber_;
//    std::list<uint8_t>                  packetType_;

    std::size_t              *bytes_;
    uint16_t                 *frameNumber_;
    uint8_t                  *subFrameNumber_;
    uint16_t                 *packetNumber_;
    uint8_t                  *packetType_;
};

#endif /* PERCIVAL_PACKET_COUNTER_H_ */

