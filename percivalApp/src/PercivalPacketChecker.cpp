/*
 * DataSender.cpp
 *
 *  Created on: 5th Aug 2013
 *      Author: gnx91527
 */

#include "PercivalPacketChecker.h"
#include "PercivalDebug.h"

PercivalPacketChecker::PercivalPacketChecker()
  : checks_(0),
    test_(0),
    debug_(0),
    parcels_(0),
    packets_(0)
{
}

PercivalPacketChecker::~PercivalPacketChecker()
{
  PercivalDebug dbg(debug_, "PercivalPacketChecker::~PercivalPacketChecker");
}

void PercivalPacketChecker::setDebug(uint32_t level)
{
  PercivalDebug dbg(debug_, "PercivalPacketChecker::setDebug");
  dbg.log(1, "Debug level", level);
  debug_ = level;
}

void PercivalPacketChecker::init(uint32_t parcels, uint32_t packets)
{
  PercivalDebug dbg(debug_, "PercivalPacketChecker::init");
  dbg.log(1, "Parcels", parcels);
  dbg.log(1, "Packets", packets);

  // Record variables
  parcels_ = parcels;
  packets_ = packets;
  uint32_t bytesize = parcels * packets;
  counter_ = 0;
  passed_ = bytesize;
  // Allocate storage for the mask
  if (checks_){
    free(checks_);
  }
  checks_ = (char *)malloc(bytesize * sizeof(char));
  // Zero the mask
  memset(checks_, 0, bytesize * sizeof(char));
  // Allocate test result
  if (test_){
    free(test_);
  }
  test_ = (char *)malloc(bytesize * sizeof(char));
  // Set the test to full packets recevied
  memset(test_, 1, bytesize * sizeof(char));
}

void PercivalPacketChecker::report()
{
  std::cout << "Current Mask---------------------" << std::endl;
  for (uint32_t index = 0; index < (parcels_ * packets_); index++){
    std::cout << (int)checks_[index];
  }
  std::cout << std::endl;
  //for (uint32_t index = 0; index < (parcels_ * packets_); index++){
  //  std::cout << (int)test_[index];
  //}
  //std::cout << std::endl;
  std::cout << "Packet count: " << counter_ << std::endl;

}

int PercivalPacketChecker::set(uint32_t parcel, uint32_t packet)
{
  uint32_t index = (parcel * packets_) + packet;
  // Check the flag is not already present
  if (checks_[index] == 1){
    // Bad situation, duplicate packet
    return PP_ERR_DUP_PKT;
  }
  // Set the flag now
  checks_[index] = 1;
  counter_++;
  return PP_OK;
}

int PercivalPacketChecker::reset(uint32_t parcel, uint32_t packet)
{
  uint32_t index = (parcel * packets_) + packet;
  // Check the flag is present
  if (checks_[index] == 0){
    // Bad situation, no packet set
    return PP_ERR_MIS_PKT;
  }
  // Set the flag now
  checks_[index] = 0;
  counter_--;
  return PP_OK;
}

int PercivalPacketChecker::check(uint32_t parcel, uint32_t packet)
{
  uint32_t index = (parcel * packets_) + packet;
  return checks_[index];
}

int PercivalPacketChecker::getCount()
{
  return counter_;
}

int PercivalPacketChecker::resetAll()
{
  uint32_t bytesize = parcels_ * packets_;
  // Zero the mask
  memset(checks_, 0, bytesize * sizeof(char));
  // Reset the counter
  counter_ = 0;
  return PP_OK;
}

int PercivalPacketChecker::checkAll()
{
//  if (memcmp(checks_, test_, (parcels_ * packets_))){
//    return 0;
//  }
  if (counter_ != passed_){
    return 0;
  }
  return 1;
}





