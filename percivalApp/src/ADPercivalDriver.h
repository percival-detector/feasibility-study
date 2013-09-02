/*
 * ADPercivalDriver.h
 *
 *  Created on: 24th Aug 2013
 *      Author: gnx91527
 */

#ifndef ADPERCIVALDRIVER_H_
#define ADPERCIVALDRIVER_H_

#include <epicsEvent.h>
#include <epicsThread.h>
#include <epicsMutex.h>

#include "ADDriver.h"
#include "IPercivalCallback.h"
#include "PercivalServer.h"
#include "Configurator.h"

//                                   String                            asyn interface  access  Description
#define PercFilePathString           "PERC_FILE_PATH"             //     (asynOctet,    r/w)   The file path
#define PercFilePathExistsString     "PERC_FILE_PATH_EXISTS"      //     (asynInt32,    r/w)   File path exists?
#define PercFileNameString           "PERC_FILE_NAME"             //     (asynOctet,    r/w)   The file name
#define PercFullFileNameString       "PERC_FULL_FILE_NAME"        //     (asynOctet,    r/o)   The actual complete file name for the last file saved
#define PercConfigFileReadString     "PERC_FILE_READ"             //     (asynInt32,    r/w)   Read in the config file
#define PercFileReadStatusString     "PERC_READ_STATUS"           //     (asynInt32,    r/w)   File read status
#define PercFileReadMessageString    "PERC_READ_MESSAGE"          //     (asynOctet,    r/w)   File read message

#define PercDebugLevelString         "PERC_DEBUG_LEVEL"           //     (asynInt32,    r/w)   Debug level for low level non-EPICS code

#define PercEnableChannel1String     "PERC_ENABLE_CHANNEL_1"      //     (asynInt32,    r/w)   Channel 1 enable/disable
#define PercEnableChannel2String     "PERC_ENABLE_CHANNEL_2"      //     (asynInt32,    r/w)   Channel 2 enable/disable
#define PercEnableChannel3String     "PERC_ENABLE_CHANNEL_3"      //     (asynInt32,    r/w)   Channel 3 enable/disable
#define PercEnableChannel4String     "PERC_ENABLE_CHANNEL_4"      //     (asynInt32,    r/w)   Channel 4 enable/disable

#define PercTopLeftXChannel1String   "PERC_TOP_LEFT_X_CHANNEL_1"  //     (asynInt32,    r/w)   Channel 1 top left X pixel
#define PercTopLeftXChannel2String   "PERC_TOP_LEFT_X_CHANNEL_2"  //     (asynInt32,    r/w)   Channel 2 top left X pixel
#define PercTopLeftXChannel3String   "PERC_TOP_LEFT_X_CHANNEL_3"  //     (asynInt32,    r/w)   Channel 3 top left X pixel
#define PercTopLeftXChannel4String   "PERC_TOP_LEFT_X_CHANNEL_4"  //     (asynInt32,    r/w)   Channel 4 top left X pixel

#define PercTopLeftYChannel1String   "PERC_TOP_LEFT_Y_CHANNEL_1"  //     (asynInt32,    r/w)   Channel 1 top left Y pixel
#define PercTopLeftYChannel2String   "PERC_TOP_LEFT_Y_CHANNEL_2"  //     (asynInt32,    r/w)   Channel 2 top left Y pixel
#define PercTopLeftYChannel3String   "PERC_TOP_LEFT_Y_CHANNEL_3"  //     (asynInt32,    r/w)   Channel 3 top left Y pixel
#define PercTopLeftYChannel4String   "PERC_TOP_LEFT_Y_CHANNEL_4"  //     (asynInt32,    r/w)   Channel 4 top left Y pixel

#define PercBotRightXChannel1String  "PERC_BOT_RIGHT_X_CHANNEL_1" //     (asynInt32,    r/w)   Channel 1 bottom right X pixel
#define PercBotRightXChannel2String  "PERC_BOT_RIGHT_X_CHANNEL_2" //     (asynInt32,    r/w)   Channel 2 bottom right X pixel
#define PercBotRightXChannel3String  "PERC_BOT_RIGHT_X_CHANNEL_3" //     (asynInt32,    r/w)   Channel 3 bottom right X pixel
#define PercBotRightXChannel4String  "PERC_BOT_RIGHT_X_CHANNEL_4" //     (asynInt32,    r/w)   Channel 4 bottom right X pixel

#define PercBotRightYChannel1String  "PERC_BOT_RIGHT_Y_CHANNEL_1" //     (asynInt32,    r/w)   Channel 1 bottom right Y pixel
#define PercBotRightYChannel2String  "PERC_BOT_RIGHT_Y_CHANNEL_2" //     (asynInt32,    r/w)   Channel 2 bottom right Y pixel
#define PercBotRightYChannel3String  "PERC_BOT_RIGHT_Y_CHANNEL_3" //     (asynInt32,    r/w)   Channel 3 bottom right Y pixel
#define PercBotRightYChannel4String  "PERC_BOT_RIGHT_Y_CHANNEL_4" //     (asynInt32,    r/w)   Channel 4 bottom right Y pixel

#define PercWidthChannel1String      "PERC_WIDTH_CHANNEL_1"       //     (asynInt32,    r/o)   Channel 1 frame width (pixels)
#define PercWidthChannel2String      "PERC_WIDTH_CHANNEL_2"       //     (asynInt32,    r/o)   Channel 2 frame width (pixels)
#define PercWidthChannel3String      "PERC_WIDTH_CHANNEL_3"       //     (asynInt32,    r/o)   Channel 3 frame width (pixels)
#define PercWidthChannel4String      "PERC_WIDTH_CHANNEL_4"       //     (asynInt32,    r/o)   Channel 4 frame width (pixels)

#define PercHeightChannel1String     "PERC_HEIGHT_CHANNEL_1"      //     (asynInt32,    r/o)   Channel 1 frame height (pixels)
#define PercHeightChannel2String     "PERC_HEIGHT_CHANNEL_2"      //     (asynInt32,    r/o)   Channel 2 frame height (pixels)
#define PercHeightChannel3String     "PERC_HEIGHT_CHANNEL_3"      //     (asynInt32,    r/o)   Channel 3 frame height (pixels)
#define PercHeightChannel4String     "PERC_HEIGHT_CHANNEL_4"      //     (asynInt32,    r/o)   Channel 4 frame height (pixels)

#define PercSubFramesChannel1String  "PERC_SUB_FRAMES_CHANNEL_1"  //     (asynInt32,    r/w)   Channel 1 how many subframes to split into
#define PercSubFramesChannel2String  "PERC_SUB_FRAMES_CHANNEL_2"  //     (asynInt32,    r/w)   Channel 2 how many subframes to split into
#define PercSubFramesChannel3String  "PERC_SUB_FRAMES_CHANNEL_3"  //     (asynInt32,    r/w)   Channel 3 how many subframes to split into
#define PercSubFramesChannel4String  "PERC_SUB_FRAMES_CHANNEL_4"  //     (asynInt32,    r/w)   Channel 4 how many subframes to split into

#define PercAddrChannel1String       "PERC_ADDR_CHANNEL_1"        //     (asynOctet,    r/w)   Channel 1 NIC address string
#define PercAddrChannel2String       "PERC_ADDR_CHANNEL_2"        //     (asynOctet,    r/w)   Channel 2 NIC address string
#define PercAddrChannel3String       "PERC_ADDR_CHANNEL_3"        //     (asynOctet,    r/w)   Channel 3 NIC address string
#define PercAddrChannel4String       "PERC_ADDR_CHANNEL_4"        //     (asynOctet,    r/w)   Channel 4 NIC address string

#define PercPortChannel1String       "PERC_PORT_CHANNEL_1"        //     (asynInt32,    r/w)   Channel 1 NIC port string
#define PercPortChannel2String       "PERC_PORT_CHANNEL_2"        //     (asynInt32,    r/w)   Channel 2 NIC port string
#define PercPortChannel3String       "PERC_PORT_CHANNEL_3"        //     (asynInt32,    r/w)   Channel 3 NIC port string
#define PercPortChannel4String       "PERC_PORT_CHANNEL_4"        //     (asynInt32,    r/w)   Channel 4 NIC port string

class ADPercivalDriver: public ADDriver, public IPercivalCallback
{
  public:
    ADPercivalDriver(const char *portName,
                     int maxBuffers,
                     size_t maxMemory,
                     int priority,
                     int stackSize);

    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
//    virtual asynStatus readGenericPointer(asynUser *pasynUser, void *genericPointer);
//    virtual asynStatus writeGenericPointer(asynUser *pasynUser, void *genericPointer);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
//    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
//    virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
    virtual void report(FILE *fp, int details);

    // These are the methods that are new to this class
    virtual int checkPath();
    virtual int createFileName(int maxChars, char *fullFileName);
    virtual int createFileName(int maxChars, char *filePath, char *fileName);

    virtual void imageReceived(PercivalBuffer *buffer);
    virtual PercivalBuffer *allocateBuffer();

  protected:
    int PercFilePath;
    #define FIRST_PERCIVAL_PARAM PercFilePath
    int PercFilePathExists;
    int PercFileName;
    int PercFullFileName;
    int PercConfigFileRead;
    int PercFileReadStatus;
    int PercFileReadMessage;
    int PercDebugLevel;

    // Parameters for each of the subframes
    int PercEnableChannel1;
    int PercEnableChannel2;
    int PercEnableChannel3;
    int PercEnableChannel4;

    int PercTopLeftXChannel1;
    int PercTopLeftXChannel2;
    int PercTopLeftXChannel3;
    int PercTopLeftXChannel4;
    int PercTopLeftYChannel1;
    int PercTopLeftYChannel2;
    int PercTopLeftYChannel3;
    int PercTopLeftYChannel4;
    int PercBotRightXChannel1;
    int PercBotRightXChannel2;
    int PercBotRightXChannel3;
    int PercBotRightXChannel4;
    int PercBotRightYChannel1;
    int PercBotRightYChannel2;
    int PercBotRightYChannel3;
    int PercBotRightYChannel4;

    int PercWidthChannel1;
    int PercWidthChannel2;
    int PercWidthChannel3;
    int PercWidthChannel4;
    int PercHeightChannel1;
    int PercHeightChannel2;
    int PercHeightChannel3;
    int PercHeightChannel4;

    int PercSubFramesChannel1;
    int PercSubFramesChannel2;
    int PercSubFramesChannel3;
    int PercSubFramesChannel4;

    int PercAddrChannel1;
    int PercAddrChannel2;
    int PercAddrChannel3;
    int PercAddrChannel4;
    int PercPortChannel1;
    int PercPortChannel2;
    int PercPortChannel3;
    int PercPortChannel4;
    #define LAST_PERCIVAL_PARAM PercPortChannel4

  private:
  
    epicsEventId startEventId_;
    epicsEventId stopEventId_;

    PercivalServer     *sPtr_;      // Percival UDP server
    PercivalBufferPool *buffers_;   // Pool of percival buffers
    Configurator       *configPtr_; // Configurator ptr for reading configuration file
    NDArray *pImage_;

    // Image declerations
    size_t        dims_[2];
    int           ndims_;
    NDArrayInfo_t arrayInfo_;

};



#define NUM_PERCIVAL_PARAMS (&LAST_PERCIVAL_PARAM - &FIRST_PERCIVAL_PARAM + 1)

#define LEN_ERR_MSG     512
#define LEN_SERIAL_MSG  128
#define DRIVER_TIMEOUT 1000


// A bit of C glue to make the config function available in the startup script (ioc shell)
extern "C" int Percival_config(const char *portName, int maxBuffers, size_t maxMemory);


#endif // ADPERCIVAL_H_ 

