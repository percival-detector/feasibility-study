/*
 * asynGeneratorDriver.h
 *
 *  Created on: 6th Aug 2013
 *      Author: gnx91527
 */

#ifndef ASYNCONFIGURATORDRIVER_H_
#define ASYNCONFIGURATORDRIVER_H_

#include <epicsEvent.h>
#include <epicsThread.h>
#include <epicsMutex.h>

#include "asynPortDriver.h"
#include "Configurator.h"

/** Maximum length of a filename or any of its components */
#define MAX_FILENAME_LEN 256

typedef enum {
    FileWriteOK,
    FileWriteError
} FileWriteStatus_t;

 /*                              String                 asyn interface  access   Description  */
#define PortNameSelfString     "PORT_NAME_SELF"     /**< (asynOctet,    r/o) Asyn port name of this driver instance */

#define ImageSizeXString       "IMAGE_SIZE_X"       /**< (asynInt32,    r/o) Size of the array data in the X direction */
#define ImageSizeYString       "IMAGE_SIZE_Y"       /**< (asynInt32,    r/o) Size of the array data in the Y direction */
#define ImagePatternXString    "IMAGE_PATTERN_X"    /**< (asynInt32,    r/o) Pattern repeat size in the X direction */
#define ImagePatternYString    "IMAGE_PATTERN_Y"    /**< (asynInt32,    r/o) Pattern repeat size in the Y direction */
#define ImagePatternTypeString "IMAGE_PATTERN_TYPE" /**< (asynInt32,    r/o) Type of pattern: rectangle, triangle, ellipse */

#define ImageSizeZString       "IMAGE_SIZE_Z"      /**< (asynInt32,    r/o) Size of the array data in the Z direction */
#define ImageSizeString        "IMAGE_SIZE"        /**< (asynInt32,    r/o) Total size of array data in bytes */
#define NDimensionsString      "IMAGE_NDIMENSIONS" /**< (asynInt32,    r/o) Number of dimensions in array */
#define DimensionsString       "IMAGE_DIMENSIONS"  /**< (asynInt32Array, r/o) Array dimensions */
#define DataTypeString         "DATA_TYPE"         /**< (asynInt32,    r/w) Data type (NDDataType_t) */

    /* File name related parameters for saving data.
     * The driver will normally combine NDFilePath, NDFileName, and NDFileNumber into
     * a file name that order using the format specification in FileTemplate.
     * For example NDFileTemplate might be "%s%s_%d.tif" */
#define FilePathString         "FILE_PATH"         /**< (asynOctet,    r/w) The file path */
#define FilePathExistsString   "FILE_PATH_EXISTS"  /**< (asynInt32,    r/w) File path exists? */
#define FileNameString         "FILE_NAME"         /**< (asynOctet,    r/w) The file name */
#define FullFileNameString     "FULL_FILE_NAME"    /**< (asynOctet,    r/o) The actual complete file name for the last file saved */
#define RawFileReadString      "RAW_FILE_WRITE"      /**< (asynInt32,    r/w) Write a raw file out */
#define FileWriteStatusString  "WRITE_STATUS"      /**< (asynInt32,    r/w) File write status */
#define FileWriteMessageString "WRITE_MESSAGE"     /**< (asynOctet,    r/w) File write message */

// The following parameters setup the physical layout of the detector
#define DPixelsPerChipXString    "D_PIXELS_PER_CHIP_X"         /**< (asynInt32,    r/w) Number of pixels per chip in the x direction */
#define DPixelsPerChipYString    "D_PIXELS_PER_CHIP_Y"         /**< (asynInt32,    r/w) Number of pixels per chip in the y direction */
#define DChipsPerBlockXString    "D_CHIPS_PER_BLOCK_X"         /**< (asynInt32,    r/w) Number of chips in a block in the x direction */
#define DBlocksPerStripeXString  "D_BLOCKS_PER_STRIPE_X"       /**< (asynInt32,    r/w) Number of blocks in a stripe in the x direction */
#define DChipsPerStripeXString   "D_CHIPS_PER_STRIPE_X"        /**< (asynInt32,    r/w) Number of chips in a stripe in the x direction */
#define DChipsPerStripeYString   "D_CHIPS_PER_STRIPE_Y"        /**< (asynInt32,    r/w) Number of chips in a stripe in the y direction */
#define DStripesPerModuleString  "D_STRIPES_PER_MODULE"        /**< (asynInt32,    r/w) Number of stripes in a module */
#define DStripesPerImageString   "D_STRIPES_PER_IMAGE"         /**< (asynInt32,    r/w) Number of stripes in an image */

#define ImageScrambleTypeString  "IMAGE_SCRAMBLE_TYPE"         /**< (asynInt32,    r/w) Type of image scramble to execute (excalibur, percival) */


#define GDPostCommandString      "GD_POST_COMMAND"             /**< (asynInt32,    r/w) Start/stop posting */

#define GDPostChannel1String     "GD_POST_CHANNEL_1"           /**< (asynInt32,    r/w) Are we posting on channel 1? */
#define GDPostChannel2String     "GD_POST_CHANNEL_2"           /**< (asynInt32,    r/w) Are we posting on channel 2? */
#define GDPostChannel3String     "GD_POST_CHANNEL_3"           /**< (asynInt32,    r/w) Are we posting on channel 3? */
#define GDPostChannel4String     "GD_POST_CHANNEL_4"           /**< (asynInt32,    r/w) Are we posting on channel 4? */

#define GDCounterChannel1String  "GD_COUNTER_CHANNEL_1"        /**< (asynInt32,    r/w) Current counter on channel 1? */
#define GDCounterChannel2String  "GD_COUNTER_CHANNEL_2"        /**< (asynInt32,    r/w) Current counter on channel 2? */
#define GDCounterChannel3String  "GD_COUNTER_CHANNEL_3"        /**< (asynInt32,    r/w) Current counter on channel 3? */
#define GDCounterChannel4String  "GD_COUNTER_CHANNEL_4"        /**< (asynInt32,    r/w) Current counter on channel 4? */

#define GDEnableChannel1String   "GD_ENABLE_CHANNEL_1"        /**< (asynInt32,    r/w) Channel 1 enable/disable */
#define GDEnableChannel2String   "GD_ENABLE_CHANNEL_2"        /**< (asynInt32,    r/w) Channel 2 enable/disable */
#define GDEnableChannel3String   "GD_ENABLE_CHANNEL_3"        /**< (asynInt32,    r/w) Channel 3 enable/disable */
#define GDEnableChannel4String   "GD_ENABLE_CHANNEL_4"        /**< (asynInt32,    r/w) Channel 4 enable/disable */

class epicsShareFunc asynGeneratorDriver : public asynPortDriver
{
  public:
    asynGeneratorDriver(const char *portName,
                        int maxAddr,
                        int interfaceMask,
                        int interruptMask,
                        int asynFlags,
                        int autoConnect,
                        int priority,
                        int stackSize);

    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
//    virtual asynStatus readGenericPointer(asynUser *pasynUser, void *genericPointer);
//    virtual asynStatus writeGenericPointer(asynUser *pasynUser, void *genericPointer);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
    virtual void report(FILE *fp, int details);

    // These are the methods that are new to this class
    virtual int checkPath();
    virtual int createFileName(int maxChars, char *fullFileName);
    virtual int createFileName(int maxChars, char *filePath, char *fileName);

    virtual void posting_task(int taskNumber);

  protected:
    int PortNameSelf;
    #define FIRST_GENERATOR_PARAM PortNameSelf
    int ImageSizeX;
    int ImageSizeY;
    int ImagePatternX;
    int ImagePatternY;
    int ImagePatternType;
    int ImageSizeZ;
    int ImageSize;
    int NDimensions;
    int Dimensions;
    int DataType;
    int FilePath;
    int FilePathExists;
    int FileName;
    int FullFileName;
    int RawFileRead;
    int FileWriteStatus;
    int FileWriteMessage;
    int DPixelsPerChipX;
    int DPixelsPerChipY;
    int DChipsPerBlockX;
    int DBlocksPerStripeX;
    int DChipsPerStripeX;
    int DChipsPerStripeY;
    int DStripesPerModule;
    int DStripesPerImage;
    int ImageScrambleType;

    // The following attributes are required for the four posting threads
    int GDPostCommand;
    int GDPostChannel1;
    int GDPostChannel2;
    int GDPostChannel3;
    int GDPostChannel4;
    
    int GDCounterChannel1;
    int GDCounterChannel2;
    int GDCounterChannel3;
    int GDCounterChannel4;

    int GDEnableChannel1;
    int GDEnableChannel2;
    int GDEnableChannel3;
    int GDEnableChannel4;
    #define LAST_GENERATOR_PARAM GDEnableChannel4

    int GDPostBase;
    int GDCounterBase;

  private:
    Configurator *configPtr;

    epicsEventId startEventId_[4];
    epicsEventId stopEventId_[4];

};

#define NUM_GENERATOR_PARAMS (&LAST_GENERATOR_PARAM - &FIRST_GENERATOR_PARAM + 1)
#endif // ASYNGENERATORDRIVER_H_

