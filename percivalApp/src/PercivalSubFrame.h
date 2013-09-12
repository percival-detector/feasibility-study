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

#include "PercivalDataType.h"
#include "PercivalServer.h"
#include "DataReceiver.h"
#include "PercivalBuffer.h"
#include "PercivalBufferPool.h"

class PercivalServer;

class PercivalSubFrame : public IPercivalCallback
{
  public:

	  PercivalSubFrame(PercivalServer *owner,
                     uint32_t frameID,
                     const std::string& host,
                     unsigned short port,
                     DataType type,
                     uint32_t topLeftX,
                     uint32_t topLeftY,
                     uint32_t bottomRightX,
                     uint32_t bottomRightY,
                     uint32_t subFrames);

    virtual ~PercivalSubFrame();

    void setDebug(uint32_t level);

    uint32_t getNumberOfPixels();

    uint32_t getTopLeftX();

    uint32_t getTopLeftY();

    uint32_t getBottomRightX();

    uint32_t getBottomRightY();

    void startAcquisition();

    void stopAcquisition();

    virtual void imageReceived(PercivalBuffer *buffer, uint32_t frameNumber);

    virtual PercivalBuffer *allocateBuffer();


  private:

    uint32_t       debug_;        // Debug level
    uint32_t       frameID_;      // Numerical unique ID for this sub-frame
    std::string    host_;         // NIC to bind UDP socket to
    unsigned short port_;         // Port number for UDP socket to listen on
    DataType       type_;         // Data type of this sub-frame
    uint32_t       topLeftX_;     // Sub frame top left X
    uint32_t       topLeftY_;     // Sub frame top left Y
    uint32_t       bottomRightX_; // Sub frame bottom right X
    uint32_t       bottomRightY_; // Sub frame bottom right Y
    uint32_t       subFrames_;    // Number of sub-frames
    uint32_t       width_;        // Width of this sub-frame
    uint32_t       height_;       // Height of this sub-frame
    uint32_t       pixelSize_;    // Number of pixels in this sub-frame
    uint32_t       byteSize_;     // Number of bytes in this sub-frame
    
    PercivalServer     *owner_;
    PercivalBufferPool *buffers_;
    PercivalBuffer     *currentBuffer_;
    DataReceiver       *server_;


};

#endif  /* PERCIVALSUBFRAME_H_ */


