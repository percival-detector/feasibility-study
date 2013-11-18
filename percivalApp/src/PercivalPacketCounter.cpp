/*
 * PercivalPacketCounter.h
 *
 *  Created on: 13th Nov 2013
 *      Author: gnx91527
 */

#include "PercivalPacketCounter.h"

PercivalPacketCounter::PercivalPacketCounter(uint32_t history)
	: history_(history),
    index_(0),
    wrap_(0)
{
  bytes_ = (std::size_t *)malloc(history * sizeof(std::size_t));
  frameNumber_ = (uint16_t *)malloc(history * sizeof(uint16_t));
  subFrameNumber_ = (uint8_t *)malloc(history * sizeof(uint8_t));
  packetNumber_ = (uint16_t *)malloc(history * sizeof(uint16_t));
  packetType_ = (uint8_t *)malloc(history * sizeof(uint8_t));
}

PercivalPacketCounter::~PercivalPacketCounter()
{
}

void PercivalPacketCounter::addPacket(//boost::posix_time::ptime ts,
                                      //std::size_t bytesReceived,
                                      uint16_t frameNumber,
                                      uint8_t  subFrameNumber,
                                      uint16_t packetNumber,
                                      uint8_t  packetType)
{

  frameNumber_[index_] = frameNumber;
  subFrameNumber_[index_] = subFrameNumber;
  packetNumber_[index_] = packetNumber;
  packetType_[index_] = packetType;
  index_++;

  if (index_ == history_){
    index_ = 0;
    wrap_ = 1;
  }

  /*ts_.push_front(ts);
  if (ts_.size() > history_){
    ts_.pop_back();
  }*/

  /*bytes_.push_front(bytesReceived);
  if (bytes_.size() > history_){
    bytes_.pop_back();
  }*/

  /*frameNumber_.push_front(frameNumber);
  if (frameNumber_.size() > history_){
    frameNumber_.pop_back();
  }

  subFrameNumber_.push_front(subFrameNumber);
  if (subFrameNumber_.size() > history_){
    subFrameNumber_.pop_back();
  }

  packetNumber_.push_front(packetNumber);
  if (packetNumber_.size() > history_){
    packetNumber_.pop_back();
  }

  packetType_.push_front(packetType);
  if (packetType_.size() > history_){
    packetType_.pop_back();
  }*/

}

void PercivalPacketCounter::report()
{
  std::cout << "Bytes     FrameNo   SubFrameNo PacketNo PacketType" << std::endl;
  if (wrap_ == 0){
    for (uint32_t index = 0; index < index_; index++){
      std::cout << std::setw(10) << (uint32_t)frameNumber_[index] << " ";
      std::cout << std::setw(10) << (uint32_t)subFrameNumber_[index] << " ";
      std::cout << std::setw(10) << (uint32_t)packetNumber_[index] << " ";
      std::cout << std::setw(10) << (uint32_t)packetType_[index] << std::endl;
    }
  } else {
    for (uint32_t index = index_; index < history_; index++){
      std::cout << std::setw(10) << (uint32_t)frameNumber_[index] << " ";
      std::cout << std::setw(10) << (uint32_t)subFrameNumber_[index] << " ";
      std::cout << std::setw(10) << (uint32_t)packetNumber_[index] << " ";
      std::cout << std::setw(10) << (uint32_t)packetType_[index] << std::endl;
    }
    for (uint32_t index = 0; index < index_; index++){
      std::cout << std::setw(10) << (uint32_t)frameNumber_[index] << " ";
      std::cout << std::setw(10) << (uint32_t)subFrameNumber_[index] << " ";
      std::cout << std::setw(10) << (uint32_t)packetNumber_[index] << " ";
      std::cout << std::setw(10) << (uint32_t)packetType_[index] << std::endl;
    }
  }

/*
  //std::list<boost::posix_time::ptime>::iterator it1 = ts_.begin();
  std::list<std::size_t>::iterator              it2 = bytes_.begin();
  std::list<uint16_t>::iterator                 it3 = frameNumber_.begin();
  std::list<uint8_t>::iterator                  it4 = subFrameNumber_.begin();
  std::list<uint16_t>::iterator                 it5 = packetNumber_.begin();
  std::list<uint8_t>::iterator                  it6 = packetType_.begin();

  std::cout << "Bytes FrameNo SubFrameNo PacketNo PacketType" << std::endl;
  while (it3 != frameNumber_.end()){

//    std::cout << it1->time_of_day().total_microseconds() << " ";
//    std::cout << std::dec << std::setw(10) << *it2 << " ";
    std::cout << std::setw(10) << (uint32_t)*it3 << " ";
    std::cout << std::setw(10) << (uint32_t)*it4 << " ";
    std::cout << std::setw(10) << (uint32_t)*it5 << " ";
    std::cout << std::setw(10) << (uint32_t)*it6 << std::endl;
//    it1++;
//    it2++;
    it3++;
    it4++;
    it5++;
    it6++;
  }
*/
}


