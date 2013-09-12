/*
 * Configurator.cpp
 *
 *  Created on: 6th Aug 2013
 *      Author: gnx91527
 */

#include "Configurator.h"
#include "PercivalDebug.h"
#include <iostream>
#include <cstring>

Configurator::Configurator()
  : imageWidth_(256),
    imageHeight_(256),
    repeatX_(256),
    repeatY_(256),
    pattern_(rectangle),
    noOfImages_(10),
    minValue_(0),
    maxValue_(1000),
    dataType_(UnsignedInt16),
    pixelsPerChipX_(128),
    pixelsPerChipY_(128),
    chipsPerBlockX_(2),
    chipsPerBlockY_(2),
    blocksPerStripeX_(1),
    blocksPerStripeY_(1),
    chipsPerStripeX_(2),
    chipsPerStripeY_(2),
    stripesPerImageX_(1),
    stripesPerImageY_(1),
    scramble_(percival),
    arraysAllocated_(false),
    noOfADCs_(32),
    debug_(0),
    errorMessage_("")
{
  gainThresholds_[0] = 200;
  gainThresholds_[1] = 2500;
  gainThresholds_[2] = 5000;
  gainThresholds_[3] = 8000;
}

Configurator::~Configurator()
{
  PercivalDebug dbg(debug_, "Configurator::~Configurator");
}

std::string Configurator::errorMessage()
{
  PercivalDebug dbg(debug_, "Configurator::errorMessage");
  dbg.log(1, "Message", errorMessage_);
  return errorMessage_;
}

void Configurator::setDebugLevel(uint32_t level)
{
  PercivalDebug dbg(debug_, "Configurator::setDebugLevel");
  dbg.log(1, "Debug Level", level);
  debug_ = level;
}

uint32_t Configurator::getImageWidth()
{
  PercivalDebug dbg(debug_, "Configurator::getImageWidth");
  // Return the current image width
  return imageWidth_;
}

uint32_t Configurator::getImageHeight()
{
  PercivalDebug dbg(debug_, "Configurator::getImageHeight");
  // Return the current image height
  return imageHeight_;
}

void Configurator::setRepeatX(uint32_t repeatX)
{
  PercivalDebug dbg(debug_, "Configurator::setRepeatX");
  // Set the pattern repeat in x (pixels)
  repeatX_ = repeatX;
}

uint32_t Configurator::getRepeatX()
{
  PercivalDebug dbg(debug_, "Configurator::getRepeatX");
  // Return the pattern repeat in x
  return repeatX_;
}

void Configurator::setRepeatY(uint32_t repeatY)
{
  PercivalDebug dbg(debug_, "Configurator::setRepeatY");
  // Set the pattern repeat in y (pixels)
  repeatY_ = repeatY;
}

uint32_t Configurator::getRepeatY()
{
  PercivalDebug dbg(debug_, "Configurator::getRepeatY");
  // Return the pattern repeat in y
  return repeatY_;
}

void Configurator::setPattern(PatternType pattern)
{
  PercivalDebug dbg(debug_, "Configurator::setPattern");
  // Set the pattern type for the image
  pattern_ = pattern;
}

PatternType Configurator::getPattern()
{
  PercivalDebug dbg(debug_, "Configurator::getPattern");
  // Return the pattern type
  return pattern_;
}

void Configurator::setNoOfImages(uint32_t noOfImages)
{
  PercivalDebug dbg(debug_, "Configurator::setNoOfImages");
  // Set the number of images in the sequence
  noOfImages_ = noOfImages;
}

uint32_t Configurator::getNoOfImages()
{
  PercivalDebug dbg(debug_, "Configurator::getNoOfImages");
  // Return the number of images in the sequence
  return noOfImages_;
}

void Configurator::setMinValue(uint32_t minValue)
{
  PercivalDebug dbg(debug_, "Configurator::setMinValue");
  // Set the minimum value for images
  minValue_ = minValue;
}

uint32_t Configurator::getMinValue()
{
  PercivalDebug dbg(debug_, "Configurator::getMinValue");
  // Return the minimum value for images
  return minValue_;
}

void Configurator::setMaxValue(uint32_t maxValue)
{
  PercivalDebug dbg(debug_, "Configurator::setMaxValue");
  // Set the maximum value for images
  maxValue_ = maxValue;
}

uint32_t Configurator::getMaxValue()
{
  PercivalDebug dbg(debug_, "Configurator::getMaxValue");
  // Return the maximum value for images
  return maxValue_;
}

void Configurator::setDataType(DataType dataType)
{
  PercivalDebug dbg(debug_, "Configurator::setDataType");
  // Set the data type for the images
  dataType_ = dataType;
}

DataType Configurator::getDataType()
{
  PercivalDebug dbg(debug_, "Configurator::getDataType");
  // Return the data type of the images
  return dataType_;
}

void Configurator::setPixelsPerChipX(uint32_t ppcx)
{
  PercivalDebug dbg(debug_, "Configurator::setPixelsPerChipX");
  // Set the number of pixels per chip in the x direction
  pixelsPerChipX_ = ppcx;
  imageWidth_ = pixelsPerChipX_ * chipsPerStripeX_ * stripesPerImageX_;
}

uint32_t Configurator::getPixelsPerChipX()
{
  PercivalDebug dbg(debug_, "Configurator::getPixelsPerChipX");
  // Return the number of pixels per chip in the x direction
  return pixelsPerChipX_;
}

void Configurator::setPixelsPerChipY(uint32_t ppcy)
{
  PercivalDebug dbg(debug_, "Configurator::setPixelsPerChipY");
  // Set the number of pixels per chip in the y direction
  pixelsPerChipY_ = ppcy;
  imageHeight_ = pixelsPerChipY_ * chipsPerStripeY_ * stripesPerImageY_;
}

uint32_t Configurator::getPixelsPerChipY()
{
  PercivalDebug dbg(debug_, "Configurator::getPixelsPerChipY");
  // Return the number of pixels per chip in the y direction
  return pixelsPerChipY_;
}

void Configurator::setChipsPerBlockX(uint32_t cpbx)
{
  PercivalDebug dbg(debug_, "Configurator::setChipsPerBlockX");
  // Set the number of chips per block in the x direction
  chipsPerBlockX_ = cpbx;
  // Set the number of chips per stripe in the x direction
  chipsPerStripeX_ = chipsPerBlockX_ * blocksPerStripeX_;
  imageWidth_ = pixelsPerChipX_ * chipsPerStripeX_ * stripesPerImageX_;
}

uint32_t Configurator::getChipsPerBlockX()
{
  PercivalDebug dbg(debug_, "Configurator::getChipsPerBlockX");
  // Return the number of chips per block in the x direction
  return chipsPerBlockX_;
}

void Configurator::setChipsPerBlockY(uint32_t cpby)
{
  PercivalDebug dbg(debug_, "Configurator::setChipsPerBlockY");
  // Set the number of chips per block in the y direction
  chipsPerBlockY_ = cpby;
  // Set the number of chips per stripe in the y direction
  chipsPerStripeY_ = chipsPerBlockY_ * blocksPerStripeY_;
  imageHeight_ = pixelsPerChipY_ * chipsPerStripeY_ * stripesPerImageY_;
}

uint32_t Configurator::getChipsPerBlockY()
{
  PercivalDebug dbg(debug_, "Configurator::getChipsPerBlockY");
  // Return the number of chips per block in the x direction
  return chipsPerBlockY_;
}

void Configurator::setBlocksPerStripeX(uint32_t bpsx)
{
  PercivalDebug dbg(debug_, "Configurator::setBlocksPerStripeX");
  // Set the number of blocks per stripe in the x direction
  blocksPerStripeX_ = bpsx;
  // Set the number of chips per stripe in the x direction
  chipsPerStripeX_ = chipsPerBlockX_ * blocksPerStripeX_;
  imageWidth_ = pixelsPerChipX_ * chipsPerStripeX_ * stripesPerImageX_;
}

uint32_t Configurator::getBlocksPerStripeX()
{
  PercivalDebug dbg(debug_, "Configurator::getBlocksPerStripeX");
  // Return the number of blocks per stripe in the x direction
  return blocksPerStripeX_;
}

void Configurator::setBlocksPerStripeY(uint32_t bpsy)
{
  PercivalDebug dbg(debug_, "Configurator::setBlocksPerStripeY");
  // Set the number of blocks per stripe in the y direction
  blocksPerStripeY_ = bpsy;
  // Set the number of chips per stripe in the y direction
  chipsPerStripeY_ = chipsPerBlockY_ * blocksPerStripeY_;
  imageHeight_ = pixelsPerChipY_ * chipsPerStripeY_ * stripesPerImageY_;
}

uint32_t Configurator::getBlocksPerStripeY()
{
  PercivalDebug dbg(debug_, "Configurator::getBlocksPerStripeY");
  // Return the number of blocks per stripe in the y direction
  return blocksPerStripeY_;
}

uint32_t Configurator::getChipsPerStripeX()
{
  PercivalDebug dbg(debug_, "Configurator::getChipsPerStripeX");
  // Return the number of chips per stripe in the x direction
  return chipsPerStripeX_;
}

uint32_t Configurator::getChipsPerStripeY()
{
  PercivalDebug dbg(debug_, "Configurator::getChipsPerStripeY");
  // Return the number of chips per stripe in the y direction
  return chipsPerStripeY_;
}

void Configurator::setStripesPerImageX(uint32_t spix)
{
  PercivalDebug dbg(debug_, "Configurator::setStripesPerImageX");
  // Set the number of stipes per image in the x direction
  stripesPerImageX_ = spix;
  imageWidth_ = pixelsPerChipX_ * chipsPerStripeX_ * stripesPerImageX_;
}

uint32_t Configurator::getStripesPerImageX()
{
  PercivalDebug dbg(debug_, "Configurator::getStripesPerImageX");
  // Return the number of stripes per image in the x direction
  return stripesPerImageX_;
}

void Configurator::setStripesPerImageY(uint32_t spiy)
{
  PercivalDebug dbg(debug_, "Configurator::setStripesPerImageY");
  // Set the number of stripes per image in the y direction
  stripesPerImageY_ = spiy;
  imageHeight_ = pixelsPerChipY_ * chipsPerStripeY_ * stripesPerImageY_;
}

uint32_t Configurator::getStripesPerImageY()
{
  PercivalDebug dbg(debug_, "Configurator::getStripesPerImageY");
  // Return the number of stripes per image in the y direction
  return stripesPerImageY_;
}

void Configurator::setScrambleType(ScrambleType scramble)
{
  PercivalDebug dbg(debug_, "Configurator::setScrambleType");
  // Set the scramble type for the image
  scramble_ = scramble;
}

ScrambleType Configurator::getScrambleType()
{
  PercivalDebug dbg(debug_, "Configurator::getScrambleType");
  // Return the scramble type
  return scramble_;
}

void Configurator::setNumberOfADCs(uint32_t noOfADCs)
{
  PercivalDebug dbg(debug_, "Configurator::setNumberOfADCs");
  noOfADCs_ = noOfADCs;
}

uint32_t Configurator::getNumberOfADCs()
{
  PercivalDebug dbg(debug_, "Configurator::getNumberOfADCs");
  return noOfADCs_;
}

void Configurator::setGainThreshold(uint32_t number, uint32_t threshold)
{
  PercivalDebug dbg(debug_, "Configurator::setGainThreshold");
  gainThresholds_[number] = threshold;
}

uint32_t Configurator::getGainThreshold(uint32_t number)
{
  PercivalDebug dbg(debug_, "Configurator::getGainThreshold");
  return gainThresholds_[number];
}

int Configurator::openHDF5File(const std::string& filename)
{
  PercivalDebug dbg(debug_, "Configurator::openHDF5File");

  herr_t status;
  // First close the file if it is already open
  if (file_id_){
    status = H5Fclose(file_id_);
  }

  // Now open the file by filename
  file_id_ = H5Fopen(filename.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  if (file_id_ < 0){
    dbg.log(0, "Unable to open file", filename);
    errorMessage_ = "Unable to open HDF5 file";
    file_id_ = 0;
    return -1;
  }
  return 0;
}

void Configurator::createHDF5File(const std::string& filename)
{
  PercivalDebug dbg(debug_, "Configurator::createHDF5File");

  herr_t status;
  // First close the file if it is already open
  if (file_id_){
    status = H5Fclose(file_id_);
  }

  // Now create (or open and truncate) the file by filename
  file_id_ = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (file_id_ < 0){
    dbg.log(0, "Unable to create file", filename);
    file_id_ = 0;
  }
}

int Configurator::closeHDF5File()
{
  PercivalDebug dbg(debug_, "Configurator::closeHDF5File");

  herr_t status;
  // Close the file if one is open
  if (file_id_){
    status = H5Fclose(file_id_);
    file_id_ = 0;
  }
  return 0;
}

int Configurator::readConfiguration(const std::string& filename)
{
  PercivalDebug dbg(debug_, "Configurator::readConfiguration");
  int status = 0;
  dbg.log(2, "Filename", filename);

  // Create a new HDF5 configuration file using the default properties
  status = openHDF5File(filename);
  if (status){
    return status;
  }

  // Read in the meta data
  status = readMetaData();
  if (status){
    return status;
  }

  // Read in the scrambled image
  status = readScrambledImage();
  if (status){
    return status;
  }

  // Close the configuration file
  status = closeHDF5File();
  if (status){
    return status;
  }

  return 0;
}

void Configurator::generateConfiguration(const std::string& filename)
{
  PercivalDebug dbg(debug_, "Configurator::generateConfiguration");

  // Create a new HDF5 configuration file using the default properties
  createHDF5File(filename);

  // Generate the raw image and store the data set
  generateRawImage();

  generateScrambledImage();

  generateMetaData();

  // Close the configuration file
  closeHDF5File();
  
}

void Configurator::generateRawImage()
{
  PercivalDebug dbg(debug_, "Configurator::generateRawImage");

  // Create the hdf5 file
  hid_t    dataset_id = 0, dataspace_id;
  hsize_t  dims[3];
  herr_t   status;
  uint32_t xIndex, yIndex, imageNo;

  // Create the data space for the dataset.
  dims[0] = noOfImages_;
  dims[1] = imageHeight_; 
  dims[2] = imageWidth_;
  dataspace_id = H5Screate_simple(3, dims, NULL);

  // Create the dataset.
  if (dataType_ == UnsignedInt32){
    dataset_id = H5Dcreate2(file_id_, "/raw_image", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  } else if (dataType_ == UnsignedInt16){
    dataset_id = H5Dcreate2(file_id_, "/raw_image", H5T_STD_U16LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  } else if (dataType_ == UnsignedInt8){
    dataset_id = H5Dcreate2(file_id_, "/raw_image", H5T_STD_U8LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  }

  // Create the data set according to the specified pattern.
  // Currently create each data type set, not the most efficient but configurator doesn't require efficiency
  rawDataUInt32_ = (uint32_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint32_t));
  rawDataUInt16_ = (uint16_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint16_t));
  rawDataUInt8_ = (uint8_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint8_t));
  for (imageNo = 0; imageNo < noOfImages_; imageNo++){
    for (yIndex = 0; yIndex < imageHeight_; yIndex++){
      for (xIndex = 0; xIndex < imageWidth_; xIndex++){
        if (pattern_ == rectangle){
          if ((xIndex / repeatX_) % 2 == 0){
            if ((yIndex / repeatY_) % 2 == 0){
              rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)maxValue_;
              rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)maxValue_;
              rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] =  (uint8_t)maxValue_;
            } else {
              if (xIndex % repeatX_ < 10 && yIndex % repeatY_ < 10){
                rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
                rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
                rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
              } else {
                rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)minValue_;
                rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)minValue_;
                rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)minValue_;
              }
            }
          } else {
            if ((yIndex / repeatY_) % 2 == 0){
              if (xIndex % repeatX_ < 10 && yIndex % repeatY_ < 10){
                rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
                rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
                rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
              } else {
                rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)minValue_;
                rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)minValue_;
                rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)minValue_;
              }
            } else {
              rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)maxValue_;
              rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)maxValue_;
              rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)maxValue_;
            }
          }
        } else if (pattern_ == triangle){
          double div = (double)repeatY_ / (double)repeatX_;
          if ((double)(yIndex % repeatY_) > (double)(xIndex % repeatX_) * div){
            rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)maxValue_;
            rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)maxValue_;
            rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)maxValue_;
          } else {
            if (xIndex % repeatX_ < 10 && yIndex % repeatY_ < 10){
              rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
              rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
              rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
            } else {
              rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)minValue_;
              rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)minValue_;
              rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)minValue_;
            }
          }
        } else if (pattern_ == ellipse){
          double a = (double)repeatX_ / 2.0;
          double b = (double)repeatY_ / 2.0;
          double x = (double)(xIndex % repeatX_)-a;
          double y = (double)(yIndex % repeatY_)-b;
          if (y*y > (1.0 - (x*x)/(a*a))*b*b){
            rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)maxValue_;
            rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)maxValue_;
            rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)maxValue_;
          } else {
            if ((xIndex % repeatX_ < repeatX_/2 + 6) &&
                (xIndex % repeatX_ > repeatX_/2 - 6) &&
                (yIndex % repeatY_ < repeatY_/2 + 6) &&
                (yIndex % repeatY_ > repeatY_/2 - 6)){
              rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
              rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
              rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)((maxValue_ - minValue_) * imageNo / noOfImages_);
            } else {
              rawDataUInt32_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint32_t)minValue_;
              rawDataUInt16_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint16_t)minValue_;
              rawDataUInt8_[yIndex * imageWidth_ + xIndex + (imageNo * imageWidth_ * imageHeight_)] = (uint8_t)minValue_;
            }
          }
        }
      }
    }
  }

  // Write the dataset.
  if (dataType_ == UnsignedInt32){
    status = H5Dwrite(dataset_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, rawDataUInt32_);
  } else if (dataType_ == UnsignedInt16){
    status = H5Dwrite(dataset_id, H5T_STD_U16LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, rawDataUInt16_);
  } else if (dataType_ == UnsignedInt8){
    status = H5Dwrite(dataset_id, H5T_STD_U8LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, rawDataUInt8_);
  }

  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);

  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Free the data set
  //free(rawData_);
}


void Configurator::generateScrambledImage()
{
  PercivalDebug dbg(debug_, "Configurator::generateScrambledImage");

  // Create the hdf5 file
  hid_t    dataset_id = 0, dataspace_id;
  hsize_t  dims[3];
  hid_t    dataset_descramble_id = 0, dataspace_descramble_id;
  hsize_t  descramble_dims[1];
  // Gain Thresholds
  hid_t    dataset_gain_thresholds_id = 0, dataspace_gain_thresholds_id;
  hsize_t  gain_thresholds_dims[1];
  // ADC indexes
  hid_t    dataset_ADC_index_id = 0, dataspace_ADC_index_id;
  hsize_t  ADC_index_dims[1];
  // ADC low gain
  hid_t    dataset_ADC_low_gain_id = 0, dataspace_ADC_low_gain_id;
  hsize_t  ADC_low_gain_dims[1];
  // ADC high gain
  hid_t    dataset_ADC_high_gain_id = 0, dataspace_ADC_high_gain_id;
  hsize_t  ADC_high_gain_dims[1];
  // ADC offset
  hid_t    dataset_ADC_offset_id = 0, dataspace_ADC_offset_id;
  hsize_t  ADC_offset_dims[1];
  // stage gains
  hid_t    dataset_stage_gains_id = 0, dataspace_stage_gains_id;
  hsize_t  stage_gains_dims[2];
  // stage offsets
  hid_t    dataset_stage_offsets_id = 0, dataspace_stage_offsets_id;
  hsize_t  stage_offsets_dims[2];
  // Status
  herr_t   status;

  // First allocate and intialize gain arrays
  ADC_index_ = (uint32_t *)malloc(imageWidth_ * imageHeight_ * sizeof(uint32_t));
  for (uint32_t index = 0; index < (imageWidth_*imageHeight_); index++){
    ADC_index_[index] = index % noOfADCs_;
  }
  ADC_low_gain_  = (float *)malloc(noOfADCs_ * sizeof(float));
  ADC_high_gain_ = (float *)malloc(noOfADCs_ * sizeof(float));
  ADC_offset_    = (float *)malloc(noOfADCs_ * sizeof(float));
  for (uint32_t index = 0; index < noOfADCs_; index++){
    ADC_low_gain_[index]  = 1.0;
    ADC_high_gain_[index] = 1.0;
    ADC_offset_[index]    = 0.0;
  }
  stage_gains_ = (float *)malloc(4 * imageWidth_ * imageHeight_ * sizeof(float));
  stage_offsets_ = (float *)malloc(4 * imageWidth_ * imageHeight_ * sizeof(float));
  for (uint32_t index = 0; index < (imageWidth_*imageHeight_); index++){
    for (uint32_t threshold = 0; threshold < 4; threshold++){
      stage_gains_[(threshold * imageWidth_ * imageHeight_) + index] = 1.0;
      stage_offsets_[(threshold * imageWidth_ * imageHeight_) + index] = 0.0;
    }
  }
  // Create the data space, dataset and write out the thresholds 
  gain_thresholds_dims[0] = 4;
  dataspace_gain_thresholds_id = H5Screate_simple(1, gain_thresholds_dims, NULL);
  dataset_gain_thresholds_id = H5Dcreate2(file_id_, "/gain_thresholds", H5T_STD_U32LE, dataspace_gain_thresholds_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Dwrite(dataset_gain_thresholds_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, gainThresholds_);
  status = H5Dclose(dataset_gain_thresholds_id);
  status = H5Sclose(dataspace_gain_thresholds_id);
  // Create the data space, dataset and write out the ADC indexes
  ADC_index_dims[0] = imageWidth_ * imageHeight_;
  dataspace_ADC_index_id = H5Screate_simple(1, ADC_index_dims, NULL);
  dataset_ADC_index_id = H5Dcreate2(file_id_, "/ADC_index", H5T_STD_U32LE, dataspace_ADC_index_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Dwrite(dataset_ADC_index_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ADC_index_);
  status = H5Dclose(dataset_ADC_index_id);
  status = H5Sclose(dataspace_ADC_index_id);
  // Create the data space, dataset and write out the ADC low gain
  ADC_low_gain_dims[0] = noOfADCs_;
  dataspace_ADC_low_gain_id = H5Screate_simple(1, ADC_low_gain_dims, NULL);
  dataset_ADC_low_gain_id = H5Dcreate2(file_id_, "/ADC_low_gain", H5T_IEEE_F32LE, dataspace_ADC_low_gain_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Dwrite(dataset_ADC_low_gain_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ADC_low_gain_);
  status = H5Dclose(dataset_ADC_low_gain_id);
  status = H5Sclose(dataspace_ADC_low_gain_id);
  // Create the data space, dataset and write out the ADC high gain
  ADC_high_gain_dims[0] = noOfADCs_;
  dataspace_ADC_high_gain_id = H5Screate_simple(1, ADC_high_gain_dims, NULL);
  dataset_ADC_high_gain_id = H5Dcreate2(file_id_, "/ADC_high_gain", H5T_IEEE_F32LE, dataspace_ADC_high_gain_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Dwrite(dataset_ADC_high_gain_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ADC_high_gain_);
  status = H5Dclose(dataset_ADC_high_gain_id);
  status = H5Sclose(dataspace_ADC_high_gain_id);
  // Create the data space, dataset and write out the ADC offset
  ADC_offset_dims[0] = noOfADCs_;
  dataspace_ADC_offset_id = H5Screate_simple(1, ADC_offset_dims, NULL);
  dataset_ADC_offset_id = H5Dcreate2(file_id_, "/ADC_offset", H5T_IEEE_F32LE, dataspace_ADC_offset_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Dwrite(dataset_ADC_offset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ADC_offset_);
  status = H5Dclose(dataset_ADC_offset_id);
  status = H5Sclose(dataspace_ADC_offset_id);
  // Create the data space, dataset and write out the stage gains
  stage_gains_dims[0] = 4;
  stage_gains_dims[1] = imageWidth_ * imageHeight_;
  dataspace_stage_gains_id = H5Screate_simple(2, stage_gains_dims, NULL);
  dataset_stage_gains_id = H5Dcreate2(file_id_, "/stage_gains", H5T_IEEE_F32LE, dataspace_stage_gains_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Dwrite(dataset_stage_gains_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, stage_gains_);
  status = H5Dclose(dataset_stage_gains_id);
  status = H5Sclose(dataspace_stage_gains_id);
  // Create the data space, dataset and write out the stage offsets
  stage_offsets_dims[0] = 4;
  stage_offsets_dims[1] = imageWidth_ * imageHeight_;
  dataspace_stage_offsets_id = H5Screate_simple(2, stage_offsets_dims, NULL);
  dataset_stage_offsets_id = H5Dcreate2(file_id_, "/stage_offsets", H5T_IEEE_F32LE, dataspace_stage_offsets_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Dwrite(dataset_stage_offsets_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, stage_offsets_);
  status = H5Dclose(dataset_stage_offsets_id);
  status = H5Sclose(dataspace_stage_offsets_id);


  // Create the data space for the dataset.
  dims[0] = noOfImages_;
  dims[1] = imageHeight_;
  dims[2] = imageWidth_;
  dataspace_id = H5Screate_simple(3, dims, NULL);

  // Create the dataset.
  if (dataType_ == UnsignedInt32){
    dataset_id = H5Dcreate2(file_id_, "/scrambled_image", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  } else if (dataType_ == UnsignedInt16){
    dataset_id = H5Dcreate2(file_id_, "/scrambled_image", H5T_STD_U16LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  } else if (dataType_ == UnsignedInt8){
    dataset_id = H5Dcreate2(file_id_, "/scrambled_image", H5T_STD_U8LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  }

  // Create the data space for the de-scrambling array
  descramble_dims[0] = imageHeight_ * imageWidth_;
  dataspace_descramble_id = H5Screate_simple(1, descramble_dims, NULL);

  // Create the dataset.
  dataset_descramble_id = H5Dcreate2(file_id_, "/descramble_array", H5T_STD_U32LE, dataspace_descramble_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  // Allocate the memory required for the descrambling array
  descrambleArray_ = (uint32_t *)malloc(imageWidth_ * imageHeight_ * sizeof(uint32_t));

  // Allocate the memory required for the scrambled image
  scrambledDataUInt32_ = (uint32_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint32_t));
  scrambledDataUInt16_ = (uint16_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint16_t));
  scrambledDataUInt8_ = (uint8_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint8_t));
  uint32_t pixelsPerStripe = pixelsPerChipX_ * pixelsPerChipY_ * chipsPerStripeX_ * chipsPerStripeY_;
  for (uint32_t imageNo = 0; imageNo < noOfImages_; imageNo++){
    // Check for which type of scrambling we are going to carry out
    if (scramble_ == excalibur){
      for (uint32_t stripe = 0; stripe < stripesPerImageX_; stripe++){
        // Scramble each stripe of data.
        if (stripe % 2 == 1){
          scrambleOddStripe(imageNo, pixelsPerStripe, stripe, scrambledDataUInt32_+(stripe*pixelsPerStripe), scrambledDataUInt16_+(stripe*pixelsPerStripe), scrambledDataUInt8_+(stripe*pixelsPerStripe), descrambleArray_+(stripe*pixelsPerStripe));
        } else {
          scrambleEvenStripe(imageNo, pixelsPerStripe, stripe, scrambledDataUInt32_+(stripe*pixelsPerStripe), scrambledDataUInt16_+(stripe*pixelsPerStripe), scrambledDataUInt8_+(stripe*pixelsPerStripe), descrambleArray_+(stripe*pixelsPerStripe));
        }
      }
    } else if (scramble_ == percival){
      // We don't have a method for this yet
      for (uint32_t indexY = 0; indexY < (pixelsPerChipY_ * chipsPerStripeY_); indexY++){
        for (uint32_t pixel = 0; pixel < pixelsPerChipX_; pixel++){
          for (uint32_t chip = 0; chip < chipsPerStripeX_; chip++){
            int pixelSourceAddress = (chip * pixelsPerChipX_) + pixel + (indexY * pixelsPerChipX_ * chipsPerStripeX_) + (imageNo * imageWidth_ * imageHeight_);
            int pixelDestAddress = chip + (pixel * chipsPerStripeX_) + (indexY * pixelsPerChipX_ * chipsPerStripeX_) + (imageNo * imageWidth_ * imageHeight_);
            // Store the descramble array (only for image 0, all images are scrambled the same way)
            if (imageNo == 0){
              descrambleArray_[pixelDestAddress] = pixelSourceAddress;
            }
            if (dataType_ == UnsignedInt8){
              scrambledDataUInt8_[pixelDestAddress] = rawDataUInt8_[pixelSourceAddress];
            } else if (dataType_ == UnsignedInt16){
              scrambledDataUInt16_[pixelDestAddress] = (uint16_t)applyGains(pixelSourceAddress - (imageNo * imageWidth_ * imageHeight_),
                                                                            rawDataUInt16_[pixelSourceAddress],
                                                                            ADC_index_,
                                                                            ADC_high_gain_,
                                                                            ADC_low_gain_,
                                                                            ADC_offset_,
                                                                            stage_gains_,
                                                                            stage_offsets_);
            } else if (dataType_ == UnsignedInt32){
              scrambledDataUInt32_[pixelDestAddress] = rawDataUInt32_[pixelSourceAddress];
            }
          }
        }
      }
    }
  }

  // Write the dataset.
  if (dataType_ == UnsignedInt32){
    status = H5Dwrite(dataset_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, scrambledDataUInt32_);
  } else if (dataType_ == UnsignedInt16){
    status = H5Dwrite(dataset_id, H5T_STD_U16LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, scrambledDataUInt16_);
  } else if (dataType_ == UnsignedInt8){
    status = H5Dwrite(dataset_id, H5T_STD_U8LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, scrambledDataUInt8_);
  }

  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);

  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Write out the descramble array
  status = H5Dwrite(dataset_descramble_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, descrambleArray_);

  // End access to the descramble array and release resources used by it.
  status = H5Dclose(dataset_descramble_id);

  // Terminate access to the descramble array data space.
  status = H5Sclose(dataspace_descramble_id);

}

uint32_t Configurator::applyGains(uint32_t index,         // Unscrambled index of point
                                  uint32_t in_data,       // Input data
                                  uint32_t * ADC_index,   // Index of ADC to use for each input data point
                                  float * ADC_low_gain,   // Array of low gain ADC gains, one per ADC
                                  float * ADC_high_gain,  // Array of high gain ADC gains, one per ADC
                                  float * ADC_offset,     // Combined offset for both ADC's
                                  float * stage_gains,    // Gain to apply for each of the output stages
                                  float * stage_offsets)  // Offsets to apply for each of the output stages (in scrambled order)
{
    uint32_t gain, ADC_low, ADC_high, ADC, imageSize;
    float value;
    
    imageSize = imageWidth_ * imageHeight_;
    value = in_data;
    ADC   = ADC_index[index];
        
    if ( value < gainThresholds_[0] ) gain = 0;
    else if ( value < gainThresholds_[1] ) gain = 1;
    else if ( value < gainThresholds_[2] ) gain = 2;
    else gain = 3;
    value = ( value - stage_offsets[gain*imageSize+index])/stage_gains[gain*imageSize+index] - ADC_offset[ADC];
    ADC_low  = (((uint32_t) (value/ADC_low_gain[ADC])) & 0xFF ) << 2;
    ADC_high = (((uint32_t) (( value - ( ADC_low >> 2 ) * ADC_low_gain[ADC] ) / ADC_high_gain[ADC] )) & 0x1F00 ) << 2;
        
    return gain + ADC_low + ADC_high;
}

int Configurator::readScrambledImage()
{
  PercivalDebug dbg(debug_, "Configurator::readScrambledImage");
  // Create the hdf5 file
  hid_t    dataset_id, dataspace_id;
  hsize_t  dims[3];
  herr_t   status;

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/scrambled_image", H5P_DEFAULT);

  // Retrieve the data space
  dataspace_id = H5Dget_space(dataset_id);

  // Find out the dimensions
  H5Sget_simple_extent_dims(dataspace_id, dims, NULL);

  noOfImages_ =  dims[0];
  imageHeight_ = dims[1];
  imageWidth_ =  dims[2];

  dbg.log(2, "Allocation", (uint32_t)(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint16_t)));

  // Allocate the memory required for the scrambled image
  scrambledDataUInt32_ = (uint32_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint32_t));
  scrambledDataUInt16_ = (uint16_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint16_t));
  scrambledDataUInt8_ = (uint8_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint8_t));
  dbg.log(2, "Datatype", (uint32_t)dataType_);
  // Read the dataset
  if (dataType_ == UnsignedInt32){
    status = H5Dread(dataset_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, scrambledDataUInt32_);
  } else if (dataType_ == UnsignedInt16){
    status = H5Dread(dataset_id, H5T_STD_U16LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, scrambledDataUInt16_);
  } else if (dataType_ == UnsignedInt8){
    status = H5Dread(dataset_id, H5T_STD_U8LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, scrambledDataUInt8_);
  }

  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);

  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Repeat the reading of a data set for the descramble array and for all other gain arrays

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/descramble_array", H5P_DEFAULT);
  // Retrieve the data space
  dataspace_id = H5Dget_space(dataset_id);
  dbg.log(2, "Allocation for descramble array", (uint32_t)(imageHeight_ * imageWidth_ * sizeof(uint32_t)));
  // Allocate the memory required for the scrambled image
  descrambleArray_ = (uint32_t *)malloc(imageHeight_ * imageWidth_ * sizeof(uint32_t));
  // Read the dataset
  status = H5Dread(dataset_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, descrambleArray_);
  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);
  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/gain_thresholds", H5P_DEFAULT);
  // Retrieve the data space
  dataspace_id = H5Dget_space(dataset_id);
  // Read the dataset
  status = H5Dread(dataset_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, gainThresholds_);
  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);
  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/ADC_index", H5P_DEFAULT);
  // Retrieve the data space
  dataspace_id = H5Dget_space(dataset_id);
  // Allocate the memory required for the scrambled image
  ADC_index_ = (uint32_t *)malloc(imageHeight_ * imageWidth_ * sizeof(uint32_t));
  // Read the dataset
  status = H5Dread(dataset_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ADC_index_);
  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);
  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/ADC_low_gain", H5P_DEFAULT);
  // Retrieve the data space
  dataspace_id = H5Dget_space(dataset_id);
  // Allocate the memory required for the scrambled image
  ADC_low_gain_ = (float *)malloc(noOfADCs_ * sizeof(float));
  // Read the dataset
  status = H5Dread(dataset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ADC_low_gain_);
  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);
  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/ADC_high_gain", H5P_DEFAULT);
  // Retrieve the data space
  dataspace_id = H5Dget_space(dataset_id);
  // Allocate the memory required for the scrambled image
  ADC_high_gain_ = (float *)malloc(noOfADCs_ * sizeof(float));
  // Read the dataset
  status = H5Dread(dataset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ADC_high_gain_);
  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);
  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/ADC_offset", H5P_DEFAULT);
  // Retrieve the data space
  dataspace_id = H5Dget_space(dataset_id);
  // Allocate the memory required for the scrambled image
  ADC_offset_ = (float *)malloc(noOfADCs_ * sizeof(float));
  // Read the dataset
  status = H5Dread(dataset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ADC_offset_);
  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);
  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/stage_gains", H5P_DEFAULT);
  // Retrieve the data space
  dataspace_id = H5Dget_space(dataset_id);
  // Allocate the memory required for the scrambled image
  stage_gains_ = (float *)malloc(4 * imageWidth_ * imageHeight_ * sizeof(float));
  // Read the dataset
  status = H5Dread(dataset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, stage_gains_);
  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);
  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/stage_offsets", H5P_DEFAULT);
  // Retrieve the data space
  dataspace_id = H5Dget_space(dataset_id);
  // Allocate the memory required for the scrambled image
  stage_offsets_ = (float *)malloc(4 * imageWidth_ * imageHeight_ * sizeof(float));
  // Read the dataset
  status = H5Dread(dataset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, stage_offsets_);
  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);
  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

  return 0;
}

void Configurator::generateMetaData()
{
  PercivalDebug dbg(debug_, "Configurator::generateMetaData");

  // Create the hdf5 file
  hid_t    dataset_id, dataspace_id, attribute_id;
  hsize_t  dims;
  herr_t   status;

  // Create the data space for the attribute.
  dims = 1;
  dataspace_id = H5Screate_simple(1, &dims, NULL);

  // Create the dataset.
  dataset_id = H5Dcreate2(file_id_, "/meta_data", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  // Create the full frame width attribute.
  attribute_id = H5Acreate2 (dataset_id, "FULL_FRAME_WIDTH", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &imageWidth_);
  // Close the attribute.
  status = H5Aclose(attribute_id);

  // Create the full frame height attribute.
  attribute_id = H5Acreate2 (dataset_id, "FULL_FRAME_HEIGHT", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &imageHeight_);
  // Close the attribute.
  status = H5Aclose(attribute_id);

  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "DATA_TYPE", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &dataType_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "PIXELS_PER_CHIP_X", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &pixelsPerChipX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "PIXELS_PER_CHIP_Y", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &pixelsPerChipY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "CHIPS_PER_BLOCK_X", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &chipsPerBlockX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "CHIPS_PER_BLOCK_Y", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &chipsPerBlockY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "BLOCKS_PER_STRIPE_X", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &blocksPerStripeX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "BLOCKS_PER_STRIPE_Y", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &blocksPerStripeY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "CHIPS_PER_STRIPE_X", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &chipsPerStripeX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "CHIPS_PER_STRIPE_Y", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &chipsPerStripeY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "STRIPES_PER_IMAGE_X", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &stripesPerImageX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "STRIPES_PER_IMAGE_Y", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &stripesPerImageY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);

  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "NUMBER_OF_ADCS", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &noOfADCs_);
  // Close the attribute.
  status = H5Aclose(attribute_id);

  // Close the dataspace.
  status = H5Sclose(dataspace_id);

  // Close to the dataset.
  status = H5Dclose(dataset_id);
}

int Configurator::readMetaData()
{
  PercivalDebug dbg(debug_, "Configurator::readMetaData");

  // Open up the hdf5 file
  hid_t    dataset_id, attribute_id;
  herr_t   status;

  // Create the data space for the attribute.
  //dims = 1;
  //dataspace_id = H5Screate_simple(1, &dims, NULL);

  // Open the dataset.
  dataset_id = H5Dopen(file_id_, "/meta_data", H5P_DEFAULT);

  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "DATA_TYPE", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &dataType_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "PIXELS_PER_CHIP_X", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &pixelsPerChipX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "PIXELS_PER_CHIP_Y", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &pixelsPerChipY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "CHIPS_PER_BLOCK_X", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &chipsPerBlockX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "CHIPS_PER_BLOCK_Y", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &chipsPerBlockY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "BLOCKS_PER_STRIPE_X", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &blocksPerStripeX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "BLOCKS_PER_STRIPE_Y", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &blocksPerStripeY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "CHIPS_PER_STRIPE_X", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &chipsPerStripeX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "CHIPS_PER_STRIPE_Y", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &chipsPerStripeY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "STRIPES_PER_IMAGE_X", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &stripesPerImageX_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "STRIPES_PER_IMAGE_Y", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &stripesPerImageY_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "NUMBER_OF_ADCS", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &noOfADCs_);
  // Close the attribute.
  status = H5Aclose(attribute_id);

  // Close to the dataset.
  status = H5Dclose(dataset_id);

  return 0;
}

void Configurator::scrambleOddStripe(uint32_t imageNo, uint32_t pixelsPerStripe, uint32_t stripe, uint32_t *out32, uint16_t *out16, uint8_t *out8, uint32_t *descrambleArray)
{
  PercivalDebug dbg(debug_, "Configurator::scrambleOddStripe");

  int gapStrip = 1;
  //int in = 0;
  out32 += (imageNo * imageHeight_ * imageWidth_);
  out16 += (imageNo * imageHeight_ * imageWidth_);
  out8 += (imageNo * imageHeight_ * imageWidth_);
  for(uint32_t block=0; block < blocksPerStripeX_; block++){
    for(uint32_t y=0; y < pixelsPerChipY_; y++){
      for(uint32_t x=0; x < pixelsPerChipX_/4; x++){
        for(uint32_t chip=0; chip < chipsPerBlockX_; chip++){
          for(uint32_t x2=0; x2 < 4; x2++){
            if(gapStrip){
              int pixelX = pixelsPerChipX_ * chipsPerStripeX_ - 1 -
                           (block * (pixelsPerChipX_ * chipsPerStripeX_ / 2 + 2) +
                           chip * pixelsPerChipX_ + (255 - (x2 + x * 4)));
              int pixelY = y;
              int pixelAddr = pixelX + pixelY * pixelsPerChipX_ * chipsPerStripeX_ + (stripe*pixelsPerStripe) + (imageNo * imageHeight_ * imageWidth_);
if (pixelAddr < 0){
std::cout << "Block: " << block << std::endl;
std::cout << "x: " << x << std::endl;
std::cout << "y: " << y << std::endl;
std::cout << "Chip: " << chip << std::endl;
std::cout << "x2: " << x2 << std::endl;
std::cout << "Pixel X: " << pixelX << std::endl;
std::cout << "Pixel Y: " << pixelY << std::endl;
std::cout << "Pixel Addr: " << pixelAddr << std::endl;
pixelAddr = 0;
}
              *out32 = rawDataUInt32_[pixelAddr];
              *out16 = rawDataUInt16_[pixelAddr];
              *out8 = rawDataUInt8_[pixelAddr];
              out32++;
              out16++;
              out8++;
              if (imageNo == 0){
                *descrambleArray = pixelAddr;
                descrambleArray++;
              }
              //out[pixelAddr] = *in;
              //in += 1;
            } else {
              //int pixelX = pixelsPerChipX_ * chipsPerStripeX_ - 1 -
              //             (block * (pixelsPerChipX_ * chipsPerStripeX_/2 + 2) +
              //             chip * (pixelsPerChipX_ + FEM_CHIP_GAP_PIXELS_X) + (255- (x2 + x * 4)));
              //int pixelY = y;
              //int pixelAddr = pixelX + pixelY*FEM_PIXELS_PER_STRIPE_X;
              //out[pixelAddr] = *in;
              //in += 1;
            }
          }
        }
      }
    }
  }
}

void Configurator::scrambleEvenStripe(uint32_t imageNo, uint32_t pixelsPerStripe, uint32_t stripe, uint32_t *out32, uint16_t *out16, uint8_t *out8, uint32_t *descrambleArray)
{
  PercivalDebug dbg(debug_, "Configurator::scrambleEvenStripe");

  int gapStrip = 1;
  //int in = 0;
  out32 += (imageNo * imageHeight_ * imageWidth_);
  out16 += (imageNo * imageHeight_ * imageWidth_);
  out8 += (imageNo * imageHeight_ * imageWidth_);
  for(uint32_t block=0; block < blocksPerStripeX_; block++){
    for(uint32_t y=0; y < pixelsPerChipY_; y++){
      for(uint32_t x=0; x < pixelsPerChipX_/4; x++){
        for(uint32_t chip=0; chip < chipsPerBlockX_; chip++){
          for(uint32_t x2=0; x2 < 4; x2++){
            if(gapStrip){
              int pixelX = (block * (pixelsPerChipX_ * chipsPerStripeX_ / 2 + 2) +
                           chip * pixelsPerChipX_ + (255 - (x2 + x * 4)));
              int pixelY = 255-y;
              int pixelAddr = pixelX + pixelY * pixelsPerChipX_ * chipsPerStripeX_ + (stripe*pixelsPerStripe) + (imageNo * imageHeight_ * imageWidth_);
if (pixelAddr < 0){
std::cout << "Block: " << block << std::endl;
std::cout << "x: " << x << std::endl;
std::cout << "y: " << y << std::endl;
std::cout << "Chip: " << chip << std::endl;
std::cout << "x2: " << x2 << std::endl;
std::cout << "Pixel X: " << pixelX << std::endl;
std::cout << "Pixel Y: " << pixelY << std::endl;
std::cout << "Pixel Addr: " << pixelAddr << std::endl;
pixelAddr = 0;
}
              *out32 = rawDataUInt32_[pixelAddr];
              *out16 = rawDataUInt16_[pixelAddr];
              *out8 = rawDataUInt8_[pixelAddr];
              out32++;
              out16++;
              out8++;
              if (imageNo == 0){
                *descrambleArray = pixelAddr;
                descrambleArray++;
              }
              //out[pixelAddr] = *in;
              //in += 1;
            } else {
              //int pixelX = (block * (pixelsPerChip_ * chipsPerStripeX_ / 2 + 2) +
              //             chip * (pixelsPerChipX_ + FEM_CHIP_GAP_PIXELS_X) + (255 - (x2 + x * 4)));
              //int pixelY = 255-y;
              //int pixelAddr = pixelX + pixelY*FEM_PIXELS_PER_STRIPE_X;
              //out[pixelAddr] = *in;
              //in += 1;
            }
          }
        }
      }
    }
  }
}

void Configurator::allocateDataArrays()
{
  PercivalDebug dbg(debug_, "Configurator::allocateDataArrays");

  if (arraysAllocated_){
    // If we simply allocate the arrays here we will have a memory leak
    // Free them first
    freeDataArrays();
  }

  // Allocate the memory required for the raw image
  rawDataUInt32_ = (uint32_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint32_t));
  rawDataUInt16_ = (uint16_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint16_t));
  rawDataUInt8_ = (uint8_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint8_t));
  // Allocate the memory required for the scrambled image
  scrambledDataUInt32_ = (uint32_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint32_t));
  scrambledDataUInt16_ = (uint16_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint16_t));
  scrambledDataUInt8_ = (uint8_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint8_t));
  
  arraysAllocated_ = true;
}

void Configurator::freeDataArrays()
{
  PercivalDebug dbg(debug_, "Configurator::freeDataArrays");

  if (arraysAllocated_){
    // Free both image arrays
    free(rawDataUInt32_);
    free(rawDataUInt16_);
    free(rawDataUInt8_);
    free(scrambledDataUInt32_);
    free(scrambledDataUInt16_);
    free(scrambledDataUInt8_);
    arraysAllocated_ = false;
  } else {
    // Attempting to free data that is not allocated
    dbg.log(0, "ERROR - Arrays are not allocated");
  }
}

void Configurator::copyScrambledSectionUInt32(uint32_t imageNo, uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint32_t *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyScrambledSectionUInt32");
  uint32_t yIndex;
  uint32_t width;
  width = botRightX - topLeftX + 1;
  // Just copy frame zero currently
  uint32_t *ptr1 = buffer;
  uint32_t *ptr2 = scrambledDataUInt32_;
  for (yIndex = topLeftY; yIndex <= botRightY; yIndex++){
    ptr1 = buffer + ((yIndex-topLeftY) * width);
    ptr2 = scrambledDataUInt32_ + (yIndex * imageWidth_) + topLeftX + (imageNo * imageWidth_ * imageHeight_);
    memcpy(ptr1, ptr2, (width * sizeof(uint32_t)));
  }
}

void Configurator::copyScrambledSectionUInt16(uint32_t imageNo, uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint16_t *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyScrambledSectionUInt16");
  uint32_t yIndex;
  uint32_t width;
  width = botRightX - topLeftX + 1;
  // Just copy frame zero currently
  uint16_t *ptr1 = buffer;
  uint16_t *ptr2 = scrambledDataUInt16_;
  for (yIndex = topLeftY; yIndex <= botRightY; yIndex++){
    ptr1 = buffer + ((yIndex-topLeftY) * width);
    ptr2 = scrambledDataUInt16_ + (yIndex * imageWidth_) + topLeftX + (imageNo * imageWidth_ * imageHeight_);
    memcpy(ptr1, ptr2, (width * sizeof(uint16_t)));
  }
}

void Configurator::copyScrambledSectionUInt8(uint32_t imageNo, uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint8_t *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyScrambledSectionUInt8");
  uint32_t yIndex;
  uint32_t width;
  width = botRightX - topLeftX + 1;
  // Just copy frame zero currently
  uint8_t *ptr1 = buffer;
  uint8_t *ptr2 = scrambledDataUInt8_;
  for (yIndex = topLeftY; yIndex <= botRightY; yIndex++){
    ptr1 = buffer + ((yIndex-topLeftY) * width);
    ptr2 = scrambledDataUInt8_ + (yIndex * imageWidth_) + topLeftX + (imageNo * imageWidth_ * imageHeight_);
    memcpy(ptr1, ptr2, (width * sizeof(uint8_t)));
  }
}

void Configurator::copyDescrambleArray(uint32_t *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyDescrambleArray");
  // Assumes the buffer provided has already been allocated
  memcpy(buffer, descrambleArray_, (imageWidth_ * imageHeight_ * sizeof(uint32_t)));
}

void Configurator::copyGainThresholds(uint32_t *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyGainThresholds");
  // Assumes the buffer provided has already been allocated
  memcpy(buffer, gainThresholds_, (4 * sizeof(uint32_t)));
}

void Configurator::copyADCIndex(uint32_t *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyADCIndex");
  // Assumes the buffer provided has already been allocated
  memcpy(buffer, ADC_index_, (imageWidth_ * imageHeight_ * sizeof(uint32_t)));
}

void Configurator::copyADCLowGain(float *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyADCLowGain");
  // Assumes the buffer provided has already been allocated
  memcpy(buffer, ADC_low_gain_, (noOfADCs_ * sizeof(float)));
}

void Configurator::copyADCHighGain(float *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyADCHighGain");
  // Assumes the buffer provided has already been allocated
  memcpy(buffer, ADC_high_gain_, (noOfADCs_ * sizeof(float)));
}

void Configurator::copyADCOffsets(float *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyADCOffsets");
  // Assumes the buffer provided has already been allocated
  memcpy(buffer, ADC_offset_, (noOfADCs_ * sizeof(float)));
}

void Configurator::copyStageGains(float *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyStageGains");
  // Assumes the buffer provided has already been allocated
  memcpy(buffer, stage_gains_, (4 * imageWidth_ * imageHeight_ * sizeof(float)));
}

void Configurator::copyStageOffsets(float *buffer)
{
  PercivalDebug dbg(debug_, "Configurator::copyStageOffsets");
  // Assumes the buffer provided has already been allocated
  memcpy(buffer, stage_offsets_, (4 * imageWidth_ * imageHeight_ * sizeof(float)));
}


