/*
 * asynGeneratorDriver.h
 *
 *  Created on: 6th Aug 2013
 *      Author: gnx91527
 */

#ifndef ASYNGENERATORDRIVER_H_
#define ASYNGENERATORDRIVER_H_

#include <epicsEvent.h>
#include <epicsThread.h>
#include <epicsMutex.h>

#include "asynPortDriver.h"
#include "Configurator.h"
#include "DataSender.h"

/** Maximum length of a filename or any of its components */
#define MAX_FILENAME_LEN 256

#define UInt8  0
#define UInt16 1
#define UInt32 2

typedef enum {
    FileWriteOK,
    FileWriteError
} FileWriteStatus_t;

 /*                              String                             asyn interface  access   Description  */
#define PortNameSelfString       "PORT_NAME_SELF"                /**< (asynOctet,    r/o) Asyn port name of this driver instance */

#define ImageSizeXString         "IMAGE_SIZE_X"                  /**< (asynInt32,    r/o) Size of the array data in the X direction */
#define ImageSizeYString         "IMAGE_SIZE_Y"                  /**< (asynInt32,    r/o) Size of the array data in the Y direction */
#define ImagePatternXString      "IMAGE_PATTERN_X"               /**< (asynInt32,    r/o) Pattern repeat size in the X direction */
#define ImagePatternYString      "IMAGE_PATTERN_Y"               /**< (asynInt32,    r/o) Pattern repeat size in the Y direction */
#define ImagePatternTypeString   "IMAGE_PATTERN_TYPE"            /**< (asynInt32,    r/o) Type of pattern: rectangle, triangle, ellipse */

#define ImageSizeZString         "IMAGE_SIZE_Z"                  /**< (asynInt32,    r/o) Size of the array data in the Z direction */
#define ImageSizeString          "IMAGE_SIZE"                    /**< (asynInt32,    r/o) Total size of array data in bytes */
#define NDimensionsString        "IMAGE_NDIMENSIONS"             /**< (asynInt32,    r/o) Number of dimensions in array */
#define DimensionsString         "IMAGE_DIMENSIONS"              /**< (asynInt32Array, r/o) Array dimensions */
#define DataTypeString           "DATA_TYPE"                     /**< (asynInt32,    r/w) Data type (NDDataType_t) */

    /* File name related parameters for saving data.
     * The driver will normally combine NDFilePath, NDFileName, and NDFileNumber into
     * a file name that order using the format specification in FileTemplate.
     * For example NDFileTemplate might be "%s%s_%d.tif" */
#define FilePathString           "FILE_PATH"                     /**< (asynOctet,    r/w) The file path */
#define FilePathExistsString     "FILE_PATH_EXISTS"              /**< (asynInt32,    r/w) File path exists? */
#define FileNameString           "FILE_NAME"                     /**< (asynOctet,    r/w) The file name */
#define FullFileNameString       "FULL_FILE_NAME"                /**< (asynOctet,    r/o) The actual complete file name for the last file saved */
#define RawFileReadString        "RAW_FILE_READ"                 /**< (asynInt32,    r/w) Write a raw file out */
#define FileReadStatusString     "GD_READ_STATUS"                /**< (asynInt32,    r/w) File write status */
#define FileReadMessageString    "READ_MESSAGE"                  /**< (asynOctet,    r/w) File write message */
#define FileErrorStatusString    "FILE_ERROR_STATUS"             /**< (asynInt32,    r/o) File read error status */

// The following parameters setup the physical layout of the detector
#define DPixelsPerChipXString    "D_PIXELS_PER_CHIP_X"           /**< (asynInt32,    r/w) Number of pixels per chip in the x direction */
#define DPixelsPerChipYString    "D_PIXELS_PER_CHIP_Y"           /**< (asynInt32,    r/w) Number of pixels per chip in the y direction */
#define DChipsPerBlockXString    "D_CHIPS_PER_BLOCK_X"           /**< (asynInt32,    r/w) Number of chips in a block in the x direction */
#define DChipsPerBlockYString    "D_CHIPS_PER_BLOCK_Y"           /**< (asynInt32,    r/w) Number of chips in a block in the y direction */
#define DBlocksPerStripeXString  "D_BLOCKS_PER_STRIPE_X"         /**< (asynInt32,    r/w) Number of blocks in a stripe in the x direction */
#define DBlocksPerStripeYString  "D_BLOCKS_PER_STRIPE_Y"         /**< (asynInt32,    r/w) Number of blocks in a stripe in the y direction */
#define DChipsPerStripeXString   "D_CHIPS_PER_STRIPE_X"          /**< (asynInt32,    r/w) Number of chips in a stripe in the x direction */
#define DChipsPerStripeYString   "D_CHIPS_PER_STRIPE_Y"          /**< (asynInt32,    r/w) Number of chips in a stripe in the y direction */
#define DStripesPerImageXString  "D_STRIPES_PER_IMAGE_X"         /**< (asynInt32,    r/w) Number of stripes in an image in the x direction */
#define DStripesPerImageYString  "D_STRIPES_PER_IMAGE_Y"         /**< (asynInt32,    r/w) Number of stripes in an image in the y direction */

#define ImageScrambleTypeString  "IMAGE_SCRAMBLE_TYPE"           /**< (asynInt32,    r/w) Type of image scramble to execute (excalibur, percival) */

#define GDDebugLevelString         "GD_DEBUG_LEVEL"              /**< (asynInt32,    r/w) Debug level for low level non-EPICS code */

#define GDPostCommandString        "GD_POST_COMMAND"             /**< (asynInt32,    r/w) Start/stop posting */

#define GDNumImagesString          "GD_NUM_IMAGES"               /**< (asynInt32,    r/w) Number of images to post */
#define GDNumImagesCounterString   "GD_NUM_IMAGES_COUNTER"       /**< (asynInt32,    r/w) Number of images posted */
#define GDImageModeString          "GD_IMAGE_MODE"               /**< (asynInt32,    r/w) Post a single image/multiple images/ continuously */
#define GDModeString               "GD_MODE"                     /**< (asynInt32,    r/w) Generator mode (temporal or spatial) */

#define GDPostChannel1String       "GD_POST_CHANNEL_1"           /**< (asynInt32,    r/w) Are we posting on channel 1? */
#define GDPostChannel2String       "GD_POST_CHANNEL_2"           /**< (asynInt32,    r/w) Are we posting on channel 2? */
#define GDPostChannel3String       "GD_POST_CHANNEL_3"           /**< (asynInt32,    r/w) Are we posting on channel 3? */
#define GDPostChannel4String       "GD_POST_CHANNEL_4"           /**< (asynInt32,    r/w) Are we posting on channel 4? */
#define GDPostChannel5String       "GD_POST_CHANNEL_5"           /**< (asynInt32,    r/w) Are we posting on channel 5? */
#define GDPostChannel6String       "GD_POST_CHANNEL_6"           /**< (asynInt32,    r/w) Are we posting on channel 6? */
#define GDPostChannel7String       "GD_POST_CHANNEL_7"           /**< (asynInt32,    r/w) Are we posting on channel 7? */
#define GDPostChannel8String       "GD_POST_CHANNEL_8"           /**< (asynInt32,    r/w) Are we posting on channel 8? */

#define GDCounterChannel1String    "GD_COUNTER_CHANNEL_1"        /**< (asynInt32,    r/w) Current counter on channel 1? */
#define GDCounterChannel2String    "GD_COUNTER_CHANNEL_2"        /**< (asynInt32,    r/w) Current counter on channel 2? */
#define GDCounterChannel3String    "GD_COUNTER_CHANNEL_3"        /**< (asynInt32,    r/w) Current counter on channel 3? */
#define GDCounterChannel4String    "GD_COUNTER_CHANNEL_4"        /**< (asynInt32,    r/w) Current counter on channel 4? */
#define GDCounterChannel5String    "GD_COUNTER_CHANNEL_5"        /**< (asynInt32,    r/w) Current counter on channel 5? */
#define GDCounterChannel6String    "GD_COUNTER_CHANNEL_6"        /**< (asynInt32,    r/w) Current counter on channel 6? */
#define GDCounterChannel7String    "GD_COUNTER_CHANNEL_7"        /**< (asynInt32,    r/w) Current counter on channel 7? */
#define GDCounterChannel8String    "GD_COUNTER_CHANNEL_8"        /**< (asynInt32,    r/w) Current counter on channel 8? */

#define GDEnableChannel1String     "GD_ENABLE_CHANNEL_1"         /**< (asynInt32,    r/w) Channel 1 enable/disable */
#define GDEnableChannel2String     "GD_ENABLE_CHANNEL_2"         /**< (asynInt32,    r/w) Channel 2 enable/disable */
#define GDEnableChannel3String     "GD_ENABLE_CHANNEL_3"         /**< (asynInt32,    r/w) Channel 3 enable/disable */
#define GDEnableChannel4String     "GD_ENABLE_CHANNEL_4"         /**< (asynInt32,    r/w) Channel 4 enable/disable */
#define GDEnableChannel5String     "GD_ENABLE_CHANNEL_5"         /**< (asynInt32,    r/w) Channel 5 enable/disable */
#define GDEnableChannel6String     "GD_ENABLE_CHANNEL_6"         /**< (asynInt32,    r/w) Channel 6 enable/disable */
#define GDEnableChannel7String     "GD_ENABLE_CHANNEL_7"         /**< (asynInt32,    r/w) Channel 7 enable/disable */
#define GDEnableChannel8String     "GD_ENABLE_CHANNEL_8"         /**< (asynInt32,    r/w) Channel 8 enable/disable */

#define GDTopLeftXChannel1String   "GD_TOP_LEFT_X_CHANNEL_1"     /**< (asynInt32,    r/w) Channel 1 top left X pixel */
#define GDTopLeftXChannel2String   "GD_TOP_LEFT_X_CHANNEL_2"     /**< (asynInt32,    r/w) Channel 2 top left X pixel */
#define GDTopLeftXChannel3String   "GD_TOP_LEFT_X_CHANNEL_3"     /**< (asynInt32,    r/w) Channel 3 top left X pixel */
#define GDTopLeftXChannel4String   "GD_TOP_LEFT_X_CHANNEL_4"     /**< (asynInt32,    r/w) Channel 4 top left X pixel */
#define GDTopLeftXChannel5String   "GD_TOP_LEFT_X_CHANNEL_5"     /**< (asynInt32,    r/w) Channel 5 top left X pixel */
#define GDTopLeftXChannel6String   "GD_TOP_LEFT_X_CHANNEL_6"     /**< (asynInt32,    r/w) Channel 6 top left X pixel */
#define GDTopLeftXChannel7String   "GD_TOP_LEFT_X_CHANNEL_7"     /**< (asynInt32,    r/w) Channel 7 top left X pixel */
#define GDTopLeftXChannel8String   "GD_TOP_LEFT_X_CHANNEL_8"     /**< (asynInt32,    r/w) Channel 8 top left X pixel */

#define GDTopLeftYChannel1String   "GD_TOP_LEFT_Y_CHANNEL_1"     /**< (asynInt32,    r/w) Channel 1 top left Y pixel */
#define GDTopLeftYChannel2String   "GD_TOP_LEFT_Y_CHANNEL_2"     /**< (asynInt32,    r/w) Channel 2 top left Y pixel */
#define GDTopLeftYChannel3String   "GD_TOP_LEFT_Y_CHANNEL_3"     /**< (asynInt32,    r/w) Channel 3 top left Y pixel */
#define GDTopLeftYChannel4String   "GD_TOP_LEFT_Y_CHANNEL_4"     /**< (asynInt32,    r/w) Channel 4 top left Y pixel */
#define GDTopLeftYChannel5String   "GD_TOP_LEFT_Y_CHANNEL_5"     /**< (asynInt32,    r/w) Channel 5 top left Y pixel */
#define GDTopLeftYChannel6String   "GD_TOP_LEFT_Y_CHANNEL_6"     /**< (asynInt32,    r/w) Channel 6 top left Y pixel */
#define GDTopLeftYChannel7String   "GD_TOP_LEFT_Y_CHANNEL_7"     /**< (asynInt32,    r/w) Channel 7 top left Y pixel */
#define GDTopLeftYChannel8String   "GD_TOP_LEFT_Y_CHANNEL_8"     /**< (asynInt32,    r/w) Channel 8 top left Y pixel */

#define GDBotRightXChannel1String  "GD_BOT_RIGHT_X_CHANNEL_1"    /**< (asynInt32,    r/w) Channel 1 bottom right X pixel */
#define GDBotRightXChannel2String  "GD_BOT_RIGHT_X_CHANNEL_2"    /**< (asynInt32,    r/w) Channel 2 bottom right X pixel */
#define GDBotRightXChannel3String  "GD_BOT_RIGHT_X_CHANNEL_3"    /**< (asynInt32,    r/w) Channel 3 bottom right X pixel */
#define GDBotRightXChannel4String  "GD_BOT_RIGHT_X_CHANNEL_4"    /**< (asynInt32,    r/w) Channel 4 bottom right X pixel */
#define GDBotRightXChannel5String  "GD_BOT_RIGHT_X_CHANNEL_5"    /**< (asynInt32,    r/w) Channel 5 bottom right X pixel */
#define GDBotRightXChannel6String  "GD_BOT_RIGHT_X_CHANNEL_6"    /**< (asynInt32,    r/w) Channel 6 bottom right X pixel */
#define GDBotRightXChannel7String  "GD_BOT_RIGHT_X_CHANNEL_7"    /**< (asynInt32,    r/w) Channel 7 bottom right X pixel */
#define GDBotRightXChannel8String  "GD_BOT_RIGHT_X_CHANNEL_8"    /**< (asynInt32,    r/w) Channel 8 bottom right X pixel */

#define GDBotRightYChannel1String  "GD_BOT_RIGHT_Y_CHANNEL_1"    /**< (asynInt32,    r/w) Channel 1 bottom right Y pixel */
#define GDBotRightYChannel2String  "GD_BOT_RIGHT_Y_CHANNEL_2"    /**< (asynInt32,    r/w) Channel 2 bottom right Y pixel */
#define GDBotRightYChannel3String  "GD_BOT_RIGHT_Y_CHANNEL_3"    /**< (asynInt32,    r/w) Channel 3 bottom right Y pixel */
#define GDBotRightYChannel4String  "GD_BOT_RIGHT_Y_CHANNEL_4"    /**< (asynInt32,    r/w) Channel 4 bottom right Y pixel */
#define GDBotRightYChannel5String  "GD_BOT_RIGHT_Y_CHANNEL_5"    /**< (asynInt32,    r/w) Channel 5 bottom right Y pixel */
#define GDBotRightYChannel6String  "GD_BOT_RIGHT_Y_CHANNEL_6"    /**< (asynInt32,    r/w) Channel 6 bottom right Y pixel */
#define GDBotRightYChannel7String  "GD_BOT_RIGHT_Y_CHANNEL_7"    /**< (asynInt32,    r/w) Channel 7 bottom right Y pixel */
#define GDBotRightYChannel8String  "GD_BOT_RIGHT_Y_CHANNEL_8"    /**< (asynInt32,    r/w) Channel 8 bottom right Y pixel */

#define GDWidthChannel1String      "GD_WIDTH_CHANNEL_1"          /**< (asynInt32,    r/o) Channel 1 frame width (pixels) */
#define GDWidthChannel2String      "GD_WIDTH_CHANNEL_2"          /**< (asynInt32,    r/o) Channel 2 frame width (pixels) */
#define GDWidthChannel3String      "GD_WIDTH_CHANNEL_3"          /**< (asynInt32,    r/o) Channel 3 frame width (pixels) */
#define GDWidthChannel4String      "GD_WIDTH_CHANNEL_4"          /**< (asynInt32,    r/o) Channel 4 frame width (pixels) */
#define GDWidthChannel5String      "GD_WIDTH_CHANNEL_5"          /**< (asynInt32,    r/o) Channel 5 frame width (pixels) */
#define GDWidthChannel6String      "GD_WIDTH_CHANNEL_6"          /**< (asynInt32,    r/o) Channel 6 frame width (pixels) */
#define GDWidthChannel7String      "GD_WIDTH_CHANNEL_7"          /**< (asynInt32,    r/o) Channel 7 frame width (pixels) */
#define GDWidthChannel8String      "GD_WIDTH_CHANNEL_8"          /**< (asynInt32,    r/o) Channel 8 frame width (pixels) */

#define GDHeightChannel1String     "GD_HEIGHT_CHANNEL_1"         /**< (asynInt32,    r/o) Channel 1 frame height (pixels) */
#define GDHeightChannel2String     "GD_HEIGHT_CHANNEL_2"         /**< (asynInt32,    r/o) Channel 2 frame height (pixels) */
#define GDHeightChannel3String     "GD_HEIGHT_CHANNEL_3"         /**< (asynInt32,    r/o) Channel 3 frame height (pixels) */
#define GDHeightChannel4String     "GD_HEIGHT_CHANNEL_4"         /**< (asynInt32,    r/o) Channel 4 frame height (pixels) */
#define GDHeightChannel5String     "GD_HEIGHT_CHANNEL_5"         /**< (asynInt32,    r/o) Channel 5 frame height (pixels) */
#define GDHeightChannel6String     "GD_HEIGHT_CHANNEL_6"         /**< (asynInt32,    r/o) Channel 6 frame height (pixels) */
#define GDHeightChannel7String     "GD_HEIGHT_CHANNEL_7"         /**< (asynInt32,    r/o) Channel 7 frame height (pixels) */
#define GDHeightChannel8String     "GD_HEIGHT_CHANNEL_8"         /**< (asynInt32,    r/o) Channel 8 frame height (pixels) */

#define GDSubFrameChannel1String   "GD_SUB_FRAME_CHANNEL_1"      /**< (asynInt32,    r/w) Channel 1 subframe number */
#define GDSubFrameChannel2String   "GD_SUB_FRAME_CHANNEL_2"      /**< (asynInt32,    r/w) Channel 2 subframe number */
#define GDSubFrameChannel3String   "GD_SUB_FRAME_CHANNEL_3"      /**< (asynInt32,    r/w) Channel 3 subframe number */
#define GDSubFrameChannel4String   "GD_SUB_FRAME_CHANNEL_4"      /**< (asynInt32,    r/w) Channel 4 subframe number */
#define GDSubFrameChannel5String   "GD_SUB_FRAME_CHANNEL_5"      /**< (asynInt32,    r/w) Channel 5 subframe number */
#define GDSubFrameChannel6String   "GD_SUB_FRAME_CHANNEL_6"      /**< (asynInt32,    r/w) Channel 6 subframe number */
#define GDSubFrameChannel7String   "GD_SUB_FRAME_CHANNEL_7"      /**< (asynInt32,    r/w) Channel 7 subframe number */
#define GDSubFrameChannel8String   "GD_SUB_FRAME_CHANNEL_8"      /**< (asynInt32,    r/w) Channel 8 subframe number */

#define GDPacketSizeChannel1String "GD_PKT_SIZE_CHANNEL_1"       /**< (asynInt32,    r/w) Channel 1 packet size in pixels */
#define GDPacketSizeChannel2String "GD_PKT_SIZE_CHANNEL_2"       /**< (asynInt32,    r/w) Channel 2 packet size in pixels */
#define GDPacketSizeChannel3String "GD_PKT_SIZE_CHANNEL_3"       /**< (asynInt32,    r/w) Channel 3 packet size in pixels */
#define GDPacketSizeChannel4String "GD_PKT_SIZE_CHANNEL_4"       /**< (asynInt32,    r/w) Channel 4 packet size in pixels */
#define GDPacketSizeChannel5String "GD_PKT_SIZE_CHANNEL_5"       /**< (asynInt32,    r/w) Channel 5 packet size in pixels */
#define GDPacketSizeChannel6String "GD_PKT_SIZE_CHANNEL_6"       /**< (asynInt32,    r/w) Channel 6 packet size in pixels */
#define GDPacketSizeChannel7String "GD_PKT_SIZE_CHANNEL_7"       /**< (asynInt32,    r/w) Channel 7 packet size in pixels */
#define GDPacketSizeChannel8String "GD_PKT_SIZE_CHANNEL_8"       /**< (asynInt32,    r/w) Channel 8 packet size in pixels */

#define GDLocAddrChannel1String    "GD_LOC_ADDR_CHANNEL_1"       /**< (asynOctet,    r/w) Channel 1 local address string */
#define GDLocAddrChannel2String    "GD_LOC_ADDR_CHANNEL_2"       /**< (asynOctet,    r/w) Channel 2 local address string */
#define GDLocAddrChannel3String    "GD_LOC_ADDR_CHANNEL_3"       /**< (asynOctet,    r/w) Channel 3 local address string */
#define GDLocAddrChannel4String    "GD_LOC_ADDR_CHANNEL_4"       /**< (asynOctet,    r/w) Channel 4 local address string */
#define GDLocAddrChannel5String    "GD_LOC_ADDR_CHANNEL_5"       /**< (asynOctet,    r/w) Channel 5 local address string */
#define GDLocAddrChannel6String    "GD_LOC_ADDR_CHANNEL_6"       /**< (asynOctet,    r/w) Channel 6 local address string */
#define GDLocAddrChannel7String    "GD_LOC_ADDR_CHANNEL_7"       /**< (asynOctet,    r/w) Channel 7 local address string */
#define GDLocAddrChannel8String    "GD_LOC_ADDR_CHANNEL_8"       /**< (asynOctet,    r/w) Channel 8 local address string */

#define GDLocPortChannel1String    "GD_LOC_PORT_CHANNEL_1"       /**< (asynInt32,    r/w) Channel 1 local port string */
#define GDLocPortChannel2String    "GD_LOC_PORT_CHANNEL_2"       /**< (asynInt32,    r/w) Channel 2 local port string */
#define GDLocPortChannel3String    "GD_LOC_PORT_CHANNEL_3"       /**< (asynInt32,    r/w) Channel 3 local port string */
#define GDLocPortChannel4String    "GD_LOC_PORT_CHANNEL_4"       /**< (asynInt32,    r/w) Channel 4 local port string */
#define GDLocPortChannel5String    "GD_LOC_PORT_CHANNEL_5"       /**< (asynInt32,    r/w) Channel 5 local port string */
#define GDLocPortChannel6String    "GD_LOC_PORT_CHANNEL_6"       /**< (asynInt32,    r/w) Channel 6 local port string */
#define GDLocPortChannel7String    "GD_LOC_PORT_CHANNEL_7"       /**< (asynInt32,    r/w) Channel 7 local port string */
#define GDLocPortChannel8String    "GD_LOC_PORT_CHANNEL_8"       /**< (asynInt32,    r/w) Channel 8 local port string */

#define GDRemAddrChannel1String    "GD_REM_ADDR_CHANNEL_1"       /**< (asynOctet,    r/w) Channel 1 remote address string */
#define GDRemAddrChannel2String    "GD_REM_ADDR_CHANNEL_2"       /**< (asynOctet,    r/w) Channel 2 remote address string */
#define GDRemAddrChannel3String    "GD_REM_ADDR_CHANNEL_3"       /**< (asynOctet,    r/w) Channel 3 remote address string */
#define GDRemAddrChannel4String    "GD_REM_ADDR_CHANNEL_4"       /**< (asynOctet,    r/w) Channel 4 remote address string */
#define GDRemAddrChannel5String    "GD_REM_ADDR_CHANNEL_5"       /**< (asynOctet,    r/w) Channel 5 remote address string */
#define GDRemAddrChannel6String    "GD_REM_ADDR_CHANNEL_6"       /**< (asynOctet,    r/w) Channel 6 remote address string */
#define GDRemAddrChannel7String    "GD_REM_ADDR_CHANNEL_7"       /**< (asynOctet,    r/w) Channel 7 remote address string */
#define GDRemAddrChannel8String    "GD_REM_ADDR_CHANNEL_8"       /**< (asynOctet,    r/w) Channel 8 remote address string */

#define GDRemPortChannel1String    "GD_REM_PORT_CHANNEL_1"       /**< (asynInt32,    r/w) Channel 1 remote port string */
#define GDRemPortChannel2String    "GD_REM_PORT_CHANNEL_2"       /**< (asynInt32,    r/w) Channel 2 remote port string */
#define GDRemPortChannel3String    "GD_REM_PORT_CHANNEL_3"       /**< (asynInt32,    r/w) Channel 3 remote port string */
#define GDRemPortChannel4String    "GD_REM_PORT_CHANNEL_4"       /**< (asynInt32,    r/w) Channel 4 remote port string */
#define GDRemPortChannel5String    "GD_REM_PORT_CHANNEL_5"       /**< (asynInt32,    r/w) Channel 5 remote port string */
#define GDRemPortChannel6String    "GD_REM_PORT_CHANNEL_6"       /**< (asynInt32,    r/w) Channel 6 remote port string */
#define GDRemPortChannel7String    "GD_REM_PORT_CHANNEL_7"       /**< (asynInt32,    r/w) Channel 7 remote port string */
#define GDRemPortChannel8String    "GD_REM_PORT_CHANNEL_8"       /**< (asynInt32,    r/w) Channel 8 remote port string */

#define GDErrorChannel1String      "GD_ERROR_CHANNEL_1"          /**< (asynInt32,    r/0) Channel 1 error string */
#define GDErrorChannel2String      "GD_ERROR_CHANNEL_2"          /**< (asynInt32,    r/0) Channel 2 error string */
#define GDErrorChannel3String      "GD_ERROR_CHANNEL_3"          /**< (asynInt32,    r/0) Channel 3 error string */
#define GDErrorChannel4String      "GD_ERROR_CHANNEL_4"          /**< (asynInt32,    r/0) Channel 4 error string */
#define GDErrorChannel5String      "GD_ERROR_CHANNEL_5"          /**< (asynInt32,    r/0) Channel 5 error string */
#define GDErrorChannel6String      "GD_ERROR_CHANNEL_6"          /**< (asynInt32,    r/0) Channel 6 error string */
#define GDErrorChannel7String      "GD_ERROR_CHANNEL_7"          /**< (asynInt32,    r/0) Channel 7 error string */
#define GDErrorChannel8String      "GD_ERROR_CHANNEL_8"          /**< (asynInt32,    r/0) Channel 8 error string */
#define GDErrorMsgChannel1String   "GD_ERROR_MSG_CHANNEL_1"      /**< (asynOctet,    r/0) Channel 1 error message string */
#define GDErrorMsgChannel2String   "GD_ERROR_MSG_CHANNEL_2"      /**< (asynOctet,    r/0) Channel 2 error message string */
#define GDErrorMsgChannel3String   "GD_ERROR_MSG_CHANNEL_3"      /**< (asynOctet,    r/0) Channel 3 error message string */
#define GDErrorMsgChannel4String   "GD_ERROR_MSG_CHANNEL_4"      /**< (asynOctet,    r/0) Channel 4 error message string */
#define GDErrorMsgChannel5String   "GD_ERROR_MSG_CHANNEL_5"      /**< (asynOctet,    r/0) Channel 5 error message string */
#define GDErrorMsgChannel6String   "GD_ERROR_MSG_CHANNEL_6"      /**< (asynOctet,    r/0) Channel 6 error message string */
#define GDErrorMsgChannel7String   "GD_ERROR_MSG_CHANNEL_7"      /**< (asynOctet,    r/0) Channel 7 error message string */
#define GDErrorMsgChannel8String   "GD_ERROR_MSG_CHANNEL_8"      /**< (asynOctet,    r/0) Channel 8 error message string */

#define GDPostFrequencyString    "GD_POST_FREQUENCY"             /**< (asynFloat64,  r/w) Frequency of image posting */

#define GDReportString             "GD_REPORT"                   /**< (asynInt32,    r/w) Print out a report of packets sent */

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

    void freeImageBuffers();
    void setupImageBuffers();

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
    int FileReadStatus;
    int FileReadMessage;
    int FileErrorStatus;
    int DPixelsPerChipX;
    int DPixelsPerChipY;
    int DChipsPerBlockX;
    int DChipsPerBlockY;
    int DBlocksPerStripeX;
    int DBlocksPerStripeY;
    int DChipsPerStripeX;
    int DChipsPerStripeY;
    int DStripesPerImageX;
    int DStripesPerImageY;
    int ImageScrambleType;

    int GDDebugLevel;

    int GDNumImages;
    int GDNumImagesCounter;
    int GDImageMode;
    int GDMode;

    // The following attributes are required for the eight posting threads
    int GDPostCommand;
    int GDPostChannel1;
    int GDPostChannel2;
    int GDPostChannel3;
    int GDPostChannel4;
    int GDPostChannel5;
    int GDPostChannel6;
    int GDPostChannel7;
    int GDPostChannel8;

    int GDCounterChannel1;
    int GDCounterChannel2;
    int GDCounterChannel3;
    int GDCounterChannel4;
    int GDCounterChannel5;
    int GDCounterChannel6;
    int GDCounterChannel7;
    int GDCounterChannel8;

    int GDEnableChannel1;
    int GDEnableChannel2;
    int GDEnableChannel3;
    int GDEnableChannel4;
    int GDEnableChannel5;
    int GDEnableChannel6;
    int GDEnableChannel7;
    int GDEnableChannel8;

    int GDTopLeftXChannel1;
    int GDTopLeftXChannel2;
    int GDTopLeftXChannel3;
    int GDTopLeftXChannel4;
    int GDTopLeftXChannel5;
    int GDTopLeftXChannel6;
    int GDTopLeftXChannel7;
    int GDTopLeftXChannel8;
    int GDTopLeftYChannel1;
    int GDTopLeftYChannel2;
    int GDTopLeftYChannel3;
    int GDTopLeftYChannel4;
    int GDTopLeftYChannel5;
    int GDTopLeftYChannel6;
    int GDTopLeftYChannel7;
    int GDTopLeftYChannel8;
    int GDBotRightXChannel1;
    int GDBotRightXChannel2;
    int GDBotRightXChannel3;
    int GDBotRightXChannel4;
    int GDBotRightXChannel5;
    int GDBotRightXChannel6;
    int GDBotRightXChannel7;
    int GDBotRightXChannel8;
    int GDBotRightYChannel1;
    int GDBotRightYChannel2;
    int GDBotRightYChannel3;
    int GDBotRightYChannel4;
    int GDBotRightYChannel5;
    int GDBotRightYChannel6;
    int GDBotRightYChannel7;
    int GDBotRightYChannel8;

    int GDWidthChannel1;
    int GDWidthChannel2;
    int GDWidthChannel3;
    int GDWidthChannel4;
    int GDWidthChannel5;
    int GDWidthChannel6;
    int GDWidthChannel7;
    int GDWidthChannel8;
    int GDHeightChannel1;
    int GDHeightChannel2;
    int GDHeightChannel3;
    int GDHeightChannel4;
    int GDHeightChannel5;
    int GDHeightChannel6;
    int GDHeightChannel7;
    int GDHeightChannel8;

    int GDSubFrameChannel1;
    int GDSubFrameChannel2;
    int GDSubFrameChannel3;
    int GDSubFrameChannel4;
    int GDSubFrameChannel5;
    int GDSubFrameChannel6;
    int GDSubFrameChannel7;
    int GDSubFrameChannel8;

    int GDPacketSizeChannel1;
    int GDPacketSizeChannel2;
    int GDPacketSizeChannel3;
    int GDPacketSizeChannel4;
    int GDPacketSizeChannel5;
    int GDPacketSizeChannel6;
    int GDPacketSizeChannel7;
    int GDPacketSizeChannel8;

    int GDLocAddrChannel1;
    int GDLocAddrChannel2;
    int GDLocAddrChannel3;
    int GDLocAddrChannel4;
    int GDLocAddrChannel5;
    int GDLocAddrChannel6;
    int GDLocAddrChannel7;
    int GDLocAddrChannel8;
    int GDLocPortChannel1;
    int GDLocPortChannel2;
    int GDLocPortChannel3;
    int GDLocPortChannel4;
    int GDLocPortChannel5;
    int GDLocPortChannel6;
    int GDLocPortChannel7;
    int GDLocPortChannel8;
    int GDRemAddrChannel1;
    int GDRemAddrChannel2;
    int GDRemAddrChannel3;
    int GDRemAddrChannel4;
    int GDRemAddrChannel5;
    int GDRemAddrChannel6;
    int GDRemAddrChannel7;
    int GDRemAddrChannel8;
    int GDRemPortChannel1;
    int GDRemPortChannel2;
    int GDRemPortChannel3;
    int GDRemPortChannel4;
    int GDRemPortChannel5;
    int GDRemPortChannel6;
    int GDRemPortChannel7;
    int GDRemPortChannel8;

    int GDErrorChannel1;
    int GDErrorChannel2;
    int GDErrorChannel3;
    int GDErrorChannel4;
    int GDErrorChannel5;
    int GDErrorChannel6;
    int GDErrorChannel7;
    int GDErrorChannel8;
    int GDErrorMsgChannel1;
    int GDErrorMsgChannel2;
    int GDErrorMsgChannel3;
    int GDErrorMsgChannel4;
    int GDErrorMsgChannel5;
    int GDErrorMsgChannel6;
    int GDErrorMsgChannel7;
    int GDErrorMsgChannel8;

    int GDReport;

    int GDPostFrequency;
    #define LAST_GENERATOR_PARAM GDPostFrequency

    int GDPostBase;
    int GDCounterBase;
    int GDTopLeftXBase;
    int GDTopLeftYBase;
    int GDBotRightXBase;
    int GDBotRightYBase;
    int GDWidthBase;
    int GDHeightBase;
    int GDSubFrameBase;
    int GDPacketSizeBase;
    int GDLocAddrBase;
    int GDLocPortBase;
    int GDRemAddrBase;
    int GDRemPortBase;
    int GDErrorBase;
    int GDErrorMsgBase;

  private:
    Configurator *configPtr;
    DataSender *senderPtr;

    epicsEventId startEventId_[8];
    epicsEventId stopEventId_[8];

    void **buffer1_;
    void **buffer2_;
    void **buffer3_;
    void **buffer4_;
    void **buffer5_;
    void **buffer6_;
    void **buffer7_;
    void **buffer8_;
    void **reset1_;
    void **reset2_;
    void **reset3_;
    void **reset4_;
    void **reset5_;
    void **reset6_;
    void **reset7_;
    void **reset8_;
    uint32_t bufSize1_;
    uint32_t bufSize2_;
    uint32_t bufSize3_;
    uint32_t bufSize4_;
    uint32_t bufSize5_;
    uint32_t bufSize6_;
    uint32_t bufSize7_;
    uint32_t bufSize8_;
    uint32_t noOfImages_;

};

#define NUM_GENERATOR_PARAMS (&LAST_GENERATOR_PARAM - &FIRST_GENERATOR_PARAM + 1)
#endif // ASYNGENERATORDRIVER_H_

