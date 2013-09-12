/*
 * asynConfiguratorDriver.cpp
 *
 *  Created on: 6th Aug 2013
 *      Author: gnx91527
 */

#include <string.h>
#include <sys/stat.h>
#include <iocsh.h>
#include <epicsExport.h>

#include "asynConfiguratorDriver.h"

static const char *driverName = "asynConfiguratorDriver";

/** Called when asyn clients call pasynOctet->write().
  * This function performs actions for some parameters, including AttributesFile.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Address of the string to write.
  * \param[in] nChars Number of characters to write.
  * \param[out] nActual Number of characters actually written. */
asynStatus asynConfiguratorDriver::writeOctet(asynUser *pasynUser, const char *value, size_t nChars, size_t *nActual)
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

asynStatus asynConfiguratorDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int function = pasynUser->reason;
  int status = asynSuccess;

  if (function == ImagePatternX){
    configPtr->setRepeatX(value);
  } else if (function == ImagePatternY){
    configPtr->setRepeatY(value);
  } else if (function == ImagePatternType){
    switch (value){
      case 0:
        configPtr->setPattern(rectangle);
        break;
      case 1:
        configPtr->setPattern(triangle);
        break;
      case 2:
        configPtr->setPattern(ellipse);
        break;
    }
  } else if (function == DPixelsPerChipX){
    configPtr->setPixelsPerChipX(value);
    setIntegerParam(ImageSizeX, configPtr->getImageWidth());
  } else if (function == DChipsPerBlockX){
    configPtr->setChipsPerBlockX(value);
    setIntegerParam(ImageSizeX, configPtr->getImageWidth());
    setIntegerParam(DChipsPerStripeX,  configPtr->getChipsPerStripeX());
  } else if (function == DBlocksPerStripeX){
    configPtr->setBlocksPerStripeX(value);
    setIntegerParam(ImageSizeX, configPtr->getImageWidth());
    setIntegerParam(DChipsPerStripeX,  configPtr->getChipsPerStripeX());
  } else if (function == DStripesPerImageX){
    configPtr->setStripesPerImageX(value);
    setIntegerParam(ImageSizeX, configPtr->getImageWidth());
    setIntegerParam(DChipsPerStripeX,  configPtr->getChipsPerStripeX());
  } else if (function == DPixelsPerChipY){
    configPtr->setPixelsPerChipY(value);
    setIntegerParam(ImageSizeY, configPtr->getImageHeight());
  } else if (function == DChipsPerBlockY){
    configPtr->setChipsPerBlockY(value);
    setIntegerParam(ImageSizeY, configPtr->getImageHeight());
    setIntegerParam(DChipsPerStripeY,  configPtr->getChipsPerStripeY());
  } else if (function == DBlocksPerStripeY){
    configPtr->setBlocksPerStripeY(value);
    setIntegerParam(ImageSizeY, configPtr->getImageHeight());
    setIntegerParam(DChipsPerStripeX,  configPtr->getChipsPerStripeY());
  } else if (function == DStripesPerImageY){
    configPtr->setStripesPerImageY(value);
    setIntegerParam(ImageSizeY, configPtr->getImageHeight());
    setIntegerParam(DChipsPerStripeY,  configPtr->getChipsPerStripeY());
  } else if (function == RawFileWrite){
    char fileName[MAX_FILENAME_LEN];
    setIntegerParam(FileWriteStatus, 1);
    setStringParam (FileWriteMessage, "Generating raw image");
    callParamCallbacks();
    status |= getStringParam(FullFileName, sizeof(fileName), fileName);
    configPtr->generateConfiguration(fileName);
    setStringParam(FileWriteMessage, "Raw image complete");
    setIntegerParam(FileWriteStatus, 0);
  } else if (function == ImageScrambleType){
    switch (value){
      case 0:
        configPtr->setScrambleType(excalibur);
        break;
      case 1:
        configPtr->setScrambleType(percival);
        break;
    }
  } else if (function == DataType){
    switch (value){
      case UInt8:
        configPtr->setDataType(UnsignedInt8);
        break;
      case UInt16:
        configPtr->setDataType(UnsignedInt16);
        break;
      case UInt32:
        configPtr->setDataType(UnsignedInt32);
        break;
    }
  } else if (function == ImageCount){
    configPtr->setNoOfImages(value);
  } else if (function == ImageMinValue){
    configPtr->setMinValue(value);
  } else if (function == ImageMaxValue){
    if (value > 8191){
      value = 8191;
      setStringParam(FileWriteMessage, "Maximum allowed value is 8191");
    }
    configPtr->setMaxValue(value);
  }

  // Call base class
  status = asynPortDriver::writeInt32(pasynUser, value);
  return (asynStatus)status;
}

asynStatus asynConfiguratorDriver::readInt32(asynUser *pasynUser, epicsInt32 *value)
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
asynStatus asynConfiguratorDriver::readFloat64(asynUser *pasynUser, epicsFloat64 *value)
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
void asynConfiguratorDriver::report(FILE *fp, int details)
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
int asynConfiguratorDriver::checkPath()
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
int asynConfiguratorDriver::createFileName(int maxChars, char *fullFileName)
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
int asynConfiguratorDriver::createFileName(int maxChars, char *filePath, char *fileName)
{
  // Formats a complete file name from the components defined in NDStdDriverParams
  int status = asynSuccess;
  char name[MAX_FILENAME_LEN];
    
  this->checkPath();
  status |= getStringParam(FilePath, maxChars, filePath); 
  status |= getStringParam(FileName, sizeof(name), fileName); 
  return(status);   
}

/** This is the constructor for the asynConfiguratorDriver class.
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

asynConfiguratorDriver::asynConfiguratorDriver(const char *portName,
                                               int maxAddr,
                                               int interfaceMask,
                                               int interruptMask,
                                               int asynFlags,
                                               int autoConnect,
                                               int priority,
                                               int stackSize)
    : asynPortDriver(portName,
                     maxAddr,
                     NUM_CONFIGURATOR_PARAMS,
                     interfaceMask,
                     interruptMask,
                     asynFlags,
                     autoConnect,
                     priority,
                     stackSize)
{
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
  createParam(ImageCountString,           asynParamInt32,           &ImageCount);
  createParam(ImageMinValueString,        asynParamInt32,           &ImageMinValue);
  createParam(ImageMaxValueString,        asynParamInt32,           &ImageMaxValue);
  createParam(FilePathString,             asynParamOctet,           &FilePath);
  createParam(FilePathExistsString,       asynParamInt32,           &FilePathExists);
  createParam(FileNameString,             asynParamOctet,           &FileName);
  createParam(FullFileNameString,         asynParamOctet,           &FullFileName);
  createParam(RawFileWriteString,         asynParamInt32,           &RawFileWrite);
  createParam(FileWriteStatusString,      asynParamInt32,           &FileWriteStatus);
  createParam(FileWriteMessageString,     asynParamOctet,           &FileWriteMessage);
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
  createParam(DNumberOfADCsString,        asynParamInt32,           &DNumberOfADCs);
  createParam(DGainThreshold1String,      asynParamInt32,           &DGainThreshold1);
  createParam(DGainThreshold2String,      asynParamInt32,           &DGainThreshold2);
  createParam(DGainThreshold3String,      asynParamInt32,           &DGainThreshold3);
  createParam(DGainThreshold4String,      asynParamInt32,           &DGainThreshold4);
  createParam(ImageScrambleTypeString,    asynParamInt32,           &ImageScrambleType);

  // Create the configurator object
  configPtr = new Configurator();

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
  setIntegerParam(DataType,          UInt32);
  setIntegerParam(ImageCount,        configPtr->getNoOfImages());
  setIntegerParam(ImageMinValue,     configPtr->getMinValue());
  setIntegerParam(ImageMaxValue,     configPtr->getMaxValue());
  setIntegerParam(FileWriteStatus,   0);
  setStringParam (FilePath,          "");
  setStringParam (FileName,          "");
  setStringParam (FullFileName,      "");
  setStringParam (FileWriteMessage,  "");
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
  setIntegerParam(DNumberOfADCs,     configPtr->getNumberOfADCs());
  setIntegerParam(DGainThreshold1,   configPtr->getGainThreshold(0));
  setIntegerParam(DGainThreshold2,   configPtr->getGainThreshold(1));
  setIntegerParam(DGainThreshold3,   configPtr->getGainThreshold(2));
  setIntegerParam(DGainThreshold4,   configPtr->getGainThreshold(3));

//  FILE* fp = stdout;
//  reportParams(fp, 3);
//  callParamCallbacks();
}

// Configuration command
extern "C" int asynConfiguratorDriverConfigure(const char *portName)
{
    new asynConfiguratorDriver(portName,
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
static const iocshFuncDef initFuncDef = {"ConfiguratorInit",1,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
  asynConfiguratorDriverConfigure(args[0].sval);
}

extern "C" void asynConfiguratorDriverRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
epicsExportRegistrar(asynConfiguratorDriverRegister);
}


