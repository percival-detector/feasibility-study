/*
 * PercivalPacketChecker.h
 *
 *  Created on: 18th Sep 2013
 *      Author: gnx91527
 */

#ifndef PERCIVAL_PACKET_CHECKER_H_
#define PERCIVAL_PACKET_CHECKER_H_

#include "PercivalDataType.h"

// Define some status codes
#define PP_OK           0   // Operation completed OK
#define PP_ERR_DUP_PKT -1   // Duplicate packet set in mask
#define PP_ERR_MIS_PKT -2   // Attempted to reset a packet that was already 0

class PercivalPacketChecker
{
  public:

	  PercivalPacketChecker();
    virtual ~PercivalPacketChecker();

    void setDebug(uint32_t level);

    void init(uint32_t parcels, uint32_t packets);

    void report();

    int set(uint32_t parcel, uint32_t packet);
    int reset(uint32_t parcel, uint32_t packet);
    int check(uint32_t parcel, uint32_t packet);

    int resetAll();
    int checkAll();

  private:

    char     *checks_;  // Enough masks to cover all packets of all parcels
    char     *test_;    // Test for valid mask
    uint32_t debug_;
    uint32_t parcels_;
    uint32_t packets_;

};

#endif /* PERCIVAL_PACKET_CHECKER_H_ */

