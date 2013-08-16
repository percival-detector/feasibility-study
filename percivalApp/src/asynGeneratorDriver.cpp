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

#include "asynGeneratorDriver.h"

static const char *driverName = "asynGeneratorDriver";


/*
 * These four routines start each of the four posting
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
    setIntegerParam(FileWriteStatus, 1);
    setStringParam (FileWriteMessage, "Reading configuration file");
    callParamCallbacks();
    status |= getStringParam(FullFileName, sizeof(fileName), fileName);
    configPtr->readConfiguration(fileName);
    setStringParam(FileWriteMessage, "Read configuration complete");
    setIntegerParam(FileWriteStatus, 0);
    setIntegerParam(ImageSizeX,        configPtr->getImageWidth());
    setIntegerParam(ImageSizeY,        configPtr->getImageHeight());
    setIntegerParam(ImagePatternX,     configPtr->getRepeatX());
    setIntegerParam(ImagePatternY,     configPtr->getRepeatY());
    setIntegerParam(DPixelsPerChipX,   configPtr->getPixelsPerChipX());
    setIntegerParam(DPixelsPerChipY,   configPtr->getPixelsPerChipY());
    setIntegerParam(DChipsPerBlockX,   configPtr->getChipsPerBlockX());
    setIntegerParam(DBlocksPerStripeX, configPtr->getBlocksPerStripeX());
    setIntegerParam(DChipsPerStripeX,  configPtr->getChipsPerStripeX());
    setIntegerParam(DChipsPerStripeY,  configPtr->getChipsPerStripeY());
    setIntegerParam(DStripesPerModule, configPtr->getStripesPerModule());
    setIntegerParam(DStripesPerImage,  configPtr->getStripesPerImage());
  } else if (function == GDPostCommand){
    // Are we starting posting
    if (value == 1){
      // Here we are starting the posting
      int channelEnabled = 0;
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
    }
  } else if (function >= GDTopLeftXChannel1 && function <= GDBotRightYChannel4){
    // First update the value
    setIntegerParam(function, value);
    // Now calculate the width and height of each channel
    for (int channel = 0; channel < 4; channel++){
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

void asynGeneratorDriver::posting_task(int taskNumber)
{
  epicsTimeStamp startTime;
//  epicsTimeStamp frameTime;
  epicsTimeStamp timeNow;
  int status;
  int debugLevel;
  uint32_t prevDebug = 0;
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
  int subFrames;             // Current value of number of sub frames
  int packetSizeAttribute;   // Address of attribute used for packet size in pixels
  int packetSize;            // Current value of packet size in pixels

  int sIWidth = 0;
  int sIHeight = 0;
  uint32_t *buffer = 0;

  double postTime = 0.0;     // Time in seconds of each frame post
  double frequency;          // Frames per second
  DataSender *senderPtr;     // Pointer to the data sender
  static const char *functionName = "asynGeneratorDriver::posting_task";
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s\n", functionName);

std::cout << "TASK " << taskNumber << " - Starting..." << std::endl;

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
  // Setup the sub frames address
  subFrameAttribute = GDSubFramesBase + taskNumber;
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
      getIntegerParam(subFrameAttribute, &subFrames);
      getIntegerParam(packetSizeAttribute, &packetSize);
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
      // Allocate the storage for the image
      buffer = (uint32_t *)malloc(sIWidth * sIHeight * sizeof(uint32_t));
      // Load the sub image into the buffer
      configPtr->copyScrambledSection(subTlx, subTly, subBrx, subBry, buffer);

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

    counter++;

    // Send the sub image, along with the number of sub frames and packet size
    senderPtr->sendImage(buffer, (sIWidth*sIHeight), subFrames, packetSize, ((uint32_t)(postTime * 1000000.0)));

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
    getIntegerParam(postAttribute, &posting);
    this->unlock();
    if (posting){
      epicsTimeGetCurrent(&timeNow);
      double wait = counter*postTime - epicsTimeDiffInSeconds(&timeNow, &startTime);
      status = epicsEventWaitWithTimeout(this->stopEventId_[taskNumber], wait);
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
  createParam(FileWriteStatusString,      asynParamInt32,           &FileWriteStatus);
  createParam(FileWriteMessageString,     asynParamOctet,           &FileWriteMessage);
  createParam(DPixelsPerChipXString,      asynParamInt32,           &DPixelsPerChipX);
  createParam(DPixelsPerChipYString,      asynParamInt32,           &DPixelsPerChipY);
  createParam(DChipsPerBlockXString,      asynParamInt32,           &DChipsPerBlockX);
  createParam(DBlocksPerStripeXString,    asynParamInt32,           &DBlocksPerStripeX);
  createParam(DChipsPerStripeXString,     asynParamInt32,           &DChipsPerStripeX);
  createParam(DChipsPerStripeYString,     asynParamInt32,           &DChipsPerStripeY);
  createParam(DStripesPerModuleString,    asynParamInt32,           &DStripesPerModule);
  createParam(DStripesPerImageString,     asynParamInt32,           &DStripesPerImage);
  createParam(ImageScrambleTypeString,    asynParamInt32,           &ImageScrambleType);

  createParam(GDDebugLevelString,         asynParamInt32,           &GDDebugLevel);

  // Parameters for thread control
  createParam(GDPostCommandString,        asynParamInt32,           &GDPostCommand);
  createParam(GDPostChannel1String,       asynParamInt32,           &GDPostChannel1);
  createParam(GDPostChannel2String,       asynParamInt32,           &GDPostChannel2);
  createParam(GDPostChannel3String,       asynParamInt32,           &GDPostChannel3);
  createParam(GDPostChannel4String,       asynParamInt32,           &GDPostChannel4);
  GDPostBase = GDPostChannel1;
  createParam(GDCounterChannel1String,    asynParamInt32,           &GDCounterChannel1);
  createParam(GDCounterChannel2String,    asynParamInt32,           &GDCounterChannel2);
  createParam(GDCounterChannel3String,    asynParamInt32,           &GDCounterChannel3);
  createParam(GDCounterChannel4String,    asynParamInt32,           &GDCounterChannel4);
  GDCounterBase = GDCounterChannel1;
  createParam(GDEnableChannel1String,     asynParamInt32,           &GDEnableChannel1);
  createParam(GDEnableChannel2String,     asynParamInt32,           &GDEnableChannel2);
  createParam(GDEnableChannel3String,     asynParamInt32,           &GDEnableChannel3);
  createParam(GDEnableChannel4String,     asynParamInt32,           &GDEnableChannel4);
  createParam(GDPostFrequencyString,      asynParamFloat64,         &GDPostFrequency);

  // Parameters to define the frames to send
  createParam(GDTopLeftXChannel1String,   asynParamInt32,           &GDTopLeftXChannel1);
  createParam(GDTopLeftXChannel2String,   asynParamInt32,           &GDTopLeftXChannel2);
  createParam(GDTopLeftXChannel3String,   asynParamInt32,           &GDTopLeftXChannel3);
  createParam(GDTopLeftXChannel4String,   asynParamInt32,           &GDTopLeftXChannel4);
  GDTopLeftXBase = GDTopLeftXChannel1;
  createParam(GDTopLeftYChannel1String,   asynParamInt32,           &GDTopLeftYChannel1);
  createParam(GDTopLeftYChannel2String,   asynParamInt32,           &GDTopLeftYChannel2);
  createParam(GDTopLeftYChannel3String,   asynParamInt32,           &GDTopLeftYChannel3);
  createParam(GDTopLeftYChannel4String,   asynParamInt32,           &GDTopLeftYChannel4);
  GDTopLeftYBase = GDTopLeftYChannel1;
  createParam(GDBotRightXChannel1String,  asynParamInt32,           &GDBotRightXChannel1);
  createParam(GDBotRightXChannel2String,  asynParamInt32,           &GDBotRightXChannel2);
  createParam(GDBotRightXChannel3String,  asynParamInt32,           &GDBotRightXChannel3);
  createParam(GDBotRightXChannel4String,  asynParamInt32,           &GDBotRightXChannel4);
  GDBotRightXBase = GDBotRightXChannel1;
  createParam(GDBotRightYChannel1String,  asynParamInt32,           &GDBotRightYChannel1);
  createParam(GDBotRightYChannel2String,  asynParamInt32,           &GDBotRightYChannel2);
  createParam(GDBotRightYChannel3String,  asynParamInt32,           &GDBotRightYChannel3);
  createParam(GDBotRightYChannel4String,  asynParamInt32,           &GDBotRightYChannel4);
  GDBotRightYBase = GDBotRightYChannel1;
  createParam(GDWidthChannel1String,      asynParamInt32,           &GDWidthChannel1);
  createParam(GDWidthChannel2String,      asynParamInt32,           &GDWidthChannel2);
  createParam(GDWidthChannel3String,      asynParamInt32,           &GDWidthChannel3);
  createParam(GDWidthChannel4String,      asynParamInt32,           &GDWidthChannel4);
  GDWidthBase = GDWidthChannel1;
  createParam(GDHeightChannel1String,     asynParamInt32,           &GDHeightChannel1);
  createParam(GDHeightChannel2String,     asynParamInt32,           &GDHeightChannel2);
  createParam(GDHeightChannel3String,     asynParamInt32,           &GDHeightChannel3);
  createParam(GDHeightChannel4String,     asynParamInt32,           &GDHeightChannel4);
  GDHeightBase = GDHeightChannel1;
  createParam(GDSubFramesChannel1String,  asynParamInt32,           &GDSubFramesChannel1);
  createParam(GDSubFramesChannel2String,  asynParamInt32,           &GDSubFramesChannel2);
  createParam(GDSubFramesChannel3String,  asynParamInt32,           &GDSubFramesChannel3);
  createParam(GDSubFramesChannel4String,  asynParamInt32,           &GDSubFramesChannel4);
  GDSubFramesBase = GDSubFramesChannel1;
  createParam(GDPacketSizeChannel1String, asynParamInt32,           &GDPacketSizeChannel1);
  createParam(GDPacketSizeChannel2String, asynParamInt32,           &GDPacketSizeChannel2);
  createParam(GDPacketSizeChannel3String, asynParamInt32,           &GDPacketSizeChannel3);
  createParam(GDPacketSizeChannel4String, asynParamInt32,           &GDPacketSizeChannel4);
  GDPacketSizeBase = GDPacketSizeChannel1;

  // Parameters for UDP configuration
  createParam(GDLocAddrChannel1String,    asynParamOctet,           &GDLocAddrChannel1);
  createParam(GDLocAddrChannel2String,    asynParamOctet,           &GDLocAddrChannel2);
  createParam(GDLocAddrChannel3String,    asynParamOctet,           &GDLocAddrChannel3);
  createParam(GDLocAddrChannel4String,    asynParamOctet,           &GDLocAddrChannel4);
  GDLocAddrBase = GDLocAddrChannel1;
  createParam(GDLocPortChannel1String,    asynParamInt32,           &GDLocPortChannel1);
  createParam(GDLocPortChannel2String,    asynParamInt32,           &GDLocPortChannel2);
  createParam(GDLocPortChannel3String,    asynParamInt32,           &GDLocPortChannel3);
  createParam(GDLocPortChannel4String,    asynParamInt32,           &GDLocPortChannel4);
  GDLocPortBase = GDLocPortChannel1;
  createParam(GDRemAddrChannel1String,    asynParamOctet,           &GDRemAddrChannel1);
  createParam(GDRemAddrChannel2String,    asynParamOctet,           &GDRemAddrChannel2);
  createParam(GDRemAddrChannel3String,    asynParamOctet,           &GDRemAddrChannel3);
  createParam(GDRemAddrChannel4String,    asynParamOctet,           &GDRemAddrChannel4);
  GDRemAddrBase = GDRemAddrChannel1;
  createParam(GDRemPortChannel1String,    asynParamInt32,           &GDRemPortChannel1);
  createParam(GDRemPortChannel2String,    asynParamInt32,           &GDRemPortChannel2);
  createParam(GDRemPortChannel3String,    asynParamInt32,           &GDRemPortChannel3);
  createParam(GDRemPortChannel4String,    asynParamInt32,           &GDRemPortChannel4);
  GDRemPortBase = GDRemPortChannel1;
  
  // Channel error parameters
  createParam(GDErrorChannel1String,      asynParamInt32,           &GDErrorChannel1);
  createParam(GDErrorChannel2String,      asynParamInt32,           &GDErrorChannel2);
  createParam(GDErrorChannel3String,      asynParamInt32,           &GDErrorChannel3);
  createParam(GDErrorChannel4String,      asynParamInt32,           &GDErrorChannel4);
  GDErrorBase = GDErrorChannel1;
  createParam(GDErrorMsgChannel1String,   asynParamOctet,           &GDErrorMsgChannel1);
  createParam(GDErrorMsgChannel2String,   asynParamOctet,           &GDErrorMsgChannel2);
  createParam(GDErrorMsgChannel3String,   asynParamOctet,           &GDErrorMsgChannel3);
  createParam(GDErrorMsgChannel4String,   asynParamOctet,           &GDErrorMsgChannel4);
  GDErrorMsgBase = GDErrorMsgChannel1;

  // Create the Configurator object
  configPtr = new Configurator();
  // Initialise all values to zero
  configPtr->setImageWidth(0);
  configPtr->setImageHeight(0);
  configPtr->setRepeatX(0);
  configPtr->setRepeatY(0);
  configPtr->setPixelsPerChipX(0);
  configPtr->setPixelsPerChipY(0);
  configPtr->setChipsPerBlockX(0);
  configPtr->setBlocksPerStripeX(0);
  configPtr->setChipsPerStripeX(0);
  configPtr->setChipsPerStripeY(0);
  configPtr->setStripesPerModule(0);
  configPtr->setStripesPerImage(0);


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
  //setIntegerParam(DataType,       UInt8);
  setIntegerParam(FileWriteStatus,      0);
  setStringParam (FilePath,             "");
  setStringParam (FileName,             "");
  setStringParam (FullFileName,         "");
  setStringParam (FileWriteMessage,     "");
  setIntegerParam(DPixelsPerChipX,      configPtr->getPixelsPerChipX());
  setIntegerParam(DPixelsPerChipY,      configPtr->getPixelsPerChipY());
  setIntegerParam(DChipsPerBlockX,      configPtr->getChipsPerBlockX());
  setIntegerParam(DBlocksPerStripeX,    configPtr->getBlocksPerStripeX());
  setIntegerParam(DChipsPerStripeX,     configPtr->getChipsPerStripeX());
  setIntegerParam(DChipsPerStripeY,     configPtr->getChipsPerStripeY());
  setIntegerParam(DStripesPerModule,    configPtr->getStripesPerModule());
  setIntegerParam(DStripesPerImage,     configPtr->getStripesPerImage());

  setIntegerParam(GDDebugLevel,         0);

  setIntegerParam(GDPostCommand,        0);
  setIntegerParam(GDPostChannel1,       0);
  setIntegerParam(GDPostChannel2,       0);
  setIntegerParam(GDPostChannel3,       0);
  setIntegerParam(GDPostChannel4,       0);
  setIntegerParam(GDCounterChannel1,    0);
  setIntegerParam(GDCounterChannel2,    0);
  setIntegerParam(GDCounterChannel3,    0);
  setIntegerParam(GDCounterChannel4,    0);
  setIntegerParam(GDEnableChannel1,     0);
  setIntegerParam(GDEnableChannel2,     0);
  setIntegerParam(GDEnableChannel3,     0);
  setIntegerParam(GDEnableChannel4,     0);
  setDoubleParam (GDPostFrequency,      1.0);

  setIntegerParam(GDTopLeftXChannel1,   0);
  setIntegerParam(GDTopLeftXChannel2,   0);
  setIntegerParam(GDTopLeftXChannel3,   0);
  setIntegerParam(GDTopLeftXChannel4,   0);
  setIntegerParam(GDTopLeftYChannel1,   0);
  setIntegerParam(GDTopLeftYChannel2,   0);
  setIntegerParam(GDTopLeftYChannel3,   0);
  setIntegerParam(GDTopLeftYChannel4,   0);
  setIntegerParam(GDBotRightXChannel1,  0);
  setIntegerParam(GDBotRightXChannel2,  0);
  setIntegerParam(GDBotRightXChannel3,  0);
  setIntegerParam(GDBotRightXChannel4,  0);
  setIntegerParam(GDBotRightYChannel1,  0);
  setIntegerParam(GDBotRightYChannel2,  0);
  setIntegerParam(GDBotRightYChannel3,  0);
  setIntegerParam(GDBotRightYChannel4,  0);

  setIntegerParam(GDWidthChannel1,      0);
  setIntegerParam(GDWidthChannel2,      0);
  setIntegerParam(GDWidthChannel3,      0);
  setIntegerParam(GDWidthChannel4,      0);
  setIntegerParam(GDHeightChannel1,     0);
  setIntegerParam(GDHeightChannel2,     0);
  setIntegerParam(GDHeightChannel3,     0);
  setIntegerParam(GDHeightChannel4,     0);

  setIntegerParam(GDSubFramesChannel1,  0);
  setIntegerParam(GDSubFramesChannel2,  0);
  setIntegerParam(GDSubFramesChannel3,  0);
  setIntegerParam(GDSubFramesChannel4,  0);
  setIntegerParam(GDPacketSizeChannel1, 0);
  setIntegerParam(GDPacketSizeChannel2, 0);
  setIntegerParam(GDPacketSizeChannel3, 0);
  setIntegerParam(GDPacketSizeChannel4, 0);

  setStringParam (GDLocAddrChannel1,    "");
  setStringParam (GDLocAddrChannel2,    "");
  setStringParam (GDLocAddrChannel3,    "");
  setStringParam (GDLocAddrChannel4,    "");
  setIntegerParam(GDLocPortChannel1,    0);
  setIntegerParam(GDLocPortChannel2,    0);
  setIntegerParam(GDLocPortChannel3,    0);
  setIntegerParam(GDLocPortChannel4,    0);
  setStringParam (GDRemAddrChannel1,    "");
  setStringParam (GDRemAddrChannel2,    "");
  setStringParam (GDRemAddrChannel3,    "");
  setStringParam (GDRemAddrChannel4,    "");
  setIntegerParam(GDRemPortChannel1,    0);
  setIntegerParam(GDRemPortChannel2,    0);
  setIntegerParam(GDRemPortChannel3,    0);
  setIntegerParam(GDRemPortChannel4,    0);

  setIntegerParam(GDErrorChannel1,      0);
  setIntegerParam(GDErrorChannel2,      0);
  setIntegerParam(GDErrorChannel3,      0);
  setIntegerParam(GDErrorChannel4,      0);
  setStringParam (GDErrorMsgChannel1,   "");
  setStringParam (GDErrorMsgChannel2,   "");
  setStringParam (GDErrorMsgChannel3,   "");
  setStringParam (GDErrorMsgChannel4,   "");

  this->startEventId_[0] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[1] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[2] = epicsEventCreate(epicsEventEmpty);
  this->startEventId_[3] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[0] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[1] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[2] = epicsEventCreate(epicsEventEmpty);
  this->stopEventId_[3] = epicsEventCreate(epicsEventEmpty);

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

//  FILE* fp = stdout;
//  reportParams(fp, 3);
//  callParamCallbacks();
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


