/*
 * asynConfiguratorDriver.h
 *
 *  Created on: 6th Aug 2013
 *      Author: gnx91527
 */

#ifndef ASYNCONFIGURATORDRIVER_H_
#define ASYNCONFIGURATORDRIVER_H_

#include "asynPortDriver.h"
#include "Configurator.h"

/** Maximum length of a filename or any of its components */
#define MAX_FILENAME_LEN 256

#define UInt8  0
#define UInt16 1
#define UInt32 2

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
#define ImageCountString       "IMAGE_COUNT"       /**< (asynInt32,    r/w) Number of images to generate in the sequence */
#define ImageMinValueString    "IMAGE_MIN_VALUE"   /**< (asynInt32,    r/w) Minimum pixel value */
#define ImageMaxValueString    "IMAGE_MAX_VALUE"   /**< (asynInt32,    r/w) Maximum pixel value */

    /* File name related parameters for saving data.
     * The driver will normally combine NDFilePath, NDFileName, and NDFileNumber into
     * a file name that order using the format specification in FileTemplate.
     * For example NDFileTemplate might be "%s%s_%d.tif" */
#define FilePathString         "FILE_PATH"         /**< (asynOctet,    r/w) The file path */
#define FilePathExistsString   "FILE_PATH_EXISTS"  /**< (asynInt32,    r/w) File path exists? */
#define FileNameString         "FILE_NAME"         /**< (asynOctet,    r/w) The file name */
#define FullFileNameString     "FULL_FILE_NAME"    /**< (asynOctet,    r/o) The actual complete file name for the last file saved */
#define RawFileWriteString     "RAW_FILE_WRITE"      /**< (asynInt32,    r/w) Write a raw file out */
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

class epicsShareFunc asynConfiguratorDriver : public asynPortDriver
{
  public:
    asynConfiguratorDriver(const char *portName,
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

  protected:
    int PortNameSelf;
    #define FIRST_CONFIGURATOR_PARAM PortNameSelf
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
    int ImageCount;
    int ImageMinValue;
    int ImageMaxValue;
    int FilePath;
    int FilePathExists;
    int FileName;
    int FullFileName;
    int RawFileWrite;
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
    #define LAST_CONFIGURATOR_PARAM ImageScrambleType

  private:
    Configurator *configPtr;
};

#define NUM_CONFIGURATOR_PARAMS (&LAST_CONFIGURATOR_PARAM - &FIRST_CONFIGURATOR_PARAM + 1)
#endif // ASYNCONFIGURATORDRIVER_H_

