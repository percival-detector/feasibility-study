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
#include "CPUstats.h"

//                                   String                            asyn interface  access  Description
#define PercFilePathString           "PERC_FILE_PATH"             //     (asynOctet,    r/w)   The file path
#define PercFilePathExistsString     "PERC_FILE_PATH_EXISTS"      //     (asynInt32,    r/w)   File path exists?
#define PercFileNameString           "PERC_FILE_NAME"             //     (asynOctet,    r/w)   The file name
#define PercFullFileNameString       "PERC_FULL_FILE_NAME"        //     (asynOctet,    r/o)   The actual complete file name for the last file saved
#define PercConfigFileReadString     "PERC_FILE_READ"             //     (asynInt32,    r/w)   Read in the config file
#define PercFileReadStatusString     "PERC_READ_STATUS"           //     (asynInt32,    r/w)   File read status
#define PercFileReadMessageString    "PERC_READ_MESSAGE"          //     (asynOctet,    r/w)   File read message
#define PercFileErrorStatusString    "PERC_FILE_ERROR_STATUS"     //     (asynOctet,    r/w)   File read error status

#define PercDebugLevelString         "PERC_DEBUG_LEVEL"           //     (asynInt32,    r/w)   Debug level for low level non-EPICS code
#define PercWatchdogTimeoutString    "PERC_WATCHDOG_TIMEOUT"      //     (asynInt32,    r/w)   Watchdog timeout in ms

#define PercChannelModeString        "PERC_CHANNEL_MODE"          //     (asynInt32,    r/w)   Are we running multi-channel or single channel mode
#define PercDescrambleString         "PERC_DESCRAMBLE"            //     (asynInt32,    r/w)   Are we going to descramble or keep raw frames

#define PercTopLeftXSF1String   "PERC_TOP_LEFT_X_SF_1"  //     (asynInt32,    r/w)   Sub-Frame 1 top left X pixel
#define PercTopLeftXSF2String   "PERC_TOP_LEFT_X_SF_2"  //     (asynInt32,    r/w)   Sub-Frame 2 top left X pixel
#define PercTopLeftXSF3String   "PERC_TOP_LEFT_X_SF_3"  //     (asynInt32,    r/w)   Sub-Frame 3 top left X pixel
#define PercTopLeftXSF4String   "PERC_TOP_LEFT_X_SF_4"  //     (asynInt32,    r/w)   Sub-Frame 4 top left X pixel
#define PercTopLeftXSF5String   "PERC_TOP_LEFT_X_SF_5"  //     (asynInt32,    r/w)   Sub-Frame 5 top left X pixel
#define PercTopLeftXSF6String   "PERC_TOP_LEFT_X_SF_6"  //     (asynInt32,    r/w)   Sub-Frame 6 top left X pixel
#define PercTopLeftXSF7String   "PERC_TOP_LEFT_X_SF_7"  //     (asynInt32,    r/w)   Sub-Frame 7 top left X pixel
#define PercTopLeftXSF8String   "PERC_TOP_LEFT_X_SF_8"  //     (asynInt32,    r/w)   Sub-Frame 8 top left X pixel

#define PercTopLeftYSF1String   "PERC_TOP_LEFT_Y_SF_1"  //     (asynInt32,    r/w)   Sub-Frame 1 top left Y pixel
#define PercTopLeftYSF2String   "PERC_TOP_LEFT_Y_SF_2"  //     (asynInt32,    r/w)   Sub-Frame 2 top left Y pixel
#define PercTopLeftYSF3String   "PERC_TOP_LEFT_Y_SF_3"  //     (asynInt32,    r/w)   Sub-Frame 3 top left Y pixel
#define PercTopLeftYSF4String   "PERC_TOP_LEFT_Y_SF_4"  //     (asynInt32,    r/w)   Sub-Frame 4 top left Y pixel
#define PercTopLeftYSF5String   "PERC_TOP_LEFT_Y_SF_5"  //     (asynInt32,    r/w)   Sub-Frame 5 top left Y pixel
#define PercTopLeftYSF6String   "PERC_TOP_LEFT_Y_SF_6"  //     (asynInt32,    r/w)   Sub-Frame 6 top left Y pixel
#define PercTopLeftYSF7String   "PERC_TOP_LEFT_Y_SF_7"  //     (asynInt32,    r/w)   Sub-Frame 7 top left Y pixel
#define PercTopLeftYSF8String   "PERC_TOP_LEFT_Y_SF_8"  //     (asynInt32,    r/w)   Sub-Frame 8 top left Y pixel

#define PercBotRightXSF1String  "PERC_BOT_RIGHT_X_SF_1" //     (asynInt32,    r/w)   Sub-Frame 1 bottom right X pixel
#define PercBotRightXSF2String  "PERC_BOT_RIGHT_X_SF_2" //     (asynInt32,    r/w)   Sub-Frame 2 bottom right X pixel
#define PercBotRightXSF3String  "PERC_BOT_RIGHT_X_SF_3" //     (asynInt32,    r/w)   Sub-Frame 3 bottom right X pixel
#define PercBotRightXSF4String  "PERC_BOT_RIGHT_X_SF_4" //     (asynInt32,    r/w)   Sub-Frame 4 bottom right X pixel
#define PercBotRightXSF5String  "PERC_BOT_RIGHT_X_SF_5" //     (asynInt32,    r/w)   Sub-Frame 5 bottom right X pixel
#define PercBotRightXSF6String  "PERC_BOT_RIGHT_X_SF_6" //     (asynInt32,    r/w)   Sub-Frame 6 bottom right X pixel
#define PercBotRightXSF7String  "PERC_BOT_RIGHT_X_SF_7" //     (asynInt32,    r/w)   Sub-Frame 7 bottom right X pixel
#define PercBotRightXSF8String  "PERC_BOT_RIGHT_X_SF_8" //     (asynInt32,    r/w)   Sub-Frame 8 bottom right X pixel

#define PercBotRightYSF1String  "PERC_BOT_RIGHT_Y_SF_1" //     (asynInt32,    r/w)   Sub-Frame 1 bottom right Y pixel
#define PercBotRightYSF2String  "PERC_BOT_RIGHT_Y_SF_2" //     (asynInt32,    r/w)   Sub-Frame 2 bottom right Y pixel
#define PercBotRightYSF3String  "PERC_BOT_RIGHT_Y_SF_3" //     (asynInt32,    r/w)   Sub-Frame 3 bottom right Y pixel
#define PercBotRightYSF4String  "PERC_BOT_RIGHT_Y_SF_4" //     (asynInt32,    r/w)   Sub-Frame 4 bottom right Y pixel
#define PercBotRightYSF5String  "PERC_BOT_RIGHT_Y_SF_5" //     (asynInt32,    r/w)   Sub-Frame 5 bottom right Y pixel
#define PercBotRightYSF6String  "PERC_BOT_RIGHT_Y_SF_6" //     (asynInt32,    r/w)   Sub-Frame 6 bottom right Y pixel
#define PercBotRightYSF7String  "PERC_BOT_RIGHT_Y_SF_7" //     (asynInt32,    r/w)   Sub-Frame 7 bottom right Y pixel
#define PercBotRightYSF8String  "PERC_BOT_RIGHT_Y_SF_8" //     (asynInt32,    r/w)   Sub-Frame 8 bottom right Y pixel

#define PercWidthSF1String      "PERC_WIDTH_SF_1"       //     (asynInt32,    r/o)   Sub-Frame 1 frame width (pixels)
#define PercWidthSF2String      "PERC_WIDTH_SF_2"       //     (asynInt32,    r/o)   Sub-Frame 2 frame width (pixels)
#define PercWidthSF3String      "PERC_WIDTH_SF_3"       //     (asynInt32,    r/o)   Sub-Frame 3 frame width (pixels)
#define PercWidthSF4String      "PERC_WIDTH_SF_4"       //     (asynInt32,    r/o)   Sub-Frame 4 frame width (pixels)
#define PercWidthSF5String      "PERC_WIDTH_SF_5"       //     (asynInt32,    r/o)   Sub-Frame 5 frame width (pixels)
#define PercWidthSF6String      "PERC_WIDTH_SF_6"       //     (asynInt32,    r/o)   Sub-Frame 6 frame width (pixels)
#define PercWidthSF7String      "PERC_WIDTH_SF_7"       //     (asynInt32,    r/o)   Sub-Frame 7 frame width (pixels)
#define PercWidthSF8String      "PERC_WIDTH_SF_8"       //     (asynInt32,    r/o)   Sub-Frame 8 frame width (pixels)

#define PercHeightSF1String     "PERC_HEIGHT_SF_1"      //     (asynInt32,    r/o)   Sub-Frame 1 frame height (pixels)
#define PercHeightSF2String     "PERC_HEIGHT_SF_2"      //     (asynInt32,    r/o)   Sub-Frame 2 frame height (pixels)
#define PercHeightSF3String     "PERC_HEIGHT_SF_3"      //     (asynInt32,    r/o)   Sub-Frame 3 frame height (pixels)
#define PercHeightSF4String     "PERC_HEIGHT_SF_4"      //     (asynInt32,    r/o)   Sub-Frame 4 frame height (pixels)
#define PercHeightSF5String     "PERC_HEIGHT_SF_5"      //     (asynInt32,    r/o)   Sub-Frame 5 frame height (pixels)
#define PercHeightSF6String     "PERC_HEIGHT_SF_6"      //     (asynInt32,    r/o)   Sub-Frame 6 frame height (pixels)
#define PercHeightSF7String     "PERC_HEIGHT_SF_7"      //     (asynInt32,    r/o)   Sub-Frame 7 frame height (pixels)
#define PercHeightSF8String     "PERC_HEIGHT_SF_8"      //     (asynInt32,    r/o)   Sub-Frame 8 frame height (pixels)

#define PercSubFrame1IDString   "PERC_SUB_FRAME_ID_1"   //     (asynInt32,    r/w)   Sub-Frame 1 ID (1 - 8)
#define PercSubFrame2IDString   "PERC_SUB_FRAME_ID_2"   //     (asynInt32,    r/w)   Sub-Frame 2 ID (1 - 8)
#define PercSubFrame3IDString   "PERC_SUB_FRAME_ID_3"   //     (asynInt32,    r/w)   Sub-Frame 3 ID (1 - 8)
#define PercSubFrame4IDString   "PERC_SUB_FRAME_ID_4"   //     (asynInt32,    r/w)   Sub-Frame 4 ID (1 - 8)
#define PercSubFrame5IDString   "PERC_SUB_FRAME_ID_5"   //     (asynInt32,    r/w)   Sub-Frame 5 ID (1 - 8)
#define PercSubFrame6IDString   "PERC_SUB_FRAME_ID_6"   //     (asynInt32,    r/w)   Sub-Frame 6 ID (1 - 8)
#define PercSubFrame7IDString   "PERC_SUB_FRAME_ID_7"   //     (asynInt32,    r/w)   Sub-Frame 7 ID (1 - 8)
#define PercSubFrame8IDString   "PERC_SUB_FRAME_ID_8"   //     (asynInt32,    r/w)   Sub-Frame 8 ID (1 - 8)

#define PercReceiveString       "PERC_RECEIVE"          //     (asynInt32,    r/o)   Are we receiving
#define PercErrorString         "PERC_ERROR"            //     (asynInt32,    r/o)   Is there an error
#define PercStatusString        "PERC_STATUS"           //     (asynOctet,    r/o)   Current status

#define PercAddrString          "PERC_ADDR"             //     (asynOctet,    r/w)   NIC address string
#define PercPortString          "PERC_PORT"             //     (asynInt32,    r/w)   NIC port string
#define PercPacketBytesString   "PERC_PACKET_BYTES"     //     (asynInt32,    r/w)   Number of payload bytes per UDP packet
#define PercSubFrameString      "PERC_SUB_FRAME"        //     (asynInt32,    r/w)   Spatial mode, sub-frame ID for this detector (0-7)
#define PercCpuGroupString      "PERC_CPU_GROUP"        //     (asynInt32,    r/w)   Which CPU group to pin threads to? (0-9)

#define PercProcessTimeString   "PERC_PROCESS_TIME"     //     (asynInt32,    r/o)   Process time in microseconds
#define PercResetProcTimeString "PERC_RESET_PROC_TIME"  //     (asynInt32,    r/o)   Process time for reset frame in microseconds
#define PercServiceTimeString   "PERC_SERVICE_TIME"     //     (asynInt32,    r/o)   Service time in microseconds

#define PercFrameProcCpuString  "PERC_FRAME_PROC_CPU"   // (asynInt32, r/o) CPU usage of frame processing thread (percent)
#define PercResetProcCpuString  "PERC_RESET_PROC_CPU"   // (asynInt32, r/o) CPU usage of reset frame thread (percent)
#define PercPktServCpuString    "PERC_PKT_SERV_CPU"     // (asynInt32, r/o) CPU usage of packet service thread (percent)

#define PercErrorDupPktString   "PERC_ERR_DUP_PKT"      //     (asynInt32,    r/o)   Duplicate packet error
#define PercErrorMisPktString   "PERC_ERR_MIS_PKT"      //     (asynInt32,    r/o)   Missing packet error
#define PercErrorLtePktString   "PERC_ERR_LTE_PKT"      //     (asynInt32,    r/o)   Late packet error
#define PercErrorIncPktString   "PERC_ERR_INC_PKT"      //     (asynInt32,    r/o)   Unexpected data error
#define PercErrorDupRPktString  "PERC_ERR_DUP_R_PKT"    //     (asynInt32,    r/o)   Duplicate reset packet error
#define PercErrorMisRPktString  "PERC_ERR_MIS_R_PKT"    //     (asynInt32,    r/o)   Missing reset packet error
#define PercErrorLteRPktString  "PERC_ERR_LTE_R_PKT"    //     (asynInt32,    r/o)   Late reset packet error
#define PercErrorIncRPktString  "PERC_ERR_INC_R_PKT"    //     (asynInt32,    r/o)   Unexpected reset data error
#define PercErrorResetString    "PERC_ERR_RESET"        //     (asynInt32,    r/o)   Missing reset frame

class ADPercivalDriver: public ADDriver, public IPercivalCallback
{
  public:
    ADPercivalDriver(const char *portName,
                     int maxBuffers,
                     size_t maxMemory,
                     int priority,
                     int stackSize);

    virtual ~ADPercivalDriver();

    virtual void stats_task();

    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual void setupSpatialImage();
    virtual void setupTemporalImage();
    virtual void report(FILE *fp, int details);

    // These are the methods that are new to this class
    virtual int checkPath();
    virtual int createFileName(int maxChars, char *fullFileName);
    virtual int createFileName(int maxChars, char *filePath, char *fileName);

    virtual void imageReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType);
    virtual void timeout();
    virtual PercivalBuffer *allocateBuffer();
    virtual void releaseBuffer(PercivalBuffer *buffer);

  protected:
    int PercFilePath;
    #define FIRST_PERCIVAL_PARAM PercFilePath
    int PercFilePathExists;
    int PercFileName;
    int PercFullFileName;
    int PercConfigFileRead;
    int PercFileReadStatus;
    int PercFileReadMessage;
    int PercFileErrorStatus;
    int PercDebugLevel;
    int PercWatchdogTimeout;
    int PercChannelMode;
    int PercDescramble;

    // Parameters for each of the subframes
    //int PercEnableChannel1;
    //int PercEnableChannel2;
    //int PercEnableChannel3;
    //int PercEnableChannel4;

    int PercTopLeftXSF1;
    int PercTopLeftXSF2;
    int PercTopLeftXSF3;
    int PercTopLeftXSF4;
    int PercTopLeftXSF5;
    int PercTopLeftXSF6;
    int PercTopLeftXSF7;
    int PercTopLeftXSF8;
    int PercTopLeftYSF1;
    int PercTopLeftYSF2;
    int PercTopLeftYSF3;
    int PercTopLeftYSF4;
    int PercTopLeftYSF5;
    int PercTopLeftYSF6;
    int PercTopLeftYSF7;
    int PercTopLeftYSF8;
    int PercBotRightXSF1;
    int PercBotRightXSF2;
    int PercBotRightXSF3;
    int PercBotRightXSF4;
    int PercBotRightXSF5;
    int PercBotRightXSF6;
    int PercBotRightXSF7;
    int PercBotRightXSF8;
    int PercBotRightYSF1;
    int PercBotRightYSF2;
    int PercBotRightYSF3;
    int PercBotRightYSF4;
    int PercBotRightYSF5;
    int PercBotRightYSF6;
    int PercBotRightYSF7;
    int PercBotRightYSF8;

    int PercWidthSF1;
    int PercWidthSF2;
    int PercWidthSF3;
    int PercWidthSF4;
    int PercWidthSF5;
    int PercWidthSF6;
    int PercWidthSF7;
    int PercWidthSF8;
    int PercHeightSF1;
    int PercHeightSF2;
    int PercHeightSF3;
    int PercHeightSF4;
    int PercHeightSF5;
    int PercHeightSF6;
    int PercHeightSF7;
    int PercHeightSF8;

    int PercSubFrame1ID;
    int PercSubFrame2ID;
    int PercSubFrame3ID;
    int PercSubFrame4ID;
    int PercSubFrame5ID;
    int PercSubFrame6ID;
    int PercSubFrame7ID;
    int PercSubFrame8ID;

    int PercReceive;
    int PercError;
    int PercStatus;

    int PercAddr;
    int PercPort;
    int PercPacketBytes;
    int PercSubFrame;
    int PercCpuGroup;

    int PercProcessTime;
    int PercResetProcTime;
    int PercServiceTime;

    int PercFrameProcCpu;
    int PercResetProcCpu;
    int PercPktServCpu;

    int PercErrorDupPkt;
    int PercErrorMisPkt;
    int PercErrorLtePkt;
    int PercErrorIncPkt;
    int PercErrorDupRPkt;
    int PercErrorMisRPkt;
    int PercErrorLteRPkt;
    int PercErrorIncRPkt;
    int PercErrorReset;

    #define LAST_PERCIVAL_PARAM PercErrorReset

  private:

    epicsEventId startEventId_;
    epicsEventId stopEventId_;

    PercivalServer     *sPtr_;      // Percival UDP server
    PercivalBufferPool *buffers_;   // Pool of percival buffers
    Configurator       *configPtr_; // Configurator ptr for reading configuration file
    NDArray *pImage_;

    // Descramble Array
    uint32_t *descrambleArray_;
    // Sub-Frame Descramble Array
    uint32_t *sfDescrambleArray_;
    // Data structures for gain information
    uint32_t noOfADCs_;
    uint32_t *ADC_index_;         // Index of ADC to use for each input data point
    float    *ADC_low_gain_;      // Array of low gain ADC gains, one per ADC
    float    *ADC_high_gain_;     // Array of high gain ADC gains, one per ADC
    float    *ADC_offset_;        // Combined offset for both ADC's
    float    *stage_gains_;       // Gain to apply for each of the output stages
    float    *stage_offsets_;     // Offsets to apply for each of the output stages (in scrambled order)

    uint32_t *ADC_low_gain_uint32_;      // Array of low gain ADC gains, one per ADC
    uint32_t *ADC_high_gain_uint32_;     // Array of high gain ADC gains, one per ADC
    uint32_t *ADC_offset_uint32_;        // Combined offset for both ADC's
    uint32_t *stage_gains_uint32_;       // Gain to apply for each of the output stages
    uint32_t *stage_offsets_uint32_;     // Offsets to apply for each of the output stages (in scrambled order)

    // Variables to store the image width when placed into single channel mode
    uint32_t      realWidth_;
    uint32_t      realHeight_;

    // Image declarations
    size_t        dims_[2];
    int           ndims_;
    NDArrayInfo_t arrayInfo_;
    PercivalBuffer *pBuffer_;

    // Bookkeeping
    CPUstats *cpustats_;
};



#define NUM_PERCIVAL_PARAMS (&LAST_PERCIVAL_PARAM - &FIRST_PERCIVAL_PARAM + 1)

#define LEN_ERR_MSG     512
#define LEN_SERIAL_MSG  128
#define DRIVER_TIMEOUT 1000


// A bit of C glue to make the config function available in the startup script (ioc shell)
extern "C" int Percival_config(const char *portName, int maxBuffers, size_t maxMemory);


#endif // ADPERCIVAL_H_

