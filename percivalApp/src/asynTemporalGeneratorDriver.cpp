/*
 * asynGeneratorDriver.cpp
 *
 *  Created on: 6th Aug 2013
 *      Author: gnx91527
 */

#include <string.h>
#include <sys/stat.h>
#include <iocsh.h>
#include <epicsExport.h>

#include "asynTemporalGeneratorDriver.h"

static const char *driverName = "asynGeneratorDriver";


/*
 * These eight routines start each of the eight posting
 * threads, 1 for each channel
 */
static void post_task_1_c(void *ptr)
{
	asynGeneratorDriver *drvPtr = (asynGeneratorDriver *)ptr;
  drvPtr->posting_task(0);
}

static void post_task_2_c(void *ptr)
{
	asynGeneratorDriver *drvPtr = (asynGeneratorDriver *)ptr;
  drvPtr->posting_task(1);
}

static void post_task_3_c(void *ptr)
{
	asynGeneratorDriver *drvPtr = (asynGeneratorDriver *)ptr;
  drvPtr->posting_task(2);
}

static void post_task_4_c(void *ptr)
{
	asynGeneratorDriver *drvPtr = (asynGeneratorDriver *)ptr;
  drvPtr->posting_task(3);
}

static void post_task_5_c(void *ptr)
{
	asynGeneratorDriver *drvPtr = (asynGeneratorDriver *)ptr;
  drvPtr->posting_task(4);
}

static void post_task_6_c(void *ptr)
{
	asynGeneratorDriver *drvPtr = (asynGeneratorDriver *)ptr;
  drvPtr->posting_task(5);
}

static void post_task_7_c(void *ptr)
{
	asynGeneratorDriver *drvPtr = (asynGeneratorDriver *)ptr;
  drvPtr->posting_task(6);
}

static void post_task_8_c(void *ptr)
{
	asynGeneratorDriver *drvPtr = (asynGeneratorDriver *)ptr;
  drvPtr->posting_task(7);
}




/** Called when asyn clients call pasynOctet->write().
  * This function performs actions for some parameters, including AttributesFile.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Address of the string to write.
  * \param[in] nChars Number of characters to write.
  * \param[out] nActual Number of characters actually written. */
asynStatus asynGeneratorDriver::writeOctet(asynUser *pasynUser, const char *value, size_t nChars, size_t *nActual)
{
  int addr=0;
  int function = pasynUser->reason;
  asynStatus status = asynSuccess;
  const char *functionName = "writeOctet";

  status = getAddress(pasynUser, &addr); if (status != asynSuccess) return(status);
  // Set the parameter in the parameter library.
  status = (asynStatus)setStringParam(addr, function, (char *)value);

  if (function == FilePath) {
    this->checkPath();
  } else if (function == FileName){
    char fullName[MAX_FILENAME_LEN];
    this->createFileName(MAX_FILENAME_LEN, fullName);
    setStringParam(addr, FullFileName, fullName);
  }
  // Do callbacks so higher layers see any changes
  status = (asynStatus)callParamCallbacks(addr, addr);

  if (status){
    epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize, 
                  "%s:%s: status=%d, function=%d, value=%s", 
                  driverName, functionName, status, function, value);
  } else {
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
              "%s:%s: function=%d, value=%s\n", 
              driverName, functionName, function, value);
  }
  *nActual = nChars;
  return status;
}

asynStatus asynGeneratorDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int function = pasynUser->reason;
  int status = asynSuccess;

  if (function == RawFileRead){
    char fileName[MAX_FILENAME_LEN];
    setIntegerParam(FileReadStatus, 1);
    setStringParam (FileReadMessage, "Reading configuration file");
    callParamCallbacks();
    status |= getStringParam(FullFileName, sizeof(fileName), fileName);
    status = configPtr->readConfiguration(fileName);
    if (status){
      // Error reading the file.  Notify
      setStringParam(FileReadMessage, configPtr->errorMessage().c_str());
      setIntegerParam(FileReadStatus, 0);
      setIntegerParam(FileErrorStatus, 1);
    } else {
      setStringParam(FileReadMessage, "Read configuration complete");
      setIntegerParam(FileReadStatus, 0);
      setIntegerParam(ImageSizeX,        configPtr->getImageWidth());
      setIntegerParam(ImageSizeY,        configPtr->getImageHeight());
      setIntegerParam(ImagePatternX,     configPtr->getRepeatX());
      setIntegerParam(ImagePatternY,     configPtr->getRepeatY());
      setIntegerParam(DPixelsPerChipX,   configPtr->getPixelsPerChipX());
      setIntegerParam(DPixelsPerChipY,   configPtr->getPixelsPerChipY());
      setIntegerParam(DChipsPerBlockX,   configPtr->getChipsPerBlockX());
      setIntegerParam(DChipsPerBlockY,   configPtr->getChipsPerBlockY());
      setIntegerParam(DBlocksPerStripeX, configPtr->getBlocksPerStripeX());
      setIntegerParam(DBlocksPerStripeY, configPtr->getBlocksPerStripeY());
      setIntegerParam(DChipsPerStripeX,  configPtr->getChipsPerStripeX());
      setIntegerParam(DChipsPerStripeY,  configPtr->getChipsPerStripeY());
      setIntegerParam(DStripesPerImageX, configPtr->getStripesPerImageX());
      setIntegerParam(DStripesPerImageY, configPtr->getStripesPerImageY());
      setIntegerParam(DataType, configPtr->getDataType());
    }
  } else if (function == GDPostCommand){
    // Are we starting posting
    if (value == 1){
      int error = 0;
      // First we must check for errors
      for (int channel = 0; channel < 8; channel++){
        int tlx, tly, brx, bry, subFrame, enabled;
        // Reset errors
        setIntegerParam(GDErrorChannel1+channel, 0);
        setStringParam(GDErrorMsgChannel1+channel, "");
        // Check to see if the subframe is enabled
        getIntegerParam(GDEnableChannel1+channel, &enabled);
        if (!error){
          if (enabled){
            // Subframe is enabled, read out parameters
            getIntegerParam(GDSubFrameChannel1+channel, &subFrame);
            getIntegerParam(GDTopLeftXChannel1+channel, &tlx);
            getIntegerParam(GDTopLeftYChannel1+channel, &tly);
            getIntegerParam(GDBotRightXChannel1+channel, &brx);
            getIntegerParam(GDBotRightYChannel1+channel, &bry);
            // Perfom a few checks
            if (tlx >= brx){
              error = 1;
              setStringParam(GDErrorMsgChannel1+channel, "Width must be > 0");
            }
            if (tly >= bry){
              error = 1;
              setStringParam(GDErrorMsgChannel1+channel, "Height must be > 0");
            }
            if (subFrame < 0){
              error = 1;
              setStringParam(GDErrorMsgChannel1+channel, "Subframe number must be >= 0");
            }
            if (error){
              setIntegerParam(GDErrorChannel1+channel, 1);
            }
          }
        }
      }
      // Only proceed if there are no errors
      if (!error){
        // Here we are starting the posting
        int channelEnabled = 0;

        // First free if necessary and then reallocate buffers for temporal mode
        freeImageBuffers();
        setupImageBuffers();

        // For each channel check the enabled state. If enabled then set the
        // channel specific posting "ON" and send the event
        getIntegerParam(GDEnableChannel1, &channelEnabled);
        if (channelEnabled){
          setIntegerParam(GDPostChannel1, 1);
          epicsEventSignal(this->startEventId_[0]);
        }
        getIntegerParam(GDEnableChannel2, &channelEnabled);
        if (channelEnabled){
          setIntegerParam(GDPostChannel2, 1);
          epicsEventSignal(this->startEventId_[1]);
        }
        getIntegerParam(GDEnableChannel3, &channelEnabled);
        if (channelEnabled){
          setIntegerParam(GDPostChannel3, 1);
          epicsEventSignal(this->startEventId_[2]);
        }
        getIntegerParam(GDEnableChannel4, &channelEnabled);
        if (channelEnabled){
          setIntegerParam(GDPostChannel4, 1);
          epicsEventSignal(this->startEventId_[3]);
        }
        getIntegerParam(GDEnableChannel5, &channelEnabled);
        if (channelEnabled){
          setIntegerParam(GDPostChannel5, 1);
          epicsEventSignal(this->startEventId_[4]);
        }
        getIntegerParam(GDEnableChannel6, &channelEnabled);
        if (channelEnabled){
          setIntegerParam(GDPostChannel6, 1);
          epicsEventSignal(this->startEventId_[5]);
        }
        getIntegerParam(GDEnableChannel7, &channelEnabled);
        if (channelEnabled){
          setIntegerParam(GDPostChannel7, 1);
          epicsEventSignal(this->startEventId_[6]);
        }
        getIntegerParam(GDEnableChannel8, &channelEnabled);
        if (channelEnabled){
          setIntegerParam(GDPostChannel8, 1);
          epicsEventSignal(this->startEventId_[7]);
        }
      }
    } else {
      // Here we are stopping the posting
      int channelRunning = 0;
      // For each channel check the running state. If running then set the
      // channel specific posting "OFF" and send the event
      getIntegerParam(GDPostChannel1, &channelRunning);
      if (channelRunning){
        setIntegerParam(GDPostChannel1, 0);
        epicsEventSignal(this->stopEventId_[0]);
      }
      getIntegerParam(GDPostChannel2, &channelRunning);
      if (channelRunning){
        setIntegerParam(GDPostChannel2, 0);
        epicsEventSignal(this->stopEventId_[1]);
      }
      getIntegerParam(GDPostChannel3, &channelRunning);
      if (channelRunning){
        setIntegerParam(GDPostChannel3, 0);
        epicsEventSignal(this->stopEventId_[2]);
      }
      getIntegerParam(GDPostChannel4, &channelRunning);
      if (channelRunning){
        setIntegerParam(GDPostChannel4, 0);
        epicsEventSignal(this->stopEventId_[3]);
      }
      getIntegerParam(GDPostChannel5, &channelRunning);
      if (channelRunning){
        setIntegerParam(GDPostChannel5, 0);
        epicsEventSignal(this->stopEventId_[4]);
      }
      getIntegerParam(GDPostChannel6, &channelRunning);
      if (channelRunning){
        setIntegerParam(GDPostChannel6, 0);
        epicsEventSignal(this->stopEventId_[5]);
      }
      getIntegerParam(GDPostChannel7, &channelRunning);
      if (channelRunning){
        setIntegerParam(GDPostChannel7, 0);
        epicsEventSignal(this->stopEventId_[6]);
      }
      getIntegerParam(GDPostChannel8, &channelRunning);
      if (channelRunning){
        setIntegerParam(GDPostChannel8, 0);
        epicsEventSignal(this->stopEventId_[7]);
      }
    }
  } else if (function >= GDTopLeftXChannel1 && function <= GDBotRightYChannel8){
    // First update the value
    setIntegerParam(function, value);
    // Now calculate the width and height of each channel
    for (int channel = 0; channel < 8; channel++){
      int tlx, tly, brx, bry, width, height;
      getIntegerParam(GDTopLeftXBase+channel, &tlx);
      getIntegerParam(GDTopLeftYBase+channel, &tly);
      getIntegerParam(GDBotRightXBase+channel, &brx);
      getIntegerParam(GDBotRightYBase+channel, &bry);
      width = brx - tlx + 1;
      if (width < 0) width = 0;
      height = bry - tly + 1;
      if (height < 0) height = 0;
      setIntegerParam(GDWidthBase+channel, width);
      setIntegerParam(GDHeightBase+channel, height);
    }
  }

  // Call base class
  status = asynPortDriver::writeInt32(pasynUser, value);
  return (asynStatus)status;
}

asynStatus asynGeneratorDriver::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
  //int function = pasynUser->reason;
  asynStatus status = asynSuccess;

  // Just read the status of the NDArrayPool
  //if (function == NDPoolMaxBuffers) {
  //  setIntegerParam(function, this->pNDArrayPool->maxBuffers());
  //}

  // Call base class
  status = asynPortDriver::readInt32(pasynUser, value);
  return status;
}

//#define MEGABYTE_DBL 1048576.
asynStatus asynGeneratorDriver::readFloat64(asynUser *pasynUser, epicsFloat64 *value)
{
  //int function = pasynUser->reason;
  asynStatus status = asynSuccess;

  // Just read the status of the NDArrayPool
  //if (function == NDPoolMaxMemory) {
  //  setDoubleParam(function, this->pNDArrayPool->maxMemory() / MEGABYTE_DBL);
  //}

  // Call base class
  status = asynPortDriver::readFloat64(pasynUser, value);
  return status;
}


/** Report status of the driver.
  * This method calls the report function in the asynPortDriver base class.
  * \param[in] fp File pointed passed by caller where the output is written to.
  * \param[in] details
  */
void asynGeneratorDriver::report(FILE *fp, int details)
{
  asynPortDriver::report(fp, details);
}

/** Checks whether the directory specified FilePath parameter exists.
  * 
  * This is a convenience function that determines the directory specified 
  * FilePath parameter exists.
  * It sets the value of FilePathExists to 0 (does not exist) or 1 (exists).  
  * It also adds a trailing '/' character to the path if one is not present.
  * Returns a error status if the directory does not exist.
  */
int asynGeneratorDriver::checkPath()
{
  // Formats a complete file name from the components
  int status = asynError;
  char filePath[MAX_FILENAME_LEN];
  int hasTerminator=0;
  struct stat buff;
  int len;
  int isDir=0;
  int pathExists=0;
    
  status = getStringParam(FilePath, sizeof(filePath), filePath);
  len = strlen(filePath);
  if (len == 0) return(asynSuccess);
  // If the path contains a trailing '/' or '\' remove it, because Windows won't find
  // the directory if it has that trailing character
  if ((filePath[len-1] == '/') || (filePath[len-1] == '\\')) {
    filePath[len-1] = 0;
    len--;
    hasTerminator=1;
  }
  status = stat(filePath, &buff);
  if (!status) isDir = (S_IFDIR & buff.st_mode);
  if (!status && isDir) {
    pathExists = 1;
    status = asynSuccess;
  }
  // If the path did not have a trailing terminator then add it if there is room
  if (!hasTerminator) {
    if (len < MAX_FILENAME_LEN-2) strcat(filePath, "/");
    setStringParam(FilePath, filePath);
  }
  setIntegerParam(FilePathExists, pathExists);
  return(status);
}

/** Build a file name from component parts.
  * \param[in] maxChars  The size of the fullFileName string.
  * \param[out] fullFileName The constructed file name including the file path.
  * 
  * This is a convenience function that constructs a complete file name
  * from the FilePath, FileName, FileNumber, and FileTemplate parameters.
  * If AutoIncrement is true then it increments the FileNumber after creating
  * the file name.
  */
int asynGeneratorDriver::createFileName(int maxChars, char *fullFileName)
{
  // Formats a complete file name from the components defined in NDStdDriverParams
  int status = asynSuccess;
  char filePath[MAX_FILENAME_LEN];
  char fileName[MAX_FILENAME_LEN];
  int len;
    
  this->checkPath();
  status |= getStringParam(FilePath, sizeof(filePath), filePath);
  status |= getStringParam(FileName, sizeof(fileName), fileName); 
  //status |= getStringParam(FileTemplate, sizeof(fileTemplate), fileTemplate); 
  //status |= getIntegerParam(FileNumber, &fileNumber);
  //status |= getIntegerParam(AutoIncrement, &autoIncrement);
  if (status) return(status);
  len = epicsSnprintf(fullFileName, maxChars, "%s%s", filePath, fileName);
  if (len < 0) {
    status |= asynError;
      return(status);
  }
  //if (autoIncrement){
  //  fileNumber++;
  //  status |= setIntegerParam(FileNumber, fileNumber);
  //}
  return(status);   
}

/** Build a file name from component parts.
  * \param[in] maxChars  The size of the fullFileName string.
  * \param[out] filePath The file path.
  * \param[out] fileName The constructed file name without file file path.
  * 
  * This is a convenience function that constructs a file path and file name
  * from the FilePath, FileName, FileNumber, and FileTemplate parameters.
  * If AutoIncrement is true then it increments the FileNumber after creating
  * the file name.
  */
int asynGeneratorDriver::createFileName(int maxChars, char *filePath, char *fileName)
{
  // Formats a complete file name from the components defined in NDStdDriverParams
  int status = asynSuccess;
  char name[MAX_FILENAME_LEN];
    
  this->checkPath();
  status |= getStringParam(FilePath, maxChars, filePath); 
  status |= getStringParam(FileName, sizeof(name), fileName); 
  return(status);   
}

void asynGeneratorDriver::freeImageBuffers()
{
  uint32_t images = noOfImages_;

  if (images > 0){
  // If the buffer is not zero then free the memory
    if (buffer1_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(buffer1_[imageIndex]);
      }
      free(buffer1_);
      buffer1_ = 0;
    }
    if (buffer2_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(buffer2_[imageIndex]);
      }
      free(buffer2_);
      buffer2_ = 0;
    }
    if (buffer3_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(buffer3_[imageIndex]);
      }
      free(buffer3_);
      buffer3_ = 0;
    }
    if (buffer4_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(buffer4_[imageIndex]);
      }
      free(buffer4_);
      buffer4_ = 0;
    }
    if (buffer5_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(buffer5_[imageIndex]);
      }
      free(buffer5_);
      buffer5_ = 0;
    }
    if (buffer6_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(buffer6_[imageIndex]);
      }
      free(buffer6_);
      buffer6_ = 0;
    }
    if (buffer7_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(buffer7_[imageIndex]);
      }
      free(buffer7_);
      buffer7_ = 0;
    }
    if (buffer8_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(buffer8_[imageIndex]);
      }
      free(buffer8_);
      buffer8_ = 0;
    }
    if (reset1_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(reset1_[imageIndex]);
      }
      free(reset1_);
      reset1_ = 0;
    }
    if (reset2_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(reset2_[imageIndex]);
      }
      free(reset2_);
      reset2_ = 0;
    }
    if (reset3_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(reset3_[imageIndex]);
      }
      free(reset3_);
      reset3_ = 0;
    }
    if (reset4_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(reset4_[imageIndex]);
      }
      free(reset4_);
      reset4_ = 0;
    }
    if (reset5_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(reset5_[imageIndex]);
      }
      free(reset5_);
      reset5_ = 0;
    }
    if (reset6_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(reset6_[imageIndex]);
      }
      free(reset6_);
      reset6_ = 0;
    }
    if (reset7_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(reset7_[imageIndex]);
      }
      free(reset7_);
      reset7_ = 0;
    }
    if (reset8_){
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        free(reset8_[imageIndex]);
      }
      free(reset8_);
      reset8_ = 0;
    }
  }
}

void asynGeneratorDriver::setupImageBuffers()
{
  int subTlx, subTly, subBrx, subBry, sIWidth, sIHeight;
  uint32_t images = configPtr->getNoOfImages();
  noOfImages_ = images;

  // Read the sub image for sub frame 1
  getIntegerParam(GDTopLeftXChannel1, &subTlx);
  getIntegerParam(GDTopLeftYChannel1, &subTly);
  getIntegerParam(GDBotRightXChannel1, &subBrx);
  getIntegerParam(GDBotRightYChannel1, &subBry);
  sIWidth = subBrx - subTlx + 1;
  sIHeight = subBry - subTly + 1;
  bufSize1_ = (uint32_t)(sIWidth * sIHeight * sizeof(uint16_t));
  buffer1_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    buffer1_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyScrambledSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)buffer1_[imageIndex]);
  }
  // Allocate the storage for the reset data and 
  reset1_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    reset1_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyResetDataSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)reset1_[imageIndex]);
  }

  // Read the sub image for sub frame 2
  getIntegerParam(GDTopLeftXChannel2, &subTlx);
  getIntegerParam(GDTopLeftYChannel2, &subTly);
  getIntegerParam(GDBotRightXChannel2, &subBrx);
  getIntegerParam(GDBotRightYChannel2, &subBry);
  sIWidth = subBrx - subTlx + 1;
  sIHeight = subBry - subTly + 1;
  bufSize2_ = (uint32_t)(sIWidth * sIHeight * sizeof(uint16_t));
  buffer2_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    buffer2_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyScrambledSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)buffer2_[imageIndex]);
  }
  // Allocate the storage for the reset data and 
  reset2_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    reset2_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyResetDataSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)reset2_[imageIndex]);
  }

  // Read the sub image for sub frame 3
  getIntegerParam(GDTopLeftXChannel3, &subTlx);
  getIntegerParam(GDTopLeftYChannel3, &subTly);
  getIntegerParam(GDBotRightXChannel3, &subBrx);
  getIntegerParam(GDBotRightYChannel3, &subBry);
  sIWidth = subBrx - subTlx + 1;
  sIHeight = subBry - subTly + 1;
  bufSize3_ = (uint32_t)(sIWidth * sIHeight * sizeof(uint16_t));
  buffer3_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    buffer3_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyScrambledSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)buffer3_[imageIndex]);
  }
  // Allocate the storage for the reset data and 
  reset3_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    reset3_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyResetDataSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)reset3_[imageIndex]);
  }

  // Read the sub image for sub frame 4
  getIntegerParam(GDTopLeftXChannel4, &subTlx);
  getIntegerParam(GDTopLeftYChannel4, &subTly);
  getIntegerParam(GDBotRightXChannel4, &subBrx);
  getIntegerParam(GDBotRightYChannel4, &subBry);
  sIWidth = subBrx - subTlx + 1;
  sIHeight = subBry - subTly + 1;
  bufSize4_ = (uint32_t)(sIWidth * sIHeight * sizeof(uint16_t));
  buffer4_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    buffer4_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyScrambledSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)buffer4_[imageIndex]);
  }
  // Allocate the storage for the reset data and 
  reset4_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    reset4_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyResetDataSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)reset4_[imageIndex]);
  }

  // Read the sub image for sub frame 5
  getIntegerParam(GDTopLeftXChannel5, &subTlx);
  getIntegerParam(GDTopLeftYChannel5, &subTly);
  getIntegerParam(GDBotRightXChannel5, &subBrx);
  getIntegerParam(GDBotRightYChannel5, &subBry);
  sIWidth = subBrx - subTlx + 1;
  sIHeight = subBry - subTly + 1;
  bufSize5_ = (uint32_t)(sIWidth * sIHeight * sizeof(uint16_t));
  buffer5_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    buffer5_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyScrambledSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)buffer5_[imageIndex]);
  }
  // Allocate the storage for the reset data and 
  reset5_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    reset5_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyResetDataSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)reset5_[imageIndex]);
  }

  // Read the sub image for sub frame 6
  getIntegerParam(GDTopLeftXChannel6, &subTlx);
  getIntegerParam(GDTopLeftYChannel6, &subTly);
  getIntegerParam(GDBotRightXChannel6, &subBrx);
  getIntegerParam(GDBotRightYChannel6, &subBry);
  sIWidth = subBrx - subTlx + 1;
  sIHeight = subBry - subTly + 1;
  bufSize6_ = (uint32_t)(sIWidth * sIHeight * sizeof(uint16_t));
  buffer6_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    buffer6_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyScrambledSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)buffer6_[imageIndex]);
  }
  // Allocate the storage for the reset data and 
  reset6_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    reset6_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyResetDataSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)reset6_[imageIndex]);
  }

  // Read the sub image for sub frame 7
  getIntegerParam(GDTopLeftXChannel7, &subTlx);
  getIntegerParam(GDTopLeftYChannel7, &subTly);
  getIntegerParam(GDBotRightXChannel7, &subBrx);
  getIntegerParam(GDBotRightYChannel7, &subBry);
  sIWidth = subBrx - subTlx + 1;
  sIHeight = subBry - subTly + 1;
  bufSize7_ = (uint32_t)(sIWidth * sIHeight * sizeof(uint16_t));
  buffer7_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    buffer7_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyScrambledSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)buffer7_[imageIndex]);
  }
  // Allocate the storage for the reset data and 
  reset7_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    reset7_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyResetDataSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)reset7_[imageIndex]);
  }

  // Read the sub image for sub frame 8
  getIntegerParam(GDTopLeftXChannel8, &subTlx);
  getIntegerParam(GDTopLeftYChannel8, &subTly);
  getIntegerParam(GDBotRightXChannel8, &subBrx);
  getIntegerParam(GDBotRightYChannel8, &subBry);
  sIWidth = subBrx - subTlx + 1;
  sIHeight = subBry - subTly + 1;
  bufSize8_ = (uint32_t)(sIWidth * sIHeight * sizeof(uint16_t));
  buffer8_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    buffer8_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyScrambledSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)buffer8_[imageIndex]);
  }
  // Allocate the storage for the reset data and 
  reset8_ = (void **)malloc(images * sizeof(void *));
  for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
    reset8_[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
    configPtr->copyResetDataSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)reset8_[imageIndex]);
  }

}


void asynGeneratorDriver::posting_task(int taskNumber)
{
  epicsTimeStamp startTime;
//  epicsTimeStamp frameTime;
  epicsTimeStamp timeNow;
  int status;
  int debugLevel;
  uint32_t prevDebug = 0;
  uint32_t images = 0;
  int imageDataType = 0;
  int numImages;
  int imageMode;
  int mode;
  int postAttribute;         // Address of attribute used for starting/stopping posting
  int posting;               // Current posting value
  int counterAttribute;      // Address of attribute used for the current count of posted data
  int counter = 0;           // Current value of counter
  int locAddressAttribute;   // Address of attribute used for local NIC address
  char locAddress[128];      // Current value of the local NIC address
  int remAddressAttribute;   // Address of attribute used for destination NIC address
  char remAddress[128];      // Current value of the destination NIC address
  int locPortAttribute;      // Address of attribute used for local port to bind to
  int locPort;               // Current value of the local port to bind to
  int remPortAttribute;      // Address of attribute used for remote port to send to
  int remPort;               // Current value of the remote port to send to
  int errorAttribute;        // Address of attribute used for error status
  int errorMsgAttribute;     // Address of attribute used for error message
  
  // Sub image coordinates
  int subTlxAttribute;       // Address of attribute used for sub image top left x
  int subTlx;                // Current value of sub image top left x
  int subTlyAttribute;       // Address of attribute used for sub image top left y
  int subTly;                // Current value of sub image top left y
  int subBrxAttribute;       // Address of attribute used for sub image bottom right x
  int subBrx;                // Current value of sub image bottom right x
  int subBryAttribute;       // Address of attribute used for sub image bottom right y
  int subBry;                // Current value of sub image bottom right y

  // Sub frame and packet info
  int subFrameAttribute;     // Address of attribute used for sub frame count
  int subFrameNumber;        // Sub Frame number for this task
  int packetSizeAttribute;   // Address of attribute used for packet size in pixels
  int packetSize;            // Current value of packet size in pixels

  int sIWidth = 0;
  int sIHeight = 0;
  void **buffer = 0;
  void **resetBuffer = 0;

  double postTime = 0.0;     // Time in seconds of each frame post
  double frequency;          // Frames per second
  DataSender *senderPtr;     // Pointer to the data sender
  static const char *functionName = "asynGeneratorDriver::posting_task";
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s\n", functionName);

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s: Task %d starting\n", functionName, taskNumber);

  // Create the new data sender
  senderPtr = new DataSender();
  
  // Setup the task post address
  postAttribute = GDPostBase + taskNumber;
  // Setup the counter address
  counterAttribute = GDCounterBase + taskNumber;
  // Setup the local NIC address
  locAddressAttribute = GDLocAddrBase + taskNumber;
  // Setup the local port address
  locPortAttribute = GDLocPortBase + taskNumber;
  // Setup the remote NIC address
  remAddressAttribute = GDRemAddrBase + taskNumber;
  // Setup the remote port address
  remPortAttribute = GDRemPortBase + taskNumber;
  // Setup the error status address
  errorAttribute = GDErrorBase + taskNumber;
  // Setup the error message address
  errorMsgAttribute = GDErrorMsgBase + taskNumber;
  // Setup the top left X sub image address
  subTlxAttribute = GDTopLeftXBase + taskNumber;
  // Setup the top left Y sub image address
  subTlyAttribute = GDTopLeftYBase + taskNumber;
  // Setup the bottom right X sub image address
  subBrxAttribute = GDBotRightXBase + taskNumber;
  // Setup the bottom right Y sub image address
  subBryAttribute = GDBotRightYBase + taskNumber;
  // Setup the sub frame number
  subFrameAttribute = GDSubFrameBase + taskNumber;
  // Setup the packet size address
  packetSizeAttribute = GDPacketSizeBase + taskNumber;

  // Loop forever in this task
  while (1){
    // Is posting active for this thread?
    this->lock();
    getIntegerParam(postAttribute, &posting);
    this->unlock();
    // If we are not posting then wait for a semaphore that is given when acquisition is started
    if (!posting){
      // If we are in here then shut down the socket
      senderPtr->shutdownSocket();

      // If the buffer is not zero then free the memory
      if (buffer){
        for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
          free(buffer[imageIndex]);
        }
        free(buffer);
        buffer = 0;
      }

      //setIntegerParam(ADStatus, ADStatusIdle);
      //callParamCallbacks();

      // Wait for an event that says posting has started
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: waiting for acquire to start\n", driverName, functionName);

      status = epicsEventWait(this->startEventId_[taskNumber]);
      this->lock();
      // Reset the counter to zero
      setIntegerParam(counterAttribute, 0);
      // Read the image mode
      getIntegerParam(GDImageMode, &imageMode);
      // Read the generator mode
      getIntegerParam(GDMode, &mode);
      // Read the posting frequency
      getDoubleParam(GDPostFrequency, &frequency);
      // Read the local NIC address
      getStringParam(locAddressAttribute, sizeof(locAddress), locAddress);
      // Read the local port to bind to
      getIntegerParam(locPortAttribute, &locPort);
      // Read the remote NIC address
      getStringParam(remAddressAttribute, sizeof(remAddress), remAddress);
      // Read the remote port to send to
      getIntegerParam(remPortAttribute, &remPort);
      // Setup the debug level before doing any socket work
      getIntegerParam(GDDebugLevel, &debugLevel);
      // Read the sub image coordinates, top left to bottom right
      getIntegerParam(subTlxAttribute, &subTlx);
      getIntegerParam(subTlyAttribute, &subTly);
      getIntegerParam(subBrxAttribute, &subBrx);
      getIntegerParam(subBryAttribute, &subBry);
      // Read the sub frame count and packet size
      getIntegerParam(subFrameAttribute, &subFrameNumber);
      getIntegerParam(packetSizeAttribute, &packetSize);
      // Read the image data type
      getIntegerParam(DataType, &imageDataType);
      // Reset message
      setStringParam(errorMsgAttribute, "");
      // Reset status
      setIntegerParam(errorAttribute, 0);
      callParamCallbacks();
      this->unlock();
      postTime = 1.0 / frequency;
      counter = 0;

      sIWidth = subBrx - subTlx + 1;
      sIHeight = subBry - subTly + 1;
      // Allocate the storage for the image and 
      // load the sub image into the buffer
      images = configPtr->getNoOfImages();
      buffer = (void **)malloc(images * sizeof(void *));
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        if (imageDataType == UInt8){
          buffer[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint8_t));
          configPtr->copyScrambledSectionUInt8(imageIndex, subTlx, subTly, subBrx, subBry, (uint8_t *)buffer[imageIndex]);
        } else if (imageDataType == UInt16){
          buffer[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
          configPtr->copyScrambledSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)buffer[imageIndex]);
        } else if (imageDataType == UInt32){
          buffer[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint32_t));
          configPtr->copyScrambledSectionUInt32(imageIndex, subTlx, subTly, subBrx, subBry, (uint32_t *)buffer[imageIndex]);
        }
      }
      // Allocate the storage for the reset data and 
      // load the sub image into the buffer
      resetBuffer = (void **)malloc(images * sizeof(void *));
      for (uint32_t imageIndex = 0; imageIndex < images; imageIndex++){
        if (imageDataType == UInt8){
//          buffer[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint8_t));
//          configPtr->copyScrambledSectionUInt8(imageIndex, subTlx, subTly, subBrx, subBry, (uint8_t *)buffer[imageIndex]);
        } else if (imageDataType == UInt16){
          resetBuffer[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint16_t));
          configPtr->copyResetDataSectionUInt16(imageIndex, subTlx, subTly, subBrx, subBry, (uint16_t *)resetBuffer[imageIndex]);
        } else if (imageDataType == UInt32){
//          buffer[imageIndex] = malloc(sIWidth * sIHeight * sizeof(uint32_t));
//          configPtr->copyScrambledSectionUInt32(imageIndex, subTlx, subTly, subBrx, subBry, (uint32_t *)buffer[imageIndex]);
        }
      }

      // Setup the debug level before doing any socket work
      senderPtr->setDebug(debugLevel);
      prevDebug = debugLevel;
      // Setup the data sender with the attributes
      status = senderPtr->setupSocket(locAddress, locPort, remAddress, remPort);
      if (status != 0){
        this->lock();
        // Failed to setup socket, get error message and abort posting
        setStringParam(errorMsgAttribute, senderPtr->errorMessage().c_str());
        setIntegerParam(errorAttribute, 1);
        setIntegerParam(postAttribute, 0);
        posting = 0;
        callParamCallbacks();
        this->unlock();
      }
      //setIntegerParam(ADStatus, ADStatusAcquire);
      //setStringParam(ADStatusMessage, "Acquiring images");
      //callParamCallbacks();

      // We are posting, get the current time
      epicsTimeGetCurrent(&startTime);
    }

    // Post
//std::cout << "TASK " << taskNumber << " - Posting now..." << std::endl;
    this->lock();
    getIntegerParam(postAttribute, &posting);
    this->unlock();
    if (posting){
      counter++;

      if (mode == 0){
        // Calculate buffer size in bytes
        uint32_t bufSize = ((uint32_t)pow(2.0, (double)imageDataType)) * sIWidth * sIHeight;
        // Send the sub image, along with the number of sub frames and packet size
        senderPtr->sendImage(buffer[counter%images], bufSize, subFrameNumber, packetSize, counter, 0);
        // Also send the reset data for the next frame, along with the number of sub frames and packet size
        senderPtr->sendImage(resetBuffer[counter%images], bufSize, subFrameNumber, packetSize, counter+1, 1);
      } else {

        // We are in temporal mode.
        // Check our task number against the frame counter, if it is our turn then post all 8 buffers out
        // on our channel.  If it is not our turn then do nothing.
        if ((counter-1)%8 == taskNumber){
          // Send the sub image, along with the number of sub frames and packet size
          senderPtr->sendImage(buffer1_[counter%images], bufSize1_, 0, packetSize, counter, 0);
          // Send the sub image, along with the number of sub frames and packet size
          senderPtr->sendImage(buffer2_[counter%images], bufSize2_, 1, packetSize, counter, 0);
          // Send the sub image, along with the number of sub frames and packet size
          senderPtr->sendImage(buffer3_[counter%images], bufSize3_, 2, packetSize, counter, 0);
          // Send the sub image, along with the number of sub frames and packet size
          senderPtr->sendImage(buffer4_[counter%images], bufSize4_, 3, packetSize, counter, 0);
          // Send the sub image, along with the number of sub frames and packet size
          senderPtr->sendImage(buffer5_[counter%images], bufSize5_, 4, packetSize, counter, 0);
          // Send the sub image, along with the number of sub frames and packet size
          senderPtr->sendImage(buffer6_[counter%images], bufSize6_, 5, packetSize, counter, 0);
          // Send the sub image, along with the number of sub frames and packet size
          senderPtr->sendImage(buffer7_[counter%images], bufSize7_, 6, packetSize, counter, 0);
          // Send the sub image, along with the number of sub frames and packet size
          senderPtr->sendImage(buffer8_[counter%images], bufSize8_, 7, packetSize, counter, 0);
        }
        if (counter%8 == taskNumber){
          // Also send the reset data for the next frame, along with the number of sub frames and packet size
          senderPtr->sendImage(reset1_[counter%images], bufSize1_, 0, packetSize, counter+1, 1);
          // Also send the reset data for the next frame, along with the number of sub frames and packet size
          senderPtr->sendImage(reset2_[counter%images], bufSize2_, 1, packetSize, counter+1, 1);
          // Also send the reset data for the next frame, along with the number of sub frames and packet size
          senderPtr->sendImage(reset3_[counter%images], bufSize3_, 2, packetSize, counter+1, 1);
          // Also send the reset data for the next frame, along with the number of sub frames and packet size
          senderPtr->sendImage(reset4_[counter%images], bufSize4_, 3, packetSize, counter+1, 1);
          // Also send the reset data for the next frame, along with the number of sub frames and packet size
          senderPtr->sendImage(reset5_[counter%images], bufSize5_, 4, packetSize, counter+1, 1);
          // Also send the reset data for the next frame, along with the number of sub frames and packet size
          senderPtr->sendImage(reset6_[counter%images], bufSize6_, 5, packetSize, counter+1, 1);
          // Also send the reset data for the next frame, along with the number of sub frames and packet size
          senderPtr->sendImage(reset7_[counter%images], bufSize7_, 6, packetSize, counter+1, 1);
          // Also send the reset data for the next frame, along with the number of sub frames and packet size
          senderPtr->sendImage(reset8_[counter%images], bufSize8_, 7, packetSize, counter+1, 1);
        }
      }

      // Call the callbacks to update any changes
      this->lock();
      // Check for a change in debug level
      getIntegerParam(GDDebugLevel, &debugLevel);
      if ((uint32_t)debugLevel != prevDebug){
        senderPtr->setDebug(debugLevel);
        prevDebug = debugLevel;
      }
      setIntegerParam(counterAttribute, counter);
      callParamCallbacks();
      //getIntegerParam(postAttribute, &posting);

      // Read in the number of Images and counter
      getIntegerParam(GDNumImages, &numImages);
      // See if acquisition is complete
      if (((imageMode == 0) && (counter == 1)) ||
          ((imageMode == 1) && (counter >= numImages))) {
        setIntegerParam(postAttribute, 0);
        setIntegerParam(GDPostCommand, 0);
        posting = 0;
        callParamCallbacks();
      }


      this->unlock();
      if (posting){
        epicsTimeGetCurrent(&timeNow);
        double wait = counter*postTime - epicsTimeDiffInSeconds(&timeNow, &startTime);
        status = epicsEventWaitWithTimeout(this->stopEventId_[taskNumber], wait);
      }
    }
  }
}

/** This is the constructor for the asynGeneratorDriver class.
  * portName, maxAddr, paramTableSize, interfaceMask, interruptMask, asynFlags, autoConnect, priority and stackSize
  * are simply passed to asynPortDriver::asynPortDriver.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] maxAddr The maximum  number of asyn addr addresses this driver supports. 1 is minimum.
  * \param[in] interfaceMask Bit mask defining the asyn interfaces that this driver supports.
  * \param[in] interruptMask Bit mask definining the asyn interfaces that can generate interrupts (callbacks)
  * \param[in] asynFlags Flags when creating the asyn port driver; includes ASYN_CANBLOCK and ASYN_MULTIDEVICE.
  * \param[in] autoConnect The autoConnect flag for the asyn port driver.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
asynGeneratorDriver::asynGeneratorDriver(const char *portName,
                                               int maxAddr,
                                               int interfaceMask,
                                               int interruptMask,
                                               int asynFlags,
                                               int autoConnect,
                                               int priority,
                                               int stackSize)
    : asynPortDriver(portName,
                     maxAddr,
                     NUM_GENERATOR_PARAMS,
                     interfaceMask,
                     interruptMask,
                     asynFlags,
                     autoConnect,
                     priority,
                     stackSize)
{
  int status;
  static const char *functionName = "asynGeneratorDriver::asynGeneratorDriver";
  createParam(PortNameSelfString,         asynParamOctet,           &PortNameSelf);
  createParam(ImageSizeXString,           asynParamInt32,           &ImageSizeX);
  createParam(ImageSizeYString,           asynParamInt32,           &ImageSizeY);
  createParam(ImagePatternXString,        asynParamInt32,           &ImagePatternX);
  createParam(ImagePatternYString,        asynParamInt32,           &ImagePatternY);
  createParam(ImagePatternTypeString,     asynParamInt32,           &ImagePatternType);
  createParam(ImageSizeZString,           asynParamInt32,           &ImageSizeZ);
  createParam(ImageSizeString,            asynParamInt32,           &ImageSize);
  createParam(NDimensionsString,          asynParamInt32,           &NDimensions);
  createParam(DimensionsString,           asynParamInt32,           &Dimensions);
  createParam(DataTypeString,             asynParamInt32,           &DataType);
  createParam(FilePathString,             asynParamOctet,           &FilePath);
  createParam(FilePathExistsString,       asynParamInt32,           &FilePathExists);
  createParam(FileNameString,             asynParamOctet,           &FileName);
  createParam(FullFileNameString,         asynParamOctet,           &FullFileName);
  createParam(RawFileReadString,          asynParamInt32,           &RawFileRead);
  createParam(FileReadStatusString,       asynParamInt32,           &FileReadStatus);
  createParam(FileReadMessageString,      asynParamOctet,           &FileReadMessage);
  createParam(FileErrorStatusString,      asynParamInt32,           &FileErrorStatus);
  createParam(DPixelsPerChipXString,      asynParamInt32,           &DPixelsPerChipX);
  createParam(DPixelsPerChipYString,      asynParamInt32,           &DPixelsPerChipY);
  createParam(DChipsPerBlockXString,      asynParamInt32,           &DChipsPerBlockX);
  createParam(DChipsPerBlockYString,      asynParamInt32,           &DChipsPerBlockY);
  createParam(DBlocksPerStripeXString,    asynParamInt32,           &DBlocksPerStripeX);
  createParam(DBlocksPerStripeYString,    asynParamInt32,           &DBlocksPerStripeY);
  createParam(DChipsPerStripeXString,     asynParamInt32,           &DChipsPerStripeX);
  createParam(DChipsPerStripeYString,     asynParamInt32,           &DChipsPerStripeY);
  createParam(DStripesPerImageXString,    asynParamInt32,           &DStripesPerImageX);
  createParam(DStripesPerImageYString,    asynParamInt32,           &DStripesPerImageY);
  createParam(ImageScrambleTypeString,    asynParamInt32,           &ImageScrambleType);

  createParam(GDDebugLevelString,         asynParamInt32,           &GDDebugLevel);

  createParam(GDNumImagesString,          asynParamInt32,           &GDNumImages);
  createParam(GDNumImagesCounterString,   asynParamInt32,           &GDNumImagesCounter);
  createParam(GDImageModeString,          asynParamInt32,           &GDImageMode);
  createParam(GDModeString,               asynParamInt32,           &GDMode);

  // Parameters for thread control
  createParam(GDPostCommandString,        asynParamInt32,           &GDPostCommand);
  createParam(GDPostChannel1String,       asynParamInt32,           &GDPostChannel1);
  createParam(GDPostChannel2String,       asynParamInt32,           &GDPostChannel2);
  createParam(GDPostChannel3String,       asynParamInt32,           &GDPostChannel3);
  createParam(GDPostChannel4String,       asynParamInt32,           &GDPostChannel4);
  createParam(GDPostChannel5String,       asynParamInt32,           &GDPostChannel5);
  createParam(GDPostChannel6String,       asynParamInt32,           &GDPostChannel6);
  createParam(GDPostChannel7String,       asynParamInt32,           &GDPostChannel7);
  createParam(GDPostChannel8String,       asynParamInt32,           &GDPostChannel8);
  GDPostBase = GDPostChannel1;
  createParam(GDCounterChannel1String,    asynParamInt32,           &GDCounterChannel1);
  createParam(GDCounterChannel2String,    asynParamInt32,           &GDCounterChannel2);
  createParam(GDCounterChannel3String,    asynParamInt32,           &GDCounterChannel3);
  createParam(GDCounterChannel4String,    asynParamInt32,           &GDCounterChannel4);
  createParam(GDCounterChannel5String,    asynParamInt32,           &GDCounterChannel5);
  createParam(GDCounterChannel6String,    asynParamInt32,           &GDCounterChannel6);
  createParam(GDCounterChannel7String,    asynParamInt32,           &GDCounterChannel7);
  createParam(GDCounterChannel8String,    asynParamInt32,           &GDCounterChannel8);
  GDCounterBase = GDCounterChannel1;
  createParam(GDEnableChannel1String,     asynParamInt32,           &GDEnableChannel1);
  createParam(GDEnableChannel2String,     asynParamInt32,           &GDEnableChannel2);
  createParam(GDEnableChannel3String,     asynParamInt32,           &GDEnableChannel3);
  createParam(GDEnableChannel4String,     asynParamInt32,           &GDEnableChannel4);
  createParam(GDEnableChannel5String,     asynParamInt32,           &GDEnableChannel5);
  createParam(GDEnableChannel6String,     asynParamInt32,           &GDEnableChannel6);
  createParam(GDEnableChannel7String,     asynParamInt32,           &GDEnableChannel7);
  createParam(GDEnableChannel8String,     asynParamInt32,           &GDEnableChannel8);
  createParam(GDPostFrequencyString,      asynParamFloat64,         &GDPostFrequency);

  // Parameters to define the frames to send
  createParam(GDTopLeftXChannel1String,   asynParamInt32,           &GDTopLeftXChannel1);
  createParam(GDTopLeftXChannel2String,   asynParamInt32,           &GDTopLeftXChannel2);
  createParam(GDTopLeftXChannel3String,   asynParamInt32,           &GDTopLeftXChannel3);
  createParam(GDTopLeftXChannel4String,   asynParamInt32,           &GDTopLeftXChannel4);
  createParam(GDTopLeftXChannel5String,   asynParamInt32,           &GDTopLeftXChannel5);
  createParam(GDTopLeftXChannel6String,   asynParamInt32,           &GDTopLeftXChannel6);
  createParam(GDTopLeftXChannel7String,   asynParamInt32,           &GDTopLeftXChannel7);
  createParam(GDTopLeftXChannel8String,   asynParamInt32,           &GDTopLeftXChannel8);
  GDTopLeftXBase = GDTopLeftXChannel1;
  createParam(GDTopLeftYChannel1String,   asynParamInt32,           &GDTopLeftYChannel1);
  createParam(GDTopLeftYChannel2String,   asynParamInt32,           &GDTopLeftYChannel2);
  createParam(GDTopLeftYChannel3String,   asynParamInt32,           &GDTopLeftYChannel3);
  createParam(GDTopLeftYChannel4String,   asynParamInt32,           &GDTopLeftYChannel4);
  createParam(GDTopLeftYChannel5String,   asynParamInt32,           &GDTopLeftYChannel5);
  createParam(GDTopLeftYChannel6String,   asynParamInt32,           &GDTopLeftYChannel6);
  createParam(GDTopLeftYChannel7String,   asynParamInt32,           &GDTopLeftYChannel7);
  createParam(GDTopLeftYChannel8String,   asynParamInt32,           &GDTopLeftYChannel8);
  GDTopLeftYBase = GDTopLeftYChannel1;
  createParam(GDBotRightXChannel1String,  asynParamInt32,           &GDBotRightXChannel1);
  createParam(GDBotRightXChannel2String,  asynParamInt32,           &GDBotRightXChannel2);
  createParam(GDBotRightXChannel3String,  asynParamInt32,           &GDBotRightXChannel3);
  createParam(GDBotRightXChannel4String,  asynParamInt32,           &GDBotRightXChannel4);
  createParam(GDBotRightXChannel5String,  asynParamInt32,           &GDBotRightXChannel5);
  createParam(GDBotRightXChannel6String,  asynParamInt32,           &GDBotRightXChannel6);
  createParam(GDBotRightXChannel7String,  asynParamInt32,           &GDBotRightXChannel7);
  createParam(GDBotRightXChannel8String,  asynParamInt32,           &GDBotRightXChannel8);
  GDBotRightXBase = GDBotRightXChannel1;
  createParam(GDBotRightYChannel1String,  asynParamInt32,           &GDBotRightYChannel1);
  createParam(GDBotRightYChannel2String,  asynParamInt32,           &GDBotRightYChannel2);
  createParam(GDBotRightYChannel3String,  asynParamInt32,           &GDBotRightYChannel3);
  createParam(GDBotRightYChannel4String,  asynParamInt32,           &GDBotRightYChannel4);
  createParam(GDBotRightYChannel5String,  asynParamInt32,           &GDBotRightYChannel5);
  createParam(GDBotRightYChannel6String,  asynParamInt32,           &GDBotRightYChannel6);
  createParam(GDBotRightYChannel7String,  asynParamInt32,           &GDBotRightYChannel7);
  createParam(GDBotRightYChannel8String,  asynParamInt32,           &GDBotRightYChannel8);
  GDBotRightYBase = GDBotRightYChannel1;
  createParam(GDWidthChannel1String,      asynParamInt32,           &GDWidthChannel1);
  createParam(GDWidthChannel2String,      asynParamInt32,           &GDWidthChannel2);
  createParam(GDWidthChannel3String,      asynParamInt32,           &GDWidthChannel3);
  createParam(GDWidthChannel4String,      asynParamInt32,           &GDWidthChannel4);
  createParam(GDWidthChannel5String,      asynParamInt32,           &GDWidthChannel5);
  createParam(GDWidthChannel6String,      asynParamInt32,           &GDWidthChannel6);
  createParam(GDWidthChannel7String,      asynParamInt32,           &GDWidthChannel7);
  createParam(GDWidthChannel8String,      asynParamInt32,           &GDWidthChannel8);
  GDWidthBase = GDWidthChannel1;
  createParam(GDHeightChannel1String,     asynParamInt32,           &GDHeightChannel1);
  createParam(GDHeightChannel2String,     asynParamInt32,           &GDHeightChannel2);
  createParam(GDHeightChannel3String,     asynParamInt32,           &GDHeightChannel3);
  createParam(GDHeightChannel4String,     asynParamInt32,           &GDHeightChannel4);
  createParam(GDHeightChannel5String,     asynParamInt32,           &GDHeightChannel5);
  createParam(GDHeightChannel6String,     asynParamInt32,           &GDHeightChannel6);
  createParam(GDHeightChannel7String,     asynParamInt32,           &GDHeightChannel7);
  createParam(GDHeightChannel8String,     asynParamInt32,           &GDHeightChannel8);
  GDHeightBase = GDHeightChannel1;
  createParam(GDSubFrameChannel1String,   asynParamInt32,           &GDSubFrameChannel1);
  createParam(GDSubFrameChannel2String,   asynParamInt32,           &GDSubFrameChannel2);
  createParam(GDSubFrameChannel3String,   asynParamInt32,           &GDSubFrameChannel3);
  createParam(GDSubFrameChannel4String,   asynParamInt32,           &GDSubFrameChannel4);
  createParam(GDSubFrameChannel5String,   asynParamInt32,           &GDSubFrameChannel5);
  createParam(GDSubFrameChannel6String,   asynParamInt32,           &GDSubFrameChannel6);
  createParam(GDSubFrameChannel7String,   asynParamInt32,           &GDSubFrameChannel7);
  createParam(GDSubFrameChannel8String,   asynParamInt32,           &GDSubFrameChannel8);
  GDSubFrameBase = GDSubFrameChannel1;
  createParam(GDPacketSizeChannel1String, asynParamInt32,           &GDPacketSizeChannel1);
  createParam(GDPacketSizeChannel2String, asynParamInt32,           &GDPacketSizeChannel2);
  createParam(GDPacketSizeChannel3String, asynParamInt32,           &GDPacketSizeChannel3);
  createParam(GDPacketSizeChannel4String, asynParamInt32,           &GDPacketSizeChannel4);
  createParam(GDPacketSizeChannel5String, asynParamInt32,           &GDPacketSizeChannel5);
  createParam(GDPacketSizeChannel6String, asynParamInt32,           &GDPacketSizeChannel6);
  createParam(GDPacketSizeChannel7String, asynParamInt32,           &GDPacketSizeChannel7);
  createParam(GDPacketSizeChannel8String, asynParamInt32,           &GDPacketSizeChannel8);
  GDPacketSizeBase = GDPacketSizeChannel1;

  // Parameters for UDP configuration
  createParam(GDLocAddrChannel1String,    asynParamOctet,           &GDLocAddrChannel1);
  createParam(GDLocAddrChannel2String,    asynParamOctet,           &GDLocAddrChannel2);
  createParam(GDLocAddrChannel3String,    asynParamOctet,           &GDLocAddrChannel3);
  createParam(GDLocAddrChannel4String,    asynParamOctet,           &GDLocAddrChannel4);
  createParam(GDLocAddrChannel5String,    asynParamOctet,           &GDLocAddrChannel5);
  createParam(GDLocAddrChannel6String,    asynParamOctet,           &GDLocAddrChannel6);
  createParam(GDLocAddrChannel7String,    asynParamOctet,           &GDLocAddrChannel7);
  createParam(GDLocAddrChannel8String,    asynParamOctet,           &GDLocAddrChannel8);
  GDLocAddrBase = GDLocAddrChannel1;
  createParam(GDLocPortChannel1String,    asynParamInt32,           &GDLocPortChannel1);
  createParam(GDLocPortChannel2String,    asynParamInt32,           &GDLocPortChannel2);
  createParam(GDLocPortChannel3String,    asynParamInt32,           &GDLocPortChannel3);
  createParam(GDLocPortChannel4String,    asynParamInt32,           &GDLocPortChannel4);
  createParam(GDLocPortChannel5String,    asynParamInt32,           &GDLocPortChannel5);
  createParam(GDLocPortChannel6String,    asynParamInt32,           &GDLocPortChannel6);
  createParam(GDLocPortChannel7String,    asynParamInt32,           &GDLocPortChannel7);
  createParam(GDLocPortChannel8String,    asynParamInt32,           &GDLocPortChannel8);
  GDLocPortBase = GDLocPortChannel1;
  createParam(GDRemAddrChannel1String,    asynParamOctet,           &GDRemAddrChannel1);
  createParam(GDRemAddrChannel2String,    asynParamOctet,           &GDRemAddrChannel2);
  createParam(GDRemAddrChannel3String,    asynParamOctet,           &GDRemAddrChannel3);
  createParam(GDRemAddrChannel4String,    asynParamOctet,           &GDRemAddrChannel4);
  createParam(GDRemAddrChannel5String,    asynParamOctet,           &GDRemAddrChannel5);
  createParam(GDRemAddrChannel6String,    asynParamOctet,           &GDRemAddrChannel6);
  createParam(GDRemAddrChannel7String,    asynParamOctet,           &GDRemAddrChannel7);
  createParam(GDRemAddrChannel8String,    asynParamOctet,           &GDRemAddrChannel8);
  GDRemAddrBase = GDRemAddrChannel1;
  createParam(GDRemPortChannel1String,    asynParamInt32,           &GDRemPortChannel1);
  createParam(GDRemPortChannel2String,    asynParamInt32,           &GDRemPortChannel2);
  createParam(GDRemPortChannel3String,    asynParamInt32,           &GDRemPortChannel3);
  createParam(GDRemPortChannel4String,    asynParamInt32,           &GDRemPortChannel4);
  createParam(GDRemPortChannel5String,    asynParamInt32,           &GDRemPortChannel5);
  createParam(GDRemPortChannel6String,    asynParamInt32,           &GDRemPortChannel6);
  createParam(GDRemPortChannel7String,    asynParamInt32,           &GDRemPortChannel7);
  createParam(GDRemPortChannel8String,    asynParamInt32,           &GDRemPortChannel8);
  GDRemPortBase = GDRemPortChannel1;
  
  // Channel error parameters
  createParam(GDErrorChannel1String,      asynParamInt32,           &GDErrorChannel1);
  createParam(GDErrorChannel2String,      asynParamInt32,           &GDErrorChannel2);
  createParam(GDErrorChannel3String,      asynParamInt32,           &GDErrorChannel3);
  createParam(GDErrorChannel4String,      asynParamInt32,           &GDErrorChannel4);
  createParam(GDErrorChannel5String,      asynParamInt32,           &GDErrorChannel5);
  createParam(GDErrorChannel6String,      asynParamInt32,           &GDErrorChannel6);
  createParam(GDErrorChannel7String,      asynParamInt32,           &GDErrorChannel7);
  createParam(GDErrorChannel8String,      asynParamInt32,           &GDErrorChannel8);
  GDErrorBase = GDErrorChannel1;
  createParam(GDErrorMsgChannel1String,   asynParamOctet,           &GDErrorMsgChannel1);
  createParam(GDErrorMsgChannel2String,   asynParamOctet,           &GDErrorMsgChannel2);
  createParam(GDErrorMsgChannel3String,   asynParamOctet,           &GDErrorMsgChannel3);
  createParam(GDErrorMsgChannel4String,   asynParamOctet,           &GDErrorMsgChannel4);
  createParam(GDErrorMsgChannel5String,   asynParamOctet,           &GDErrorMsgChannel5);
  createParam(GDErrorMsgChannel6String,   asynParamOctet,           &GDErrorMsgChannel6);
  createParam(GDErrorMsgChannel7String,   asynParamOctet,           &GDErrorMsgChannel7);
  createParam(GDErrorMsgChannel8String,   asynParamOctet,           &GDErrorMsgChannel8);
  GDErrorMsgBase = GDErrorMsgChannel1;

  // Create the Configurator object
  configPtr = new Configurator();
  // Initialise all values to zero
  configPtr->setRepeatX(0);
  configPtr->setRepeatY(0);
  configPtr->setPixelsPerChipX(0);
  configPtr->setPixelsPerChipY(0);
  configPtr->setChipsPerBlockX(0);
  configPtr->setChipsPerBlockY(0);
  configPtr->setBlocksPerStripeX(0);
  configPtr->setBlocksPerStripeY(0);
  configPtr->setStripesPerImageX(0);
  configPtr->setStripesPerImageY(0);


  /* Here we set the values of read-only parameters and of read/write parameters that cannot
   * or should not get their values from the database.  Note that values set here will override
   * those in the database for output records because if asyn device support reads a value from 
   * the driver with no error during initialization then it sets the output record to that value.  
   * If a value is not set here then the read request will return an error (uninitialized).
   * Values set here will be overridden by values from save/restore if they exist. */
  setStringParam (PortNameSelf,         portName);
  setIntegerParam(ImageSizeX,           configPtr->getImageWidth());
  setIntegerParam(ImageSizeY,           configPtr->getImageHeight());
  setIntegerParam(ImagePatternX,        configPtr->getRepeatX());
  setIntegerParam(ImagePatternY,        configPtr->getRepeatY());
  setIntegerParam(ImageSizeZ,           0);
  setIntegerParam(ImageSize,            0);
  setIntegerParam(NDimensions,          3);
  setIntegerParam(DataType,             UInt8);
  setIntegerParam(FileReadStatus,       0);
  setStringParam (FilePath,             "");
  setStringParam (FileName,             "");
  setStringParam (FullFileName,         "");
  setStringParam (FileReadMessage,      "");
  setIntegerParam(FileErrorStatus,      0);
  setIntegerParam(DPixelsPerChipX,      configPtr->getPixelsPerChipX());
  setIntegerParam(DPixelsPerChipY,      configPtr->getPixelsPerChipY());
  setIntegerParam(DChipsPerBlockX,      configPtr->getChipsPerBlockX());
  setIntegerParam(DChipsPerBlockY,      configPtr->getChipsPerBlockY());
  setIntegerParam(DBlocksPerStripeX,    configPtr->getBlocksPerStripeX());
  setIntegerParam(DBlocksPerStripeY,    configPtr->getBlocksPerStripeY());
  setIntegerParam(DChipsPerStripeX,     configPtr->getChipsPerStripeX());
  setIntegerParam(DChipsPerStripeY,     configPtr->getChipsPerStripeY());
  setIntegerParam(DStripesPerImageX,    configPtr->getStripesPerImageX());
  setIntegerParam(DStripesPerImageY,    configPtr->getStripesPerImageY());

  setIntegerParam(GDDebugLevel,         0);

  setIntegerParam(GDNumImages,          100);
  setIntegerParam(GDNumImagesCounter,   0);
  setIntegerParam(GDImageMode,          2);
  setIntegerParam(GDMode,               0);

  setIntegerParam(GDPostCommand,        0);
  setIntegerParam(GDPostChannel1,       0);
  setIntegerParam(GDPostChannel2,       0);
  setIntegerParam(GDPostChannel3,       0);
  setIntegerParam(GDPostChannel4,       0);
  setIntegerParam(GDPostChannel5,       0);
  setIntegerParam(GDPostChannel6,       0);
  setIntegerParam(GDPostChannel7,       0);
  setIntegerParam(GDPostChannel8,       0);
  setIntegerParam(GDCounterChannel1,    0);
  setIntegerParam(GDCounterChannel2,    0);
  setIntegerParam(GDCounterChannel3,    0);
  setIntegerParam(GDCounterChannel4,    0);
  setIntegerParam(GDCounterChannel5,    0);
  setIntegerParam(GDCounterChannel6,    0);
  setIntegerParam(GDCounterChannel7,    0);
  setIntegerParam(GDCounterChannel8,    0);
  setIntegerParam(GDEnableChannel1,     0);
  setIntegerParam(GDEnableChannel2,     0);
  setIntegerParam(GDEnableChannel3,     0);
  setIntegerParam(GDEnableChannel4,     0);
  setIntegerParam(GDEnableChannel5,     0);
  setIntegerParam(GDEnableChannel6,     0);
  setIntegerParam(GDEnableChannel7,     0);
  setIntegerParam(GDEnableChannel8,     0);
  setDoubleParam (GDPostFrequency,      1.0);

  setIntegerParam(GDTopLeftXChannel1,   0);
  setIntegerParam(GDTopLeftXChannel2,   0);
  setIntegerParam(GDTopLeftXChannel3,   0);
  setIntegerParam(GDTopLeftXChannel4,   0);
  setIntegerParam(GDTopLeftXChannel5,   0);
  setIntegerParam(GDTopLeftXChannel6,   0);
  setIntegerParam(GDTopLeftXChannel7,   0);
  setIntegerParam(GDTopLeftXChannel8,   0);
  setIntegerParam(GDTopLeftYChannel1,   0);
  setIntegerParam(GDTopLeftYChannel2,   0);
  setIntegerParam(GDTopLeftYChannel3,   0);
  setIntegerParam(GDTopLeftYChannel4,   0);
  setIntegerParam(GDTopLeftYChannel5,   0);
  setIntegerParam(GDTopLeftYChannel6,   0);
  setIntegerParam(GDTopLeftYChannel7,   0);
  setIntegerParam(GDTopLeftYChannel8,   0);
  setIntegerParam(GDBotRightXChannel1,  0);
  setIntegerParam(GDBotRightXChannel2,  0);
  setIntegerParam(GDBotRightXChannel3,  0);
  setIntegerParam(GDBotRightXChannel4,  0);
  setIntegerParam(GDBotRightXChannel5,  0);
  setIntegerParam(GDBotRightXChannel6,  0);
  setIntegerParam(GDBotRightXChannel7,  0);
  setIntegerParam(GDBotRightXChannel8,  0);
  setIntegerParam(GDBotRightYChannel1,  0);
  setIntegerParam(GDBotRightYChannel2,  0);
  setIntegerParam(GDBotRightYChannel3,  0);
  setIntegerParam(GDBotRightYChannel4,  0);
  setIntegerParam(GDBotRightYChannel5,  0);
  setIntegerParam(GDBotRightYChannel6,  0);
  setIntegerParam(GDBotRightYChannel7,  0);
  setIntegerParam(GDBotRightYChannel8,  0);

  setIntegerParam(GDWidthChannel1,      0);
  setIntegerParam(GDWidthChannel2,      0);
  setIntegerParam(GDWidthChannel3,      0);
  setIntegerParam(GDWidthChannel4,      0);
  setIntegerParam(GDWidthChannel5,      0);
  setIntegerParam(GDWidthChannel6,      0);
  setIntegerParam(GDWidthChannel7,      0);
  setIntegerParam(GDWidthChannel8,      0);
  setIntegerParam(GDHeightChannel1,     0);
  setIntegerParam(GDHeightChannel2,     0);
  setIntegerParam(GDHeightChannel3,     0);
  setIntegerParam(GDHeightChannel4,     0);
  setIntegerParam(GDHeightChannel5,     0);
  setIntegerParam(GDHeightChannel6,     0);
  setIntegerParam(GDHeightChannel7,     0);
  setIntegerParam(GDHeightChannel8,     0);

  setIntegerParam(GDSubFrameChannel1,   0);
  setIntegerParam(GDSubFrameChannel2,   0);
  setIntegerParam(GDSubFrameChannel3,   0);
  setIntegerParam(GDSubFrameChannel4,   0);
  setIntegerParam(GDSubFrameChannel5,   0);
  setIntegerParam(GDSubFrameChannel6,   0);
  setIntegerParam(GDSubFrameChannel7,   0);
  setIntegerParam(GDSubFrameChannel8,   0);
  setIntegerParam(GDPacketSizeChannel1, 0);
  setIntegerParam(GDPacketSizeChannel2, 0);
  setIntegerParam(GDPacketSizeChannel3, 0);
  setIntegerParam(GDPacketSizeChannel4, 0);
  setIntegerParam(GDPacketSizeChannel5, 0);
  setIntegerParam(GDPacketSizeChannel6, 0);
  setIntegerParam(GDPacketSizeChannel7, 0);
  setIntegerParam(GDPacketSizeChannel8, 0);

  setStringParam (GDLocAddrChannel1,    "");
  setStringParam (GDLocAddrChannel2,    "");
  setStringParam (GDLocAddrChannel3,    "");
  setStringParam (GDLocAddrChannel4,    "");
  setStringParam (GDLocAddrChannel5,    "");
  setStringParam (GDLocAddrChannel6,    "");
  setStringParam (GDLocAddrChannel7,    "");
  setStringParam (GDLocAddrChannel8,    "");
  setIntegerParam(GDLocPortChannel1,    0);
  setIntegerParam(GDLocPortChannel2,    0);
  setIntegerParam(GDLocPortChannel3,    0);
  setIntegerParam(GDLocPortChannel4,    0);
  setIntegerParam(GDLocPortChannel5,    0);
  setIntegerParam(GDLocPortChannel6,    0);
  setIntegerParam(GDLocPortChannel7,    0);
  setIntegerParam(GDLocPortChannel8,    0);
  setStringParam (GDRemAddrChannel1,    "");
  setStringParam (GDRemAddrChannel2,    "");
  setStringParam (GDRemAddrChannel3,    "");
  setStringParam (GDRemAddrChannel4,    "");
  setStringParam (GDRemAddrChannel5,    "");
  setStringParam (GDRemAddrChannel6,    "");
  setStringParam (GDRemAddrChannel7,    "");
  setStringParam (GDRemAddrChannel8,    "");
  setIntegerParam(GDRemPortChannel1,    0);
  setIntegerParam(GDRemPortChannel2,    0);
  setIntegerParam(GDRemPortChannel3,    0);
  setIntegerParam(GDRemPortChannel4,    0);
  setIntegerParam(GDRemPortChannel5,    0);
  setIntegerParam(GDRemPortChannel6,    0);
  setIntegerParam(GDRemPortChannel7,    0);
  setIntegerParam(GDRemPortChannel8,    0);

  setIntegerParam(GDErrorChannel1,      0);
  setIntegerParam(GDErrorChannel2,      0);
  setIntegerParam(GDErrorChannel3,      0);
  setIntegerParam(GDErrorChannel4,      0);
  setIntegerParam(GDErrorChannel5,      0);
  setIntegerParam(GDErrorChannel6,      0);
  setIntegerParam(GDErrorChannel7,      0);
  setIntegerParam(GDErrorChannel8,      0);
  setStringParam (GDErrorMsgChannel1,   "");
  setStringParam (GDErrorMsgChannel2,   "");
  setStringParam (GDErrorMsgChannel3,   "");
  setStringParam (GDErrorMsgChannel4,   "");
  setStringParam (GDErrorMsgChannel5,   "");
  setStringParam (GDErrorMsgChannel6,   "");
  setStringParam (GDErrorMsgChannel7,   "");
  setStringParam (GDErrorMsgChannel8,   "");

  this->startEventId_[0] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[1] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[2] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[3] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[4] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[5] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[6] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[7] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[0] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[1] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[2] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[3] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[4] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[5] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[6] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[7] = epicsEventCreate(epicsEventEmpty);

  buffer1_  = 0;
  buffer2_  = 0;
  buffer3_  = 0;
  buffer4_  = 0;
  buffer5_  = 0;
  buffer6_  = 0;
  buffer7_  = 0;
  buffer8_  = 0;
  reset1_   = 0;
  reset2_   = 0;
  reset3_   = 0;
  reset4_   = 0;
  reset5_   = 0;
  reset6_   = 0;
  reset7_   = 0;
  reset8_   = 0;
  bufSize1_ = 0;
  bufSize2_ = 0;
  bufSize3_ = 0;
  bufSize4_ = 0;
  bufSize5_ = 0;
  bufSize6_ = 0;
  bufSize7_ = 0;
  bufSize8_ = 0;
  noOfImages_ = 0;

  // Start each channel in a separate thread
	status = (epicsThreadCreate("post_task_1",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)post_task_1_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsThreadCreate failure for channel 1 task\n", driverName, functionName);
    return;
  }

	status = (epicsThreadCreate("post_task_2",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)post_task_2_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsThreadCreate failure for channel 2 task\n", driverName, functionName);
    return;
  }

	status = (epicsThreadCreate("post_task_3",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)post_task_3_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsThreadCreate failure for channel 3 task\n", driverName, functionName);
    return;
  }

	status = (epicsThreadCreate("post_task_4",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)post_task_4_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsThreadCreate failure for channel 4 task\n", driverName, functionName);
    return;
  }

	status = (epicsThreadCreate("post_task_5",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)post_task_5_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsThreadCreate failure for channel 5 task\n", driverName, functionName);
    return;
  }

	status = (epicsThreadCreate("post_task_6",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)post_task_6_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsThreadCreate failure for channel 6 task\n", driverName, functionName);
    return;
  }

	status = (epicsThreadCreate("post_task_7",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)post_task_7_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsThreadCreate failure for channel 7 task\n", driverName, functionName);
    return;
  }

	status = (epicsThreadCreate("post_task_8",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)post_task_8_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsThreadCreate failure for channel 8 task\n", driverName, functionName);
    return;
  }

}

// Configuration command
extern "C" int asynGeneratorDriverConfigure(const char *portName)
{
    new asynGeneratorDriver(portName,
                               1,
                               asynDrvUserMask | asynInt32Mask | asynInt32ArrayMask | asynOctetMask | asynFloat64Mask,
                               asynInt32Mask | asynInt32ArrayMask | asynOctetMask | asynFloat64Mask,
                               0,
                               1,
                               0,
                               104857600);
    return(asynSuccess);
}

/* EPICS iocsh shell commands */
static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg * const initArgs[] = {&initArg0};
static const iocshFuncDef initFuncDef = {"GeneratorInit",1,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
  asynGeneratorDriverConfigure(args[0].sval);
}

extern "C" void asynGeneratorDriverRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
epicsExportRegistrar(asynGeneratorDriverRegister);
}


