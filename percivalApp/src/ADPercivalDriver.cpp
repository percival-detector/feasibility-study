/*
 * ADPercivalDriver.cpp
 *
 *  Created on: 24th Aug 2013
 *      Author: gnx91527
 */

#include <string.h>
#include <sys/stat.h>
#include <iocsh.h>
#include <epicsExport.h>

#include "ADPercivalDriver.h"

static const char *driverName = "ADPercivalDriver";

//static void acq_task_c(void *ptr)
//{
//	ADPercivalDriver *pPtr = (ADPercivalDriver *)ptr;
//  pPtr->acquisition_task();
//}


/** This is the constructor for the ADPercivalDriver class.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] maxBuffers
  * \param[in] maxMemory
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
ADPercivalDriver::ADPercivalDriver(const char *portName,
                                   int maxBuffers,
                                   size_t maxMemory,
                                   int priority,
                                   int stackSize)
    : ADDriver(portName,
               1,
               NUM_PERCIVAL_PARAMS,
               maxBuffers,
               maxMemory,
               0, 0, /* No interfaces beyond those set in ADDriver.cpp */
               0, 1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=0, autoConnect=1 */
               priority, stackSize)
{
  static const char *functionName = "ADPercivalDriver::ADPercivalDriver";
  int status = asynSuccess;

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s\n", functionName);

  // Setup the parameters
  ndims_ = 2;   // 2 dimensions, width and height
  dims_[0] = 0; // Defaults to width of 0, this is read from configuration file
  dims_[1] = 0; // Defaults to height of 0, this is read from configuration file

  // Create all internal parameters.
  // Name of driver port
//  createParam(PortNameSelfString,         asynParamOctet,           &PortNameSelf);

  // Full frame image data
  //createParam(ImageSizeXString,           asynParamInt32,           &ImageSizeX);
  //createParam(ImageSizeYString,           asynParamInt32,           &ImageSizeY);
  //createParam(ImageDataTypeString,        asynParamInt32,           &ImageDataType);

  // Configuration file setup data
  createParam(PercFilePathString,           asynParamOctet,     &PercFilePath);
  createParam(PercFilePathExistsString,     asynParamInt32,     &PercFilePathExists);
  createParam(PercFileNameString,           asynParamOctet,     &PercFileName);
  createParam(PercFullFileNameString,       asynParamOctet,     &PercFullFileName);
  createParam(PercConfigFileReadString,     asynParamInt32,     &PercConfigFileRead);
  createParam(PercFileReadStatusString,     asynParamInt32,     &PercFileReadStatus);
  createParam(PercFileReadMessageString,    asynParamOctet,     &PercFileReadMessage);

  createParam(PercDebugLevelString,         asynParamInt32,     &PercDebugLevel);

  createParam(PercEnableChannel1String,     asynParamInt32,     &PercEnableChannel1);
  createParam(PercEnableChannel2String,     asynParamInt32,     &PercEnableChannel2);
  createParam(PercEnableChannel3String,     asynParamInt32,     &PercEnableChannel3);
  createParam(PercEnableChannel4String,     asynParamInt32,     &PercEnableChannel4);

  createParam(PercTopLeftXChannel1String,   asynParamInt32,     &PercTopLeftXChannel1);
  createParam(PercTopLeftXChannel2String,   asynParamInt32,     &PercTopLeftXChannel2);
  createParam(PercTopLeftXChannel3String,   asynParamInt32,     &PercTopLeftXChannel3);
  createParam(PercTopLeftXChannel4String,   asynParamInt32,     &PercTopLeftXChannel4);
  createParam(PercTopLeftYChannel1String,   asynParamInt32,     &PercTopLeftYChannel1);
  createParam(PercTopLeftYChannel2String,   asynParamInt32,     &PercTopLeftYChannel2);
  createParam(PercTopLeftYChannel3String,   asynParamInt32,     &PercTopLeftYChannel3);
  createParam(PercTopLeftYChannel4String,   asynParamInt32,     &PercTopLeftYChannel4);
  createParam(PercBotRightXChannel1String,  asynParamInt32,     &PercBotRightXChannel1);
  createParam(PercBotRightXChannel2String,  asynParamInt32,     &PercBotRightXChannel2);
  createParam(PercBotRightXChannel3String,  asynParamInt32,     &PercBotRightXChannel3);
  createParam(PercBotRightXChannel4String,  asynParamInt32,     &PercBotRightXChannel4);
  createParam(PercBotRightYChannel1String,  asynParamInt32,     &PercBotRightYChannel1);
  createParam(PercBotRightYChannel2String,  asynParamInt32,     &PercBotRightYChannel2);
  createParam(PercBotRightYChannel3String,  asynParamInt32,     &PercBotRightYChannel3);
  createParam(PercBotRightYChannel4String,  asynParamInt32,     &PercBotRightYChannel4);
  createParam(PercWidthChannel1String,      asynParamInt32,     &PercWidthChannel1);
  createParam(PercWidthChannel2String,      asynParamInt32,     &PercWidthChannel2);
  createParam(PercWidthChannel3String,      asynParamInt32,     &PercWidthChannel3);
  createParam(PercWidthChannel4String,      asynParamInt32,     &PercWidthChannel4);
  createParam(PercHeightChannel1String,     asynParamInt32,     &PercHeightChannel1);
  createParam(PercHeightChannel2String,     asynParamInt32,     &PercHeightChannel2);
  createParam(PercHeightChannel3String,     asynParamInt32,     &PercHeightChannel3);
  createParam(PercHeightChannel4String,     asynParamInt32,     &PercHeightChannel4);
  createParam(PercSubFramesChannel1String,  asynParamInt32,     &PercSubFramesChannel1);
  createParam(PercSubFramesChannel2String,  asynParamInt32,     &PercSubFramesChannel2);
  createParam(PercSubFramesChannel3String,  asynParamInt32,     &PercSubFramesChannel3);
  createParam(PercSubFramesChannel4String,  asynParamInt32,     &PercSubFramesChannel4);
  createParam(PercAddrChannel1String,       asynParamOctet,     &PercAddrChannel1);
  createParam(PercAddrChannel2String,       asynParamOctet,     &PercAddrChannel2);
  createParam(PercAddrChannel3String,       asynParamOctet,     &PercAddrChannel3);
  createParam(PercAddrChannel4String,       asynParamOctet,     &PercAddrChannel4);
  createParam(PercPortChannel1String,       asynParamInt32,     &PercPortChannel1);
  createParam(PercPortChannel2String,       asynParamInt32,     &PercPortChannel2);
  createParam(PercPortChannel3String,       asynParamInt32,     &PercPortChannel3);
  createParam(PercPortChannel4String,       asynParamInt32,     &PercPortChannel4);

  // Set some default values for parameters
  status =  setStringParam (ADManufacturer, "Percival");
  status |= setStringParam (ADModel, "TestCam");
  status |= setIntegerParam(ADMaxSizeX, 0);
  status |= setIntegerParam(ADMaxSizeY, 0);
  status |= setIntegerParam(ADSizeX, 0);
  status |= setIntegerParam(ADSizeY, 0);
  status |= setIntegerParam(NDArraySizeX, 0);
  status |= setIntegerParam(NDArraySizeY, 0);
  status |= setIntegerParam(NDArraySize, 0);
  status |= setIntegerParam(NDDataType, NDUInt16);
  status |= setIntegerParam(ADImageMode, ADImageContinuous);
  status |= setIntegerParam(ADAcquire, 0);
  status |= setIntegerParam(ADStatus, ADStatusIdle);
  status |= setDoubleParam (ADAcquireTime, .000);
  status |= setDoubleParam (ADAcquirePeriod, .000);
  status |= setIntegerParam(ADNumImages, 100);
  status |= setIntegerParam(PercDebugLevel, 0);
  status |= setStringParam(ADStatusMessage, " ");

  status |= setIntegerParam(PercEnableChannel1,     0);
  status |= setIntegerParam(PercEnableChannel2,     0);
  status |= setIntegerParam(PercEnableChannel3,     0);
  status |= setIntegerParam(PercEnableChannel4,     0);

  status |= setIntegerParam(PercTopLeftXChannel1,   0);
  status |= setIntegerParam(PercTopLeftXChannel2,   0);
  status |= setIntegerParam(PercTopLeftXChannel3,   0);
  status |= setIntegerParam(PercTopLeftXChannel4,   0);
  status |= setIntegerParam(PercTopLeftYChannel1,   0);
  status |= setIntegerParam(PercTopLeftYChannel2,   0);
  status |= setIntegerParam(PercTopLeftYChannel3,   0);
  status |= setIntegerParam(PercTopLeftYChannel4,   0);
  status |= setIntegerParam(PercBotRightXChannel1,  0);
  status |= setIntegerParam(PercBotRightXChannel2,  0);
  status |= setIntegerParam(PercBotRightXChannel3,  0);
  status |= setIntegerParam(PercBotRightXChannel4,  0);
  status |= setIntegerParam(PercBotRightYChannel1,  0);
  status |= setIntegerParam(PercBotRightYChannel2,  0);
  status |= setIntegerParam(PercBotRightYChannel3,  0);
  status |= setIntegerParam(PercBotRightYChannel4,  0);

  status |= setIntegerParam(PercWidthChannel1,      0);
  status |= setIntegerParam(PercWidthChannel2,      0);
  status |= setIntegerParam(PercWidthChannel3,      0);
  status |= setIntegerParam(PercWidthChannel4,      0);
  status |= setIntegerParam(PercHeightChannel1,     0);
  status |= setIntegerParam(PercHeightChannel2,     0);
  status |= setIntegerParam(PercHeightChannel3,     0);
  status |= setIntegerParam(PercHeightChannel4,     0);

  status |= setIntegerParam(PercSubFramesChannel1,  0);
  status |= setIntegerParam(PercSubFramesChannel2,  0);
  status |= setIntegerParam(PercSubFramesChannel3,  0);
  status |= setIntegerParam(PercSubFramesChannel4,  0);

  status |= setStringParam (PercAddrChannel1,       "");
  status |= setStringParam (PercAddrChannel2,       "");
  status |= setStringParam (PercAddrChannel3,       "");
  status |= setStringParam (PercAddrChannel4,       "");
  status |= setIntegerParam(PercPortChannel1,       0);
  status |= setIntegerParam(PercPortChannel2,       0);
  status |= setIntegerParam(PercPortChannel3,       0);
  status |= setIntegerParam(PercPortChannel4,       0);

  callParamCallbacks();


  // Initialise the buffer pool pointer to 0
  buffers_ = 0;
  //buffers_ = new PercivalBufferPool(2048 * 1024 * sizeof(short));

//  rawImage_ = (unsigned short *)calloc(3680 * 3680, sizeof(short));

  // Create the configuration class
  configPtr_ = new Configurator();

  // Create the Percival server class
  sPtr_ = new PercivalServer();

  // Temporarily setup fake full frame
  // Setup for excalibur, 2048 * 256 * 4 stripes, no scramble, gain,
  // or any other factors
  sPtr_->setupFullFrame(256, 256, UnsignedInt16, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

  // Setup four sub frames, hardwire for excalibur 2048 * 256
  // Ports 9876, 9877, 9878, 9879
  sPtr_->setupSubFrame(0, "127.0.0.1", 9876, 0, 0,   255, 255,  2);
  //sPtr_->setupSubFrame(0, "127.0.0.1", 9876, 0, 0,   2047, 255,  2);
  //sPtr_->setupSubFrame(1, "127.0.0.1", 9877, 0, 256, 2047, 511,  2);
  //sPtr_->setupSubFrame(2, "127.0.0.1", 9878, 0, 512, 2047, 767,  2);
  //sPtr_->setupSubFrame(3, "127.0.0.1", 9879, 0, 768, 2047, 1023, 2);

  // Register callback as this class
  sPtr_->registerCallback(this);

	this->startEventId_ = epicsEventCreate(epicsEventEmpty);
	if (!this->startEventId_) {
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s epicsEventCreate failure for start event\n",
				driverName, functionName);
		return;
	}
	this->stopEventId_ = epicsEventCreate(epicsEventEmpty);
	if (!this->stopEventId_) {
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s epicsEventCreate failure for stop event\n",
				driverName, functionName);
		return;
	}

	// Create the thread that is notified about completed images
	/*status = (epicsThreadCreate("acq_task",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)acq_task_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
              "%s:%s epicsThreadCreate failure for image task\n",
              driverName, functionName);
    return;
  }*/

}

/*
void ADPercivalDriver::acquisitionTask()
{
  static const char *functionName = "ADPercivalDriver::acquisitionTask";
  int status = asynSuccess;
  int imageCounter;
  int numImages, numImagesCounter;
  int imageMode;
  int arrayCallbacks;

    // Loop forever
    while (1) {


  // Allocate an NDArray
  if (pImage_){
    pImage_->release();
  }
  // Allocate the buffer using the read image.
  pImage_ = this->pNDArrayPool->alloc(ndims_, dims_, NDUInt16, buffer->size(), buffer->raw());
  if (!pImage_){
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
              "%s:%s: error allocating raw buffer\n",
              driverName, functionName);
  } else {
    pImage_->getInfo(&arrayInfo_);

    status = asynSuccess;
    status |= setIntegerParam(NDArraySize,  arrayInfo_.totalBytes);
    status |= setIntegerParam(NDArraySizeX, pImage_->dims[0].size);
    status |= setIntegerParam(NDArraySizeY, pImage_->dims[1].size);
    if (status) asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                          "%s:%s: error setting parameters\n",
                          driverName, functionName);

    // Get the current parameters
    getIntegerParam(NDArrayCounter, &imageCounter);
    getIntegerParam(ADNumImages, &numImages);
    getIntegerParam(ADNumImagesCounter, &numImagesCounter);
    getIntegerParam(ADImageMode, &imageMode);
    getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
    imageCounter++;
    numImagesCounter++;

    // Put the frame number and time stamp into the buffer
    pImage_->uniqueId = imageCounter;
    //pImage_->timeStamp = startTime.secPastEpoch + startTime.nsec / 1.e9;
  
    // Set parameters based on collected image data
    setIntegerParam(NDArrayCounter, imageCounter);
    setIntegerParam(ADNumImagesCounter, numImagesCounter);

    // Get any attributes that have been defined for this driver
    this->getAttributes(pImage_->pAttributeList);

    if (arrayCallbacks) {
      // Call the NDArray callback
      // Must release the lock here, or we can get into a deadlock, because we can
      // block on the plugin lock, and the plugin can be calling us
 //     this->unlock();
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                "%s:%s: calling imageData callback\n", driverName, functionName);
      doCallbacksGenericPointer(pImage_, NDArrayData, 0);
 //     this->lock();
    }
  }

  // Release the Percival buffer back into the pool
  buffers_->free(buffer);

}
*/
void ADPercivalDriver::imageReceived(PercivalBuffer *buffer)
{
  static const char *functionName = "ADPercivalDriver::imageReceived";
  int status = asynSuccess;
  int imageCounter;
  int numImages, numImagesCounter;
  int imageMode;
  int arrayCallbacks;

  this->lock();

  // Allocate an NDArray
  if (pImage_){
    pImage_->release();
  }
  // Allocate the buffer using the read image.
  pImage_ = this->pNDArrayPool->alloc(ndims_, dims_, NDUInt16, buffer->size(), buffer->raw());
  if (!pImage_){
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
              "%s:%s: error allocating raw buffer\n",
              driverName, functionName);
  } else {
    pImage_->getInfo(&arrayInfo_);

    status = asynSuccess;
    status |= setIntegerParam(NDArraySize,  arrayInfo_.totalBytes);
    status |= setIntegerParam(NDArraySizeX, pImage_->dims[0].size);
    status |= setIntegerParam(NDArraySizeY, pImage_->dims[1].size);
    if (status) asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                          "%s:%s: error setting parameters\n",
                          driverName, functionName);

    // Get the current parameters
    getIntegerParam(NDArrayCounter, &imageCounter);
    getIntegerParam(ADNumImages, &numImages);
    getIntegerParam(ADNumImagesCounter, &numImagesCounter);
    getIntegerParam(ADImageMode, &imageMode);
    getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
    imageCounter++;
    numImagesCounter++;

    // Put the frame number and time stamp into the buffer
    pImage_->uniqueId = imageCounter;
    //pImage_->timeStamp = startTime.secPastEpoch + startTime.nsec / 1.e9;
  
    // Set parameters based on collected image data
    setIntegerParam(NDArrayCounter, imageCounter);
    setIntegerParam(ADNumImagesCounter, numImagesCounter);

    // Get any attributes that have been defined for this driver
    this->getAttributes(pImage_->pAttributeList);

    callParamCallbacks();

    if (arrayCallbacks) {
      // Call the NDArray callback
      // Must release the lock here, or we can get into a deadlock, because we can
      // block on the plugin lock, and the plugin can be calling us
      this->unlock();
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                "%s:%s: calling imageData callback\n", driverName, functionName);
      doCallbacksGenericPointer(pImage_, NDArrayData, 0);
      this->lock();
    }
  }

  // Release the Percival buffer back into the pool
  buffers_->free(buffer);

  this->unlock();
}

PercivalBuffer *ADPercivalDriver::allocateBuffer()
{
  const char *functionName = "allocateBuffer";
  PercivalBuffer *buffer = buffers_->allocate();
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, 
              "%s:%s: allocated buffer address: %ld\n", 
              driverName, functionName, (long int)buffer);
  return buffer;
}

/** Called when asyn clients call pasynOctet->write().
  * This function performs actions for some parameters, including AttributesFile.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Address of the string to write.
  * \param[in] nChars Number of characters to write.
  * \param[out] nActual Number of characters actually written. */
asynStatus ADPercivalDriver::writeOctet(asynUser *pasynUser, const char *value, size_t nChars, size_t *nActual)
{
  int addr=0;
  int function = pasynUser->reason;
  asynStatus status = asynSuccess;
  const char *functionName = "writeOctet";

  status = getAddress(pasynUser, &addr); if (status != asynSuccess) return(status);
  // Set the parameter in the parameter library.
  status = (asynStatus)setStringParam(addr, function, (char *)value);

  if (function == PercFilePath) {
    this->checkPath();
    char fullName[MAX_FILENAME_LEN];
    this->createFileName(MAX_FILENAME_LEN, fullName);
    setStringParam(addr, PercFullFileName, fullName);
  } else if (function == PercFileName){
    char fullName[MAX_FILENAME_LEN];
    this->createFileName(MAX_FILENAME_LEN, fullName);
    setStringParam(addr, PercFullFileName, fullName);
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


asynStatus ADPercivalDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int param = pasynUser->reason;
  int status = asynSuccess;

  if (param == ADAcquire){
    epicsInt32 adstatus;
    getIntegerParam(ADStatus, &adstatus);
    if (value && (adstatus == ADStatusIdle)){
      sPtr_->startAcquisition();
      setStringParam(ADStatusMessage, "Acquiring images");
    }
    if (!value && (adstatus != ADStatusIdle)){
      // This is a command to stop acquisition
      sPtr_->stopAcquisition();
      setStringParam(ADStatusMessage, "Acquisition completed");
    }
  } else if (param == PercConfigFileRead){
    char fileName[MAX_FILENAME_LEN];
    setIntegerParam(PercFileReadStatus, 1);
    setStringParam (PercFileReadMessage, "Reading configuration file");
    callParamCallbacks();
    status |= getStringParam(PercFullFileName, sizeof(fileName), fileName);
    configPtr_->readConfiguration(fileName);
    // Setup image dimensions
    dims_[0] = configPtr_->getImageWidth();
    dims_[1] = configPtr_->getImageHeight();

    setStringParam(PercFileReadMessage, "Read configuration complete");
    setIntegerParam(PercFileReadStatus, 0);
    setIntegerParam(ADSizeX,            dims_[0]);
    setIntegerParam(ADSizeY,            dims_[1]);
    setIntegerParam(NDArraySizeX,       dims_[0]);
    setIntegerParam(NDArraySizeY,       dims_[1]);
    setIntegerParam(NDDataType,         configPtr_->getDataType());

    // Create the Percival buffer pool from the size of image loaded from the configuration file
    if (buffers_){
      delete(buffers_);
    }
    buffers_ = new PercivalBufferPool(dims_[0] * dims_[1] * (uint32_t)pow(2.0, (double)configPtr_->getDataType()));
    setIntegerParam(NDArraySize, dims_[0] * dims_[1] * (uint32_t)pow(2.0, (double)configPtr_->getDataType()));

    // Setup server for full frame as defined by configuration file
    sPtr_->setupFullFrame(dims_[0], dims_[1], UnsignedInt16, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

  } else if (param == PercDebugLevel){
    setIntegerParam(PercDebugLevel, value);
    // Set the debug level within the classes
    sPtr_->setDebug(value);
  } else if (param >= PercTopLeftXChannel1 && param <= PercBotRightYChannel4){
    // First update the value
    setIntegerParam(param, value);
    // Now calculate the width and height of each channel
    for (int channel = 0; channel < 4; channel++){
      int tlx, tly, brx, bry, width, height;
      getIntegerParam(PercTopLeftXChannel1+channel, &tlx);
      getIntegerParam(PercTopLeftYChannel1+channel, &tly);
      getIntegerParam(PercBotRightXChannel1+channel, &brx);
      getIntegerParam(PercBotRightYChannel1+channel, &bry);
      width = brx - tlx + 1;
      if (width < 0) width = 0;
      height = bry - tly + 1;
      if (height < 0) height = 0;
      setIntegerParam(PercWidthChannel1+channel, width);
      setIntegerParam(PercHeightChannel1+channel, height);
    }
  } else if (param >= PercEnableChannel1 && param <= PercEnableChannel4){
    setIntegerParam(param, value);
  } else if (param >= PercSubFramesChannel1 && param <= PercSubFramesChannel4){
    setIntegerParam(param, value);
  } else if (param >= PercPortChannel1 && param <= PercPortChannel4){
    setIntegerParam(param, value);
  } else {
    // If this parameter belongs to a base class call its method
    if (param < FIRST_PERCIVAL_PARAM) status = ADDriver::writeInt32(pasynUser, value);
  }

  callParamCallbacks();

  if (status){
    asynPrint(pasynUser, ASYN_TRACE_ERROR,
              "%s:writeInt32 ERROR, status=%d param=%d, value=%d\n",
              driverName, status, param, value);
  } else {
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:writeInt32: param=%d, value=%d\n",
              driverName, param, value);
  }

  return (asynStatus)status;
}

/*asynStatus ADPercivalDriver::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
  int param = pasynUser->reason;
  asynStatus status = asynSuccess;

  // Just read the status of the NDArrayPool
  if (param == NDPoolMaxBuffers) {
    setIntegerParam(param, this->pNDArrayPool->maxBuffers());
  }

  // Call base class
  status = asynPortDriver::readInt32(pasynUser, value);
  return status;
}

#define MEGABYTE_DBL 1048576.
asynStatus ADPercivalDriver::readFloat64(asynUser *pasynUser, epicsFloat64 *value)
{
  int param = pasynUser->reason;
  asynStatus status = asynSuccess;

  // Just read the status of the NDArrayPool
  if (param == NDPoolMaxMemory) {
    setDoubleParam(param, this->pNDArrayPool->maxMemory() / MEGABYTE_DBL);
  }

  // Call base class
  status = asynPortDriver::readFloat64(pasynUser, value);
  return status;
}*/

/** Report status of the driver.
  * This method calls the report function in the asynPortDriver base class.
  * \param[in] fp File pointed passed by caller where the output is written to.
  * \param[in] details
  */
void ADPercivalDriver::report(FILE *fp, int details)
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
int ADPercivalDriver::checkPath()
{
  // Formats a complete file name from the components
  int status = asynError;
  char filePath[MAX_FILENAME_LEN];
  int hasTerminator=0;
  struct stat buff;
  int len;
  int isDir=0;
  int pathExists=0;
    
  status = getStringParam(PercFilePath, sizeof(filePath), filePath);
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
    setStringParam(PercFilePath, filePath);
  }
  setIntegerParam(PercFilePathExists, pathExists);
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
int ADPercivalDriver::createFileName(int maxChars, char *fullFileName)
{
  // Formats a complete file name from the components defined in NDStdDriverParams
  int status = asynSuccess;
  char filePath[MAX_FILENAME_LEN];
  char fileName[MAX_FILENAME_LEN];
  int len;
    
  this->checkPath();
  status |= getStringParam(PercFilePath, sizeof(filePath), filePath);
  status |= getStringParam(PercFileName, sizeof(fileName), fileName); 
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
int ADPercivalDriver::createFileName(int maxChars, char *filePath, char *fileName)
{
  // Formats a complete file name from the components defined in NDStdDriverParams
  int status = asynSuccess;
  char name[MAX_FILENAME_LEN];
    
  this->checkPath();
  status |= getStringParam(PercFilePath, maxChars, filePath); 
  status |= getStringParam(PercFileName, sizeof(name), fileName); 
  return(status);   
}



// Configuration command
extern "C" int ADPercivalDriverConfigure(const char *portName)
{
    new ADPercivalDriver(portName,
                         0,
                         1,
                         0,
                         104857600);
    return(asynSuccess);
}

/* EPICS iocsh shell commands */
static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg * const initArgs[] = {&initArg0};
static const iocshFuncDef initFuncDef = {"PercivalInit",1,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
  ADPercivalDriverConfigure(args[0].sval);
}

extern "C" void ADPercivalDriverRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
epicsExportRegistrar(ADPercivalDriverRegister);
}
