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

static void stats_task_c(void *ptr)
{
	ADPercivalDriver *drvPtr = (ADPercivalDriver *)ptr;
  drvPtr->stats_task();
}



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

  // Configuration file setup data
  createParam(PercFilePathString,           asynParamOctet,     &PercFilePath);
  createParam(PercFilePathExistsString,     asynParamInt32,     &PercFilePathExists);
  createParam(PercFileNameString,           asynParamOctet,     &PercFileName);
  createParam(PercFullFileNameString,       asynParamOctet,     &PercFullFileName);
  createParam(PercConfigFileReadString,     asynParamInt32,     &PercConfigFileRead);
  createParam(PercFileReadStatusString,     asynParamInt32,     &PercFileReadStatus);
  createParam(PercFileReadMessageString,    asynParamOctet,     &PercFileReadMessage);
  createParam(PercFileErrorStatusString,    asynParamInt32,     &PercFileErrorStatus);

  createParam(PercDebugLevelString,         asynParamInt32,     &PercDebugLevel);
  createParam(PercWatchdogTimeoutString,    asynParamInt32,     &PercWatchdogTimeout);

  createParam(PercChannelModeString,        asynParamInt32,     &PercChannelMode);
  createParam(PercDescrambleString,         asynParamInt32,     &PercDescramble);

  //createParam(PercEnableChannel1String,     asynParamInt32,     &PercEnableChannel1);
  //createParam(PercEnableChannel2String,     asynParamInt32,     &PercEnableChannel2);
  //createParam(PercEnableChannel3String,     asynParamInt32,     &PercEnableChannel3);
  //createParam(PercEnableChannel4String,     asynParamInt32,     &PercEnableChannel4);

  createParam(PercTopLeftXSF1String,   asynParamInt32,     &PercTopLeftXSF1);
  createParam(PercTopLeftXSF2String,   asynParamInt32,     &PercTopLeftXSF2);
  createParam(PercTopLeftXSF3String,   asynParamInt32,     &PercTopLeftXSF3);
  createParam(PercTopLeftXSF4String,   asynParamInt32,     &PercTopLeftXSF4);
  createParam(PercTopLeftXSF5String,   asynParamInt32,     &PercTopLeftXSF5);
  createParam(PercTopLeftXSF6String,   asynParamInt32,     &PercTopLeftXSF6);
  createParam(PercTopLeftXSF7String,   asynParamInt32,     &PercTopLeftXSF7);
  createParam(PercTopLeftXSF8String,   asynParamInt32,     &PercTopLeftXSF8);
  createParam(PercTopLeftYSF1String,   asynParamInt32,     &PercTopLeftYSF1);
  createParam(PercTopLeftYSF2String,   asynParamInt32,     &PercTopLeftYSF2);
  createParam(PercTopLeftYSF3String,   asynParamInt32,     &PercTopLeftYSF3);
  createParam(PercTopLeftYSF4String,   asynParamInt32,     &PercTopLeftYSF4);
  createParam(PercTopLeftYSF5String,   asynParamInt32,     &PercTopLeftYSF5);
  createParam(PercTopLeftYSF6String,   asynParamInt32,     &PercTopLeftYSF6);
  createParam(PercTopLeftYSF7String,   asynParamInt32,     &PercTopLeftYSF7);
  createParam(PercTopLeftYSF8String,   asynParamInt32,     &PercTopLeftYSF8);
  createParam(PercBotRightXSF1String,  asynParamInt32,     &PercBotRightXSF1);
  createParam(PercBotRightXSF2String,  asynParamInt32,     &PercBotRightXSF2);
  createParam(PercBotRightXSF3String,  asynParamInt32,     &PercBotRightXSF3);
  createParam(PercBotRightXSF4String,  asynParamInt32,     &PercBotRightXSF4);
  createParam(PercBotRightXSF5String,  asynParamInt32,     &PercBotRightXSF5);
  createParam(PercBotRightXSF6String,  asynParamInt32,     &PercBotRightXSF6);
  createParam(PercBotRightXSF7String,  asynParamInt32,     &PercBotRightXSF7);
  createParam(PercBotRightXSF8String,  asynParamInt32,     &PercBotRightXSF8);
  createParam(PercBotRightYSF1String,  asynParamInt32,     &PercBotRightYSF1);
  createParam(PercBotRightYSF2String,  asynParamInt32,     &PercBotRightYSF2);
  createParam(PercBotRightYSF3String,  asynParamInt32,     &PercBotRightYSF3);
  createParam(PercBotRightYSF4String,  asynParamInt32,     &PercBotRightYSF4);
  createParam(PercBotRightYSF5String,  asynParamInt32,     &PercBotRightYSF5);
  createParam(PercBotRightYSF6String,  asynParamInt32,     &PercBotRightYSF6);
  createParam(PercBotRightYSF7String,  asynParamInt32,     &PercBotRightYSF7);
  createParam(PercBotRightYSF8String,  asynParamInt32,     &PercBotRightYSF8);

  createParam(PercWidthSF1String,      asynParamInt32,     &PercWidthSF1);
  createParam(PercWidthSF2String,      asynParamInt32,     &PercWidthSF2);
  createParam(PercWidthSF3String,      asynParamInt32,     &PercWidthSF3);
  createParam(PercWidthSF4String,      asynParamInt32,     &PercWidthSF4);
  createParam(PercWidthSF5String,      asynParamInt32,     &PercWidthSF5);
  createParam(PercWidthSF6String,      asynParamInt32,     &PercWidthSF6);
  createParam(PercWidthSF7String,      asynParamInt32,     &PercWidthSF7);
  createParam(PercWidthSF8String,      asynParamInt32,     &PercWidthSF8);
  createParam(PercHeightSF1String,     asynParamInt32,     &PercHeightSF1);
  createParam(PercHeightSF2String,     asynParamInt32,     &PercHeightSF2);
  createParam(PercHeightSF3String,     asynParamInt32,     &PercHeightSF3);
  createParam(PercHeightSF4String,     asynParamInt32,     &PercHeightSF4);
  createParam(PercHeightSF5String,     asynParamInt32,     &PercHeightSF5);
  createParam(PercHeightSF6String,     asynParamInt32,     &PercHeightSF6);
  createParam(PercHeightSF7String,     asynParamInt32,     &PercHeightSF7);
  createParam(PercHeightSF8String,     asynParamInt32,     &PercHeightSF8);

  createParam(PercSubFrame1IDString,   asynParamInt32,     &PercSubFrame1ID);
  createParam(PercSubFrame2IDString,   asynParamInt32,     &PercSubFrame2ID);
  createParam(PercSubFrame3IDString,   asynParamInt32,     &PercSubFrame3ID);
  createParam(PercSubFrame4IDString,   asynParamInt32,     &PercSubFrame4ID);
  createParam(PercSubFrame5IDString,   asynParamInt32,     &PercSubFrame5ID);
  createParam(PercSubFrame6IDString,   asynParamInt32,     &PercSubFrame6ID);
  createParam(PercSubFrame7IDString,   asynParamInt32,     &PercSubFrame7ID);
  createParam(PercSubFrame8IDString,   asynParamInt32,     &PercSubFrame8ID);

  createParam(PercReceiveString,       asynParamInt32,     &PercReceive);
  createParam(PercErrorString,         asynParamInt32,     &PercError);
  createParam(PercStatusString,        asynParamOctet,     &PercStatus);
  createParam(PercAddrString,          asynParamOctet,     &PercAddr);
  createParam(PercPortString,          asynParamInt32,     &PercPort);
  createParam(PercPacketBytesString,   asynParamInt32,     &PercPacketBytes);
  createParam(PercSubFrameString,      asynParamInt32,     &PercSubFrame);

  createParam(PercErrorDupPktString,   asynParamInt32,     &PercErrorDupPkt);
  createParam(PercErrorMisPktString,   asynParamInt32,     &PercErrorMisPkt);
  createParam(PercErrorLtePktString,   asynParamInt32,     &PercErrorLtePkt);
  createParam(PercErrorIncPktString,   asynParamInt32,     &PercErrorIncPkt);
  createParam(PercErrorDupRPktString,  asynParamInt32,     &PercErrorDupRPkt);
  createParam(PercErrorMisRPktString,  asynParamInt32,     &PercErrorMisRPkt);
  createParam(PercErrorLteRPktString,  asynParamInt32,     &PercErrorLteRPkt);
  createParam(PercErrorIncRPktString,  asynParamInt32,     &PercErrorIncRPkt);

  // Set some default values for parameters
  status =  setStringParam (ADManufacturer,      "Percival");
  status |= setStringParam (ADModel,             "TestCam");
  status |= setIntegerParam(ADMaxSizeX,          0);
  status |= setIntegerParam(ADMaxSizeY,          0);
  status |= setIntegerParam(ADSizeX,             0);
  status |= setIntegerParam(ADSizeY,             0);
  status |= setIntegerParam(NDArraySizeX,        0);
  status |= setIntegerParam(NDArraySizeY,        0);
  status |= setIntegerParam(NDArraySize,         0);
  status |= setIntegerParam(NDDataType,          NDUInt16);
  status |= setIntegerParam(ADImageMode,         ADImageContinuous);
  status |= setIntegerParam(ADAcquire,           0);
  status |= setIntegerParam(ADStatus,            ADStatusIdle);
  status |= setDoubleParam (ADAcquireTime,       .000);
  status |= setDoubleParam (ADAcquirePeriod,     .000);
  status |= setIntegerParam(ADNumImages,         100);
  status |= setStringParam (ADStatusMessage,     " ");

  status |= setIntegerParam(PercFileReadStatus,  0);
  status |= setStringParam (PercFileReadMessage, " ");
  status |= setIntegerParam(PercFileErrorStatus, 0);

  status |= setIntegerParam(PercDebugLevel,      0);
  status |= setIntegerParam(PercWatchdogTimeout, 2000);

  status |= setIntegerParam(PercChannelMode,     0);
  status |= setIntegerParam(PercDescramble,      0);

  //status |= setIntegerParam(PercEnableChannel1,     0);
  //status |= setIntegerParam(PercEnableChannel2,     0);
  //status |= setIntegerParam(PercEnableChannel3,     0);
  //status |= setIntegerParam(PercEnableChannel4,     0);

  status |= setIntegerParam(PercTopLeftXSF1,   0);
  status |= setIntegerParam(PercTopLeftXSF2,   0);
  status |= setIntegerParam(PercTopLeftXSF3,   0);
  status |= setIntegerParam(PercTopLeftXSF4,   0);
  status |= setIntegerParam(PercTopLeftXSF5,   0);
  status |= setIntegerParam(PercTopLeftXSF6,   0);
  status |= setIntegerParam(PercTopLeftXSF7,   0);
  status |= setIntegerParam(PercTopLeftXSF8,   0);
  status |= setIntegerParam(PercTopLeftYSF1,   0);
  status |= setIntegerParam(PercTopLeftYSF2,   0);
  status |= setIntegerParam(PercTopLeftYSF3,   0);
  status |= setIntegerParam(PercTopLeftYSF4,   0);
  status |= setIntegerParam(PercTopLeftYSF5,   0);
  status |= setIntegerParam(PercTopLeftYSF6,   0);
  status |= setIntegerParam(PercTopLeftYSF7,   0);
  status |= setIntegerParam(PercTopLeftYSF8,   0);
  status |= setIntegerParam(PercBotRightXSF1,  0);
  status |= setIntegerParam(PercBotRightXSF2,  0);
  status |= setIntegerParam(PercBotRightXSF3,  0);
  status |= setIntegerParam(PercBotRightXSF4,  0);
  status |= setIntegerParam(PercBotRightXSF5,  0);
  status |= setIntegerParam(PercBotRightXSF6,  0);
  status |= setIntegerParam(PercBotRightXSF7,  0);
  status |= setIntegerParam(PercBotRightXSF8,  0);
  status |= setIntegerParam(PercBotRightYSF1,  0);
  status |= setIntegerParam(PercBotRightYSF2,  0);
  status |= setIntegerParam(PercBotRightYSF3,  0);
  status |= setIntegerParam(PercBotRightYSF4,  0);
  status |= setIntegerParam(PercBotRightYSF5,  0);
  status |= setIntegerParam(PercBotRightYSF6,  0);
  status |= setIntegerParam(PercBotRightYSF7,  0);
  status |= setIntegerParam(PercBotRightYSF8,  0);

  status |= setIntegerParam(PercWidthSF1,      0);
  status |= setIntegerParam(PercWidthSF2,      0);
  status |= setIntegerParam(PercWidthSF3,      0);
  status |= setIntegerParam(PercWidthSF4,      0);
  status |= setIntegerParam(PercWidthSF5,      0);
  status |= setIntegerParam(PercWidthSF6,      0);
  status |= setIntegerParam(PercWidthSF7,      0);
  status |= setIntegerParam(PercWidthSF8,      0);
  status |= setIntegerParam(PercHeightSF1,     0);
  status |= setIntegerParam(PercHeightSF2,     0);
  status |= setIntegerParam(PercHeightSF3,     0);
  status |= setIntegerParam(PercHeightSF4,     0);
  status |= setIntegerParam(PercHeightSF5,     0);
  status |= setIntegerParam(PercHeightSF6,     0);
  status |= setIntegerParam(PercHeightSF7,     0);
  status |= setIntegerParam(PercHeightSF8,     0);

  status |= setIntegerParam(PercSubFrame1ID,   0);
  status |= setIntegerParam(PercSubFrame2ID,   0);
  status |= setIntegerParam(PercSubFrame3ID,   0);
  status |= setIntegerParam(PercSubFrame4ID,   0);
  status |= setIntegerParam(PercSubFrame5ID,   0);
  status |= setIntegerParam(PercSubFrame6ID,   0);
  status |= setIntegerParam(PercSubFrame7ID,   0);
  status |= setIntegerParam(PercSubFrame8ID,   0);

  status |= setIntegerParam(PercReceive,       0);
  status |= setIntegerParam(PercError,         0);
  status |= setStringParam (PercStatus,        "");

  status |= setStringParam (PercAddr,          "");
  status |= setIntegerParam(PercPort,          0);
  status |= setIntegerParam(PercPacketBytes,   0);
  status |= setIntegerParam(PercSubFrame,      0);

  status |= setIntegerParam(PercErrorDupPkt,   0);
  status |= setIntegerParam(PercErrorMisPkt,   0);
  status |= setIntegerParam(PercErrorLtePkt,   0);
  status |= setIntegerParam(PercErrorIncPkt,   0);
  status |= setIntegerParam(PercErrorDupRPkt,  0);
  status |= setIntegerParam(PercErrorMisRPkt,  0);
  status |= setIntegerParam(PercErrorLteRPkt,  0);
  status |= setIntegerParam(PercErrorIncRPkt,  0);

  callParamCallbacks();

  pBuffer_ = NULL;
  descrambleArray_ = 0;

  // Initialise the buffer pool pointer to 0
  buffers_ = 0;

  // Create the configuration class
  configPtr_ = new Configurator();

  // Create the Percival server class
  sPtr_ = new PercivalServer();

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

  // Start the stats task in a separate thread
	status = (epicsThreadCreate("stats_task",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)stats_task_c,
            this) == NULL);
  if (status){
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsThreadCreate failure for stats task\n", driverName, functionName);
    return;
  }

}

void ADPercivalDriver::stats_task()
{
  uint32_t dupPkt;
  uint32_t misPkt;
  uint32_t ltePkt;
  uint32_t incPkt;
  uint32_t dupRPkt;
  uint32_t misRPkt;
  uint32_t lteRPkt;
  uint32_t incRPkt;
  // Loop forever in this task
  while (1){
    sPtr_->readErrorStats(&dupPkt, &misPkt, &ltePkt, &incPkt, &dupRPkt, &misRPkt, &lteRPkt, &incRPkt);
    this->lock();
    setIntegerParam(PercErrorDupPkt, dupPkt);
    setIntegerParam(PercErrorMisPkt, misPkt);
    setIntegerParam(PercErrorLtePkt, ltePkt);
    setIntegerParam(PercErrorIncPkt, incPkt);
    setIntegerParam(PercErrorDupRPkt, dupRPkt);
    setIntegerParam(PercErrorMisRPkt, misRPkt);
    setIntegerParam(PercErrorLteRPkt, lteRPkt);
    setIntegerParam(PercErrorIncRPkt, incRPkt);
    callParamCallbacks();
    this->unlock();
    epicsThreadSleep(1.0);
  }
}

void ADPercivalDriver::imageReceived(PercivalBuffer *buffer, uint32_t bytes, uint16_t frameNumber, uint8_t subFrameNumber, uint16_t packetNumber, uint8_t packetType)
{
  static const char *functionName = "ADPercivalDriver::imageReceived";
  int status = asynSuccess;
  int imageCounter;
  int numImages, numImagesCounter;
  int imageMode;
  int arrayCallbacks;
  epicsTimeStamp imageTime;

  this->lock();
  // If we have an old buffer then its time to release
  if (pBuffer_ != NULL){
      // Release the Percival buffer back into the pool
      buffers_->free(pBuffer_);
  }
  pBuffer_ = buffer;

  // Allocate an NDArray
  if (pImage_){
    pImage_->release();
  }
  // Allocate the buffer using the read image.
  if (configPtr_->getDataType() == UnsignedInt8){
    pImage_ = this->pNDArrayPool->alloc(ndims_, dims_, NDUInt8, buffer->size(), buffer->raw());
  }else if (configPtr_->getDataType() == UnsignedInt16){
    pImage_ = this->pNDArrayPool->alloc(ndims_, dims_, NDUInt16, buffer->size(), buffer->raw());
  } else if (configPtr_->getDataType() == UnsignedInt32){
    pImage_ = this->pNDArrayPool->alloc(ndims_, dims_, NDUInt32, buffer->size(), buffer->raw());
  }
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
    // Get the current time
    epicsTimeGetCurrent(&imageTime);
    pImage_->timeStamp = imageTime.secPastEpoch + imageTime.nsec / 1.e9;
  
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


  // Read in the number of Images and counter
  getIntegerParam(ADNumImages, &numImages);
  getIntegerParam(ADNumImagesCounter, &numImagesCounter);
  // See if acquisition is complete
  if (((imageMode == ADImageSingle) && (numImagesCounter == 1)) ||
      ((imageMode == ADImageMultiple) && (numImagesCounter >= numImages))) {
    // This is a command to stop acquisition
    sPtr_->stopAcquisition();
    // Release all of the subframes
    sPtr_->releaseAllSubFrames();
    // Set the status
    //setIntegerParam(PercReceiveChannel1, 0);
    //setIntegerParam(PercReceiveChannel2, 0);
    //setIntegerParam(PercReceiveChannel3, 0);
    //setIntegerParam(PercReceiveChannel4, 0);
    setIntegerParam(ADStatus, ADStatusIdle);
    setStringParam(ADStatusMessage, "Acquisition completed");
    setIntegerParam(ADAcquire, 0);
    callParamCallbacks();

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: acquisition completed\n", driverName, functionName);
  }

  this->unlock();
}

void ADPercivalDriver::timeout()
{
  // Here we do not need to do anything, it has already been handled at the server level
  const char *functionName = "allocateBuffer";
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: timer watchdog, handled within server\n", 
            driverName, functionName);
}

PercivalBuffer *ADPercivalDriver::allocateBuffer()
{
  const char *functionName = "allocateBuffer";
  //PercivalBuffer *buffer = buffers_->allocateClean();
  PercivalBuffer *buffer = buffers_->allocate();
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, 
              "%s:%s: allocated buffer address: %ld\n", 
              driverName, functionName, (long int)buffer);
  return buffer;
}

void ADPercivalDriver::releaseBuffer(PercivalBuffer *buffer)
{
  buffers_->free(buffer);
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

  status = setIntegerParam(param, value);

  if (param == ADAcquire){
    int tlx, tly, brx, bry, subFrame;
    int subFrameID;
    char address[256];
    int port;
    int packetBytes;
    int error = 0;
    int mode = 0;
    epicsInt32 adstatus;
    getIntegerParam(ADStatus, &adstatus);
    //int error = 0;
    if (value && (adstatus == ADStatusIdle)){
      // Read the mode
      getIntegerParam(PercChannelMode, &mode);
      // If the mode is zero then we are running in spatial mode
      if (mode == 0){
        // Spatial mode only initially
        setupSpatialImage();
      
        // Get the selected subframe
        getIntegerParam(PercSubFrame, &subFrame);
      
        // Reset status message and error
        setIntegerParam(PercError, 0);
        setStringParam(PercStatus, "");

        getIntegerParam(PercSubFrame1ID+subFrame, &subFrameID);
        getIntegerParam(PercTopLeftXSF1+subFrame, &tlx);
        getIntegerParam(PercTopLeftYSF1+subFrame, &tly);
        getIntegerParam(PercBotRightXSF1+subFrame, &brx);
        getIntegerParam(PercBotRightYSF1+subFrame, &bry);
        // Perfom a few checks
        if (tlx >= brx){
          error = 1;
          setStringParam(PercStatus, "Width must be > 0");
        }
        if (tly >= bry){
          error = 1;
          setStringParam(PercStatus, "Height must be > 0");
        }
        if (error){
          setIntegerParam(PercError, 1);
        }

        if (!error){
          // Setup the channel
          getStringParam(PercAddr, sizeof(address), address);
          getIntegerParam(PercPort, &port);
          getIntegerParam(PercPacketBytes, &packetBytes);
          sPtr_->setupChannel(address, port, packetBytes);

          // Setup the subframe within the server
          sPtr_->setupSubFrame(subFrameID, tlx, tly, brx, bry);
          setIntegerParam(PercReceive, 1);
 
          // Zero the image counter
          setIntegerParam(ADNumImagesCounter, 0);
          // Tell the server to start the acquisition
          sPtr_->startAcquisition();
          setIntegerParam(ADStatus, ADStatusAcquire);
          setStringParam(ADStatusMessage, "Acquiring images");
        }
      } else if (mode == 1){
        // We are running in temporal mode
        // Spatial mode only initially
        setupTemporalImage();
      
        // Reset status message and error
        setIntegerParam(PercError, 0);
        setStringParam(PercStatus, "");

        for (subFrame = 0; subFrame < 8; subFrame++){
          getIntegerParam(PercSubFrame1ID+subFrame, &subFrameID);
          getIntegerParam(PercTopLeftXSF1+subFrame, &tlx);
          getIntegerParam(PercTopLeftYSF1+subFrame, &tly);
          getIntegerParam(PercBotRightXSF1+subFrame, &brx);
          getIntegerParam(PercBotRightYSF1+subFrame, &bry);
          // Perfom a few checks
          if (tlx >= brx){
            error = 1;
            setStringParam(PercStatus, "Sub-Frame Width must be > 0");
          }
          if (tly >= bry){
            error = 1;
            setStringParam(PercStatus, "Sub-Frame Height must be > 0");
          }
          if (error){
            setIntegerParam(PercError, 1);
          }
        }
        if (!error){
          // Setup the channel
          getStringParam(PercAddr, sizeof(address), address);
          getIntegerParam(PercPort, &port);
          getIntegerParam(PercPacketBytes, &packetBytes);
          sPtr_->setupChannel(address, port, packetBytes);

          for (subFrame = 0; subFrame < 8; subFrame++){
            getIntegerParam(PercSubFrame1ID+subFrame, &subFrameID);
            getIntegerParam(PercTopLeftXSF1+subFrame, &tlx);
            getIntegerParam(PercTopLeftYSF1+subFrame, &tly);
            getIntegerParam(PercBotRightXSF1+subFrame, &brx);
            getIntegerParam(PercBotRightYSF1+subFrame, &bry);
            // Setup each subframe within the server
            sPtr_->setupSubFrame(subFrameID, tlx, tly, brx, bry);
          }
          setIntegerParam(PercReceive, 1);
 
          // Zero the image counter
          setIntegerParam(ADNumImagesCounter, 0);
          // Tell the server to start the acquisition
          sPtr_->startAcquisition();
          setIntegerParam(ADStatus, ADStatusAcquire);
          setStringParam(ADStatusMessage, "Acquiring images");
        }
      }
/*      int channelMode;
      getIntegerParam(PercChannelMode, &channelMode);
      setupImage();
//        char address[256];
//        // Reset status msg and error
//        setIntegerParam(PercErrorChannel1+channel, 0);
//        setStringParam(PercStatusChannel1+channel, "");
      // First we must check for errors
      for (int channel = 0; channel < 4; channel++){
        int tlx, tly, brx, bry, subFrame;
        if (!error){
          // Subframe is enabled, read out parameters
          getIntegerParam(PercSubFrame1ID+channel, &subFrame);
          getIntegerParam(PercTopLeftXSF1+channel, &tlx);
          getIntegerParam(PercTopLeftYSF1+channel, &tly);
          getIntegerParam(PercBotRightXSF1+channel, &brx);
          getIntegerParam(PercBotRightYSF1+channel, &bry);
          // Perfom a few checks
          if (tlx >= brx){
            error = 1;
//            setStringParam(PercStatusChannel1+channel, "Width must be > 0");
          }
          if (tly >= bry){
            error = 1;
//            setStringParam(PercStatusChannel1+channel, "Height must be > 0");
          }
          if (subFrames <= 0){
            error = 1;
//            setStringParam(PercStatusChannel1+channel, "Subframes must be > 0");
          }
          if (error){
//            setIntegerParam(PercErrorChannel1+channel, 1);
          }
        }
      }
      if (!error){
        // Now setup each subframe
        for (int channel = 0; channel < 4; channel++){
          int tlx, tly, brx, bry, port, subFrames, enabled;
          char address[256];
          // Check to see if the subframe is enabled
          getIntegerParam(PercEnableChannel1+channel, &enabled);
          // If single channel and not channel 0 then disable
          if (channelMode == 1 && channel != 0){
            enabled = 0;
          }
          if (enabled){
            // Subframe is enabled, read out parameters
            getStringParam(PercAddrChannel1+channel, sizeof(address), address);
            getIntegerParam(PercPortChannel1+channel, &port);
            getIntegerParam(PercSubFramesChannel1+channel, &subFrames);
            getIntegerParam(PercTopLeftXChannel1+channel, &tlx);
            getIntegerParam(PercTopLeftYChannel1+channel, &tly);
            getIntegerParam(PercBotRightXChannel1+channel, &brx);
            getIntegerParam(PercBotRightYChannel1+channel, &bry);
            // Setup the subframe within the server
            sPtr_->setupSubFrame(channel, address, port, tlx, tly, brx, bry, subFrames);
            setIntegerParam(PercReceiveChannel1+channel, 1);
          }
        }
        // Zero the image counter
        setIntegerParam(ADNumImagesCounter, 0);
        // Tell the server to start the acquisition
        sPtr_->startAcquisition();
        setIntegerParam(ADStatus, ADStatusAcquire);
        setStringParam(ADStatusMessage, "Acquiring images");
      } else {
        setIntegerParam(PercReceiveChannel1, 0);
        setIntegerParam(PercReceiveChannel2, 0);
        setIntegerParam(PercReceiveChannel3, 0);
        setIntegerParam(PercReceiveChannel4, 0);
        setStringParam(ADStatusMessage, "Error in one of the channels");
      }*/
    }
    if (!value && (adstatus != ADStatusIdle)){
      // This is a command to stop acquisition
      sPtr_->stopAcquisition();
      // Release all of the subframes
      sPtr_->releaseAllSubFrames();
      // Set the status
      setIntegerParam(PercReceive, 0);
      setIntegerParam(ADStatus, ADStatusIdle);
      setStringParam(ADStatusMessage, "Acquisition completed");
    }
  } else if (param == PercConfigFileRead){
    char fileName[MAX_FILENAME_LEN];
    setIntegerParam(PercFileErrorStatus, 0);
    setIntegerParam(PercFileReadStatus, 1);
    setStringParam (PercFileReadMessage, "Reading configuration file");
    callParamCallbacks();
    status |= getStringParam(PercFullFileName, sizeof(fileName), fileName);
    status = configPtr_->readConfiguration(fileName);
    if (status){
      // Error reading the file.  Notify
      setStringParam(PercFileReadMessage, configPtr_->errorMessage().c_str());
      setIntegerParam(PercFileReadStatus, 0);
      setIntegerParam(PercFileErrorStatus, 1);
    } else {
      // Setup image dimensions
      dims_[0] = configPtr_->getImageWidth();
      dims_[1] = configPtr_->getImageHeight();
      // Read in the number of ADCs
      noOfADCs_ = configPtr_->getNumberOfADCs();

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

      // Now allocate and copy in the descramble array from the configuration file
      if (descrambleArray_){
        free(descrambleArray_);
      }
      descrambleArray_ = (uint32_t *)malloc(dims_[0] * dims_[1] * sizeof(uint32_t));
      configPtr_->copyDescrambleArray(descrambleArray_);

      // Now allocate all of the gain arrays
      if (!ADC_index_){
        free(ADC_index_);
      }
      ADC_index_ = (uint32_t *)malloc(dims_[0] * dims_[1] * sizeof(uint32_t));
      configPtr_->copyADCIndex(ADC_index_);
      
      if (ADC_low_gain_){
        free(ADC_low_gain_);
      }
      ADC_low_gain_ = (float *)malloc(noOfADCs_ * sizeof(float));
      configPtr_->copyADCLowGain(ADC_low_gain_);

      if (ADC_high_gain_){
        free(ADC_high_gain_);
      }
      ADC_high_gain_ = (float *)malloc(noOfADCs_ * sizeof(float));
      configPtr_->copyADCHighGain(ADC_high_gain_);

      if (ADC_offset_){
        free(ADC_offset_);
      }
      ADC_offset_ = (float *)malloc(noOfADCs_ * sizeof(float));
      configPtr_->copyADCOffsets(ADC_offset_);

      if (stage_gains_){
        free(stage_gains_);
      }
      stage_gains_ = (float *)malloc(4 * dims_[0] * dims_[1] * sizeof(float));
      configPtr_->copyStageGains(stage_gains_);

      if (stage_offsets_){
        free(stage_offsets_);
      }
      stage_offsets_ = (float *)malloc(4 * dims_[0] * dims_[1] * sizeof(float));
      configPtr_->copyStageOffsets(stage_offsets_);

      // Setup server for full frame as defined by configuration file
      sPtr_->setupFullFrame(dims_[0], dims_[1], configPtr_->getDataType(), descrambleArray_, ADC_index_, ADC_low_gain_, ADC_high_gain_, ADC_offset_, stage_gains_, stage_offsets_);
    }
  } else if (param == PercDebugLevel){
    setIntegerParam(PercDebugLevel, value);
    // Set the debug level within the classes
    sPtr_->setDebug(value);
  } else if (param == PercWatchdogTimeout){
    setIntegerParam(PercWatchdogTimeout, value);
    // Set the debug level within the classes
    sPtr_->setWatchdogTimeout(value);
  } else if (param == PercChannelMode){
    setIntegerParam(PercChannelMode, value);
  } else if (param == PercDescramble){
    setIntegerParam(PercDescramble, value);
    // Set the descramble flag in the server
    sPtr_->setDescramble(value);
  } else if (param >= PercTopLeftXSF1 && param <= PercBotRightYSF8){
    // First update the value
    setIntegerParam(param, value);
    // Now calculate the width and height of each channel
    for (int channel = 0; channel < 8; channel++){
      int tlx, tly, brx, bry, width, height;
      getIntegerParam(PercTopLeftXSF1+channel, &tlx);
      getIntegerParam(PercTopLeftYSF1+channel, &tly);
      getIntegerParam(PercBotRightXSF1+channel, &brx);
      getIntegerParam(PercBotRightYSF1+channel, &bry);
      width = brx - tlx + 1;
      if (width < 0) width = 0;
      height = bry - tly + 1;
      if (height < 0) height = 0;
      setIntegerParam(PercWidthSF1+channel, width);
      setIntegerParam(PercHeightSF1+channel, height);
    }
  } else if (param == PercPort){
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

void ADPercivalDriver::setupSpatialImage()
{
  // Setup server with single channel size
  int tlx, tly, brx, bry, width, height, subFrame;

  // Get the selected subframe
  getIntegerParam(PercSubFrame, &subFrame);
  getIntegerParam(PercTopLeftXSF1 + subFrame, &tlx);
  getIntegerParam(PercTopLeftYSF1 + subFrame, &tly);
  getIntegerParam(PercBotRightXSF1 + subFrame, &brx);
  getIntegerParam(PercBotRightYSF1 + subFrame, &bry);
  width  = brx - tlx + 1;
  height = bry - tly + 1;
  dims_[0] = width;
  dims_[1] = height;

  setIntegerParam(ADSizeX,      width);
  setIntegerParam(ADSizeY,      height);
  setIntegerParam(NDArraySizeX, width);
  setIntegerParam(NDArraySizeY, height);

  // Create the Percival buffer pool from the size of image loaded from the configuration file
  if (buffers_ != 0){
    delete buffers_;
  }
  buffers_ = new PercivalBufferPool(width * height * (uint32_t)pow(2.0, (double)configPtr_->getDataType()));
  setIntegerParam(NDArraySize, width * height * (uint32_t)pow(2.0, (double)configPtr_->getDataType()));
  callParamCallbacks();

  if (sfDescrambleArray_){
    free(sfDescrambleArray_);
    sfDescrambleArray_ = 0;
  }
  // We need to pass in the correct descrambleArray for the subframe in this case
  sfDescrambleArray_ = (uint32_t *)malloc(width * height * sizeof(uint32_t));
  // Loop over rows, copying in 1 row of descramble data at a time
  uint32_t *ptr1 = sfDescrambleArray_;
  for (int rows = tly; rows < (bry+1); rows++){
    uint32_t *ptr2 = descrambleArray_+(rows*configPtr_->getImageWidth())+tlx;
    memcpy(ptr1, ptr2, width*sizeof(uint32_t));
    ptr1 += width;
  }
  // Now each index in the descramble array is pointing to a point in the whole frame
  // Realign each point to a point in the subframe
  for (int x = 0; x < width; x++){
    for (int y = 0; y < height; y++){
      uint32_t raw_index = sfDescrambleArray_[(y*width)+x];
      // Find the corresponding index within the subframe
      uint32_t ffy = raw_index / configPtr_->getImageWidth();
      uint32_t ffx = raw_index - (ffy * configPtr_->getImageWidth());
      uint32_t sfy = ffy - tly;
      uint32_t sfx = ffx - tlx;
      sfDescrambleArray_[(y*width)+x] = (sfy*width)+sfx;
    }
  }
  // Setup server for partial frame as defined by selected subframe
  sPtr_->setToSpatialMode(subFrame);
  sPtr_->setupFullFrame(width, height, configPtr_->getDataType(), sfDescrambleArray_, ADC_index_, ADC_low_gain_, ADC_high_gain_, ADC_offset_, stage_gains_, stage_offsets_);
}

void ADPercivalDriver::setupTemporalImage()
{
  // Setup image dimensions from the configuration file
  dims_[0] = configPtr_->getImageWidth();
  dims_[1] = configPtr_->getImageHeight();

  setIntegerParam(ADSizeX,            dims_[0]);
  setIntegerParam(ADSizeY,            dims_[1]);
  setIntegerParam(NDArraySizeX,       dims_[0]);
  setIntegerParam(NDArraySizeY,       dims_[1]);
  setIntegerParam(NDDataType,         configPtr_->getDataType());

  // Create the Percival buffer pool from the size of image loaded from the configuration file
  if (buffers_ != 0){
    delete(buffers_);
  }
  buffers_ = new PercivalBufferPool(dims_[0] * dims_[1] * sizeof(uint16_t));
  setIntegerParam(NDArraySize, dims_[0] * dims_[1] * sizeof(uint16_t));
  callParamCallbacks();

  // Setup server for full frame as defined by configuration
  sPtr_->setToTemporalMode();
  sPtr_->setupFullFrame(dims_[0], dims_[1], configPtr_->getDataType(), descrambleArray_, ADC_index_, ADC_low_gain_, ADC_high_gain_, ADC_offset_, stage_gains_, stage_offsets_);
}

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
                         1024288000);
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
