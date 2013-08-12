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
  int status;
  int postAttribute;         // Address of attribute used for starting/stopping posting
  int posting;               // Current posting value
  int counterAttribute;      // Address of attribute used for the current count of posted data
  int counter;               // Current value of counter
  static const char *functionName = "asynGeneratorDriver::posting_task";
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s\n", functionName);

std::cout << "TASK " << taskNumber << " - Starting..." << std::endl;

  // Setup the task post address
  postAttribute = GDPostBase + taskNumber;
  // Setup the counter address
  counterAttribute = GDCounterBase + taskNumber;

  // Loop forever in this task
  while (1){
    // Is posting active for this thread?
    this->lock();
    getIntegerParam(postAttribute, &posting);
    this->unlock();
    // If we are not posting then wait for a semaphore that is given when acquisition is started
    if (!posting){
      //setIntegerParam(ADStatus, ADStatusIdle);
      //callParamCallbacks();

      // Wait for an event that says posting has started
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: waiting for acquire to start\n", driverName, functionName);

      status = epicsEventWait(this->startEventId_[taskNumber]);
      this->lock();
      setIntegerParam(counterAttribute, 0);
      this->unlock();
      counter = 0;

      //setIntegerParam(ADStatus, ADStatusAcquire);
      //setStringParam(ADStatusMessage, "Acquiring images");
      //callParamCallbacks();

    }
    // We are posting, get the current time
    epicsTimeGetCurrent(&startTime);

    // Post
std::cout << "TASK " << taskNumber << " - Posting now..." << std::endl;

    // Call the callbacks to update any changes
    this->lock();
    callParamCallbacks();
    getIntegerParam(postAttribute, &posting);
    this->unlock();
    if (posting){
      status = epicsEventWaitWithTimeout(this->stopEventId_[taskNumber], 1.0);
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

  // Create the Configurator object
  configPtr = new Configurator();
  // Initialise all values to zero
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
  setStringParam (PortNameSelf,      portName);
  setIntegerParam(ImageSizeX,        configPtr->getImageWidth());
  setIntegerParam(ImageSizeY,        configPtr->getImageHeight());
  setIntegerParam(ImagePatternX,     configPtr->getRepeatX());
  setIntegerParam(ImagePatternY,     configPtr->getRepeatY());
  setIntegerParam(ImageSizeZ,        0);
  setIntegerParam(ImageSize,         0);
  setIntegerParam(NDimensions,       3);
  //setIntegerParam(DataType,       UInt8);
  setIntegerParam(FileWriteStatus,   0);
  setStringParam (FilePath,          "");
  setStringParam (FileName,          "");
  setStringParam (FullFileName,      "");
  setStringParam (FileWriteMessage,  "");
  setIntegerParam(DPixelsPerChipX,   configPtr->getPixelsPerChipX());
  setIntegerParam(DPixelsPerChipY,   configPtr->getPixelsPerChipY());
  setIntegerParam(DChipsPerBlockX,   configPtr->getChipsPerBlockX());
  setIntegerParam(DBlocksPerStripeX, configPtr->getBlocksPerStripeX());
  setIntegerParam(DChipsPerStripeX,  configPtr->getChipsPerStripeX());
  setIntegerParam(DChipsPerStripeY,  configPtr->getChipsPerStripeY());
  setIntegerParam(DStripesPerModule, configPtr->getStripesPerModule());
  setIntegerParam(DStripesPerImage,  configPtr->getStripesPerImage());

  setIntegerParam(GDPostCommand,     0);
  setIntegerParam(GDPostChannel1,    0);
  setIntegerParam(GDPostChannel2,    0);
  setIntegerParam(GDPostChannel3,    0);
  setIntegerParam(GDPostChannel4,    0);
  setIntegerParam(GDCounterChannel1, 0);
  setIntegerParam(GDCounterChannel2, 0);
  setIntegerParam(GDCounterChannel3, 0);
  setIntegerParam(GDCounterChannel4, 0);
  setIntegerParam(GDEnableChannel1,  0);
  setIntegerParam(GDEnableChannel2,  0);
  setIntegerParam(GDEnableChannel3,  0);
  setIntegerParam(GDEnableChannel4,  0);

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
                               asynDrvUserMask | asynInt32Mask | asynInt32ArrayMask | asynOctetMask,
                               asynInt32Mask | asynInt32ArrayMask | asynOctetMask,
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


