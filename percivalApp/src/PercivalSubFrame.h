/*
 * PercivalSubFrame.h
 *
 *  Created on: 20th Aug 2013
 *      Author: gnx91527
 *
 *  This class contains one UDP server and associated data to
 *  simplfy managing multiple servers.
 */

#ifndef PERCIVALSUBFRAME_H_
#define PERCIVALSUBFRAME_H_

#include <boost/cstdint.hpp>

class PercivalSubFrame
{
  public:

	  PercivalSubFrame(uint32_t subFrameID,
                     uint32_t topLeftX,
                     uint32_t topLeftY,
                     uint32_t bottomRightX,
                     uint32_t bottomRightY);

    virtual ~PercivalSubFrame();

    void setDebug(uint32_t level);

    uint32_t getNumberOfPixels();

    uint32_t getTopLeftX();

    uint32_t getTopLeftY();

    uint32_t getBottomRightX();

    uint32_t getBottomRightY();

  private:

    uint32_t       debug_;            // Debug level
    uint32_t       subFrameID_;       // Numerical unique ID for this sub-frame
    uint32_t       topLeftX_;         // Sub frame top left X
    uint32_t       topLeftY_;         // Sub frame top left Y
    uint32_t       bottomRightX_;     // Sub frame bottom right X
    uint32_t       bottomRightY_;     // Sub frame bottom right Y
    uint32_t       width_;            // Width of this sub-frame
    uint32_t       height_;           // Height of this sub-frame
    uint32_t       pixelSize_;        // Number of pixels in this sub-frame
    
};

#endif  /* PERCIVALSUBFRAME_H_ */


