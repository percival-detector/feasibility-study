/*
 * Configurator.cpp
 *
 *  Created on: 6th Aug 2013
 *      Author: gnx91527
 */

#include "Configurator.h"
#include <iostream>
#include <cstring>

Configurator::Configurator()
  : imageWidth_(2048),
    imageHeight_(1536),
    repeatX_(64),
    repeatY_(64),
    pattern_(rectangle),
    noOfImages_(10),
    minValue_(0),
    maxValue_(256),
    dataType_(UnsignedInt32),
    pixelsPerChipX_(256),
    pixelsPerChipY_(256),
    chipsPerBlockX_(4),
    blocksPerStripeX_(2),
    chipsPerStripeX_(8),
    chipsPerStripeY_(1),
    stripesPerModule_(2),
    stripesPerImage_(6),
    scramble_(excalibur),
    arraysAllocated_(false),
    debug_(0)
{

}

Configurator::~Configurator()
{

}

void Configurator::setDebugLevel(uint32_t level)
{
  debug_ = level;
}

void Configurator::setImageWidth(uint32_t width)
{
  // Set the image width in pixels
  imageWidth_ = width;
}

uint32_t Configurator::getImageWidth()
{
  // Return the current image width
  return imageWidth_;
}

void Configurator::setImageHeight(uint32_t height)
{
  // Set the image height in pixels
  imageHeight_ = height;
}

uint32_t Configurator::getImageHeight()
{
  // Return the current image height
  return imageHeight_;
}

void Configurator::setRepeatX(uint32_t repeatX)
{
  // Set the pattern repeat in x (pixels)
  repeatX_ = repeatX;
}

uint32_t Configurator::getRepeatX()
{
  // Return the pattern repeat in x
  return repeatX_;
}

void Configurator::setRepeatY(uint32_t repeatY)
{
  // Set the pattern repeat in y (pixels)
  repeatY_ = repeatY;
}

uint32_t Configurator::getRepeatY()
{
  // Return the pattern repeat in y
  return repeatY_;
}

void Configurator::setPattern(PatternType pattern)
{
  // Set the pattern type for the image
  pattern_ = pattern;
}

PatternType Configurator::getPattern()
{
  // Return the pattern type
  return pattern_;
}

void Configurator::setNoOfImages(uint32_t noOfImages)
{
  // Set the number of images in the sequence
  noOfImages_ = noOfImages;
}

uint32_t Configurator::getNoOfImages()
{
  // Return the number of images in the sequence
  return noOfImages_;
}

void Configurator::setMinValue(uint32_t minValue)
{
  // Set the minimum value for images
  minValue_ = minValue;
}

uint32_t Configurator::getMinValue()
{
  // Return the minimum value for images
  return minValue_;
}

void Configurator::setMaxValue(uint32_t maxValue)
{
  // Set the maximum value for images
  maxValue_ = maxValue;
}

uint32_t Configurator::getMaxValue()
{
  // Return the maximum value for images
  return maxValue_;
}

void Configurator::setDataType(DataType dataType)
{
  // Set the data type for the images
  dataType_ = dataType;
}

DataType Configurator::getDataType()
{
  // Return the data type of the images
  return dataType_;
}

void Configurator::setPixelsPerChipX(uint32_t ppcx)
{
  // Set the number of pixels per chip in the x direction
  pixelsPerChipX_ = ppcx;
}

uint32_t Configurator::getPixelsPerChipX()
{
  // Return the number of pixels per chip in the x direction
  return pixelsPerChipX_;
}

void Configurator::setPixelsPerChipY(uint32_t ppcy)
{
  // Set the number of pixels per chip in the y direction
  pixelsPerChipY_ = ppcy;
}

uint32_t Configurator::getPixelsPerChipY()
{
  // Return the number of pixels per chip in the y direction
  return pixelsPerChipY_;
}

void Configurator::setChipsPerBlockX(uint32_t cpbx)
{
  // Set the number of chips per block in the x direction
  chipsPerBlockX_ = cpbx;
}

uint32_t Configurator::getChipsPerBlockX()
{
  // Return the number of chips per block in the x direction
  return chipsPerBlockX_;
}

void Configurator::setBlocksPerStripeX(uint32_t bpsx)
{
  // Set the number of blocks per stripe in the x direction
  blocksPerStripeX_ = bpsx;
}

uint32_t Configurator::getBlocksPerStripeX()
{
  // Return the number of blocks per stripe in the x direction
  return blocksPerStripeX_;
}

void Configurator::setChipsPerStripeX(uint32_t cpsx)
{
  // Set the number of chips per stripe in the x direction
  chipsPerStripeX_ = cpsx;
}

uint32_t Configurator::getChipsPerStripeX()
{
  // Return the number of chips per stripe in the x direction
  return chipsPerStripeX_;
}

void Configurator::setChipsPerStripeY(uint32_t cpsy)
{
  // Set the number of chips per stripe in the y direction
  chipsPerStripeY_ = cpsy;
}

uint32_t Configurator::getChipsPerStripeY()
{
  // Return the number of chips per stripe in the y direction
  return chipsPerStripeY_;
}

void Configurator::setStripesPerModule(uint32_t spm)
{
  // Set the number of stipes per module
  stripesPerModule_ = spm;
}

uint32_t Configurator::getStripesPerModule()
{
  // Return the number of stripes per module
  return stripesPerModule_;
}

void Configurator::setStripesPerImage(uint32_t spi)
{
  // Set the number of stripes per image
  stripesPerImage_ = spi;
}

uint32_t Configurator::getStripesPerImage()
{
  // Return the number of stripes per image
  return stripesPerImage_;
}

void Configurator::setScrambleType(ScrambleType scramble)
{
  // Set the scramble type for the image
  scramble_ = scramble;
}

ScrambleType Configurator::getScrambleType()
{
  // Return the scramble type
  return scramble_;
}

void Configurator::openHDF5File(const std::string& filename)
{
  static const char *functionName = "Configurator::openHDF5File";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

  herr_t status;
  // First close the file if it is already open
  if (file_id_){
    status = H5Fclose(file_id_);
  }

  // Now open the file by filename
  file_id_ = H5Fopen(filename.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  if (file_id_ < 0){
    std::cout << "ERROR Configurator::openHDF5File - Unable to open file [" << filename << "]" << std::endl;
    file_id_ = 0;
  }
}

void Configurator::createHDF5File(const std::string& filename)
{
  static const char *functionName = "Configurator::createHDF5File";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

  herr_t status;
  // First close the file if it is already open
  if (file_id_){
    status = H5Fclose(file_id_);
  }

  // Now create (or open and truncate) the file by filename
  file_id_ = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (file_id_ < 0){
    std::cout << "ERROR Configurator::createHDF5File - Unable to create file [" << filename << "]" << std::endl;
    file_id_ = 0;
  }
}

void Configurator::closeHDF5File()
{
  static const char *functionName = "Configurator::closeHDF5File";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

  herr_t status;
  // Close the file if one is open
  if (file_id_){
    status = H5Fclose(file_id_);
    file_id_ = 0;
  }
}

void Configurator::readConfiguration(const std::string& filename)
{
  static const char *functionName = "Configurator::readConfiguration";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

  // Create a new HDF5 configuration file using the default properties
  openHDF5File(filename);

  // Read in the meta data
  readMetaData();

  // Read in the scrambled image
  readScrambledImage();

  // Close the configuration file
  closeHDF5File();
  
}

void Configurator::generateConfiguration(const std::string& filename)
{
  static const char *functionName = "Configurator::generateConfiguration";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

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
  static const char *functionName = "Configurator::generateRawImage";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

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
  static const char *functionName = "Configurator::generateScrambledImage";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

  // Create the hdf5 file
  hid_t    dataset_id = 0, dataspace_id;
  hsize_t  dims[3];
  herr_t   status;

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

  // Allocate the memory required for the scrambled image
  scrambledDataUInt32_ = (uint32_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint32_t));
  scrambledDataUInt16_ = (uint16_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint16_t));
  scrambledDataUInt8_ = (uint8_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint8_t));
  uint32_t pixelsPerStripe = pixelsPerChipX_ * pixelsPerChipY_ * chipsPerStripeX_ * chipsPerStripeY_;
  for (uint32_t imageNo = 0; imageNo < noOfImages_; imageNo++){
    for (uint32_t stripe = 0; stripe < stripesPerImage_; stripe++){
      // Check for which type of scrambling we are going to carry out
      if (scramble_ == excalibur){
        // Scramble each stripe of data.
        if (stripe % 2 == 1){
          scrambleOddStripe(imageNo, pixelsPerStripe, stripe, scrambledDataUInt32_+(stripe*pixelsPerStripe), scrambledDataUInt16_+(stripe*pixelsPerStripe), scrambledDataUInt8_+(stripe*pixelsPerStripe));
        } else {
          scrambleEvenStripe(imageNo, pixelsPerStripe, stripe, scrambledDataUInt32_+(stripe*pixelsPerStripe), scrambledDataUInt16_+(stripe*pixelsPerStripe), scrambledDataUInt8_+(stripe*pixelsPerStripe));
        }
      } else if (scramble_ == percival){
        // We don't have a method for this yet
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

}

void Configurator::readScrambledImage()
{
  static const char *functionName = "Configurator::readScrambledImage";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

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

std::cout << "Allocation: " << imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint16_t) << std::endl;

  // Allocate the memory required for the scrambled image
  scrambledDataUInt32_ = (uint32_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint32_t));
  scrambledDataUInt16_ = (uint16_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint16_t));
  scrambledDataUInt8_ = (uint8_t *)malloc(imageHeight_ * imageWidth_ * noOfImages_ * sizeof(uint8_t));
std::cout << "Reading in datatype: " << dataType_ << std::endl;
  // Read the dataset
  if (dataType_ == UnsignedInt32){
    status = H5Dread(dataset_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, scrambledDataUInt32_);
std::cout << "Here 1" << std::endl;
  } else if (dataType_ == UnsignedInt16){
    status = H5Dread(dataset_id, H5T_STD_U16LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, scrambledDataUInt16_);
std::cout << "Here 2" << std::endl;
  } else if (dataType_ == UnsignedInt8){
    status = H5Dread(dataset_id, H5T_STD_U8LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, scrambledDataUInt8_);
std::cout << "Here 3" << std::endl;
  }

  // End access to the dataset and release resources used by it.
  status = H5Dclose(dataset_id);

  // Terminate access to the data space.
  status = H5Sclose(dataspace_id);

}

void Configurator::generateMetaData()
{
  static const char *functionName = "Configurator::generateMetaData";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

  // Create the hdf5 file
  hid_t    dataset_id, dataspace_id, attribute_id;
  hsize_t  dims;
  herr_t   status;

  // Create the data space for the attribute.
  dims = 1;
  dataspace_id = H5Screate_simple(1, &dims, NULL);

  // Create the dataset.
  dataset_id = H5Dcreate2(file_id_, "/meta_data", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

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
  attribute_id = H5Acreate2 (dataset_id, "BLOCKS_PER_STRIPE_X", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &blocksPerStripeX_);
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
  attribute_id = H5Acreate2 (dataset_id, "STRIPES_PER_MODULE", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &stripesPerModule_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Create a dataset attribute.
  attribute_id = H5Acreate2 (dataset_id, "STRIPES_PER_IMAGE", H5T_STD_U32LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
  // Write the attribute data.
  status = H5Awrite(attribute_id, H5T_STD_U32LE, &stripesPerImage_);
  // Close the attribute.
  status = H5Aclose(attribute_id);

  // Close the dataspace.
  status = H5Sclose(dataspace_id);

  // Close to the dataset.
  status = H5Dclose(dataset_id);
}

void Configurator::readMetaData()
{
  static const char *functionName = "Configurator::readMetaData";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

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
  attribute_id = H5Aopen(dataset_id, "BLOCKS_PER_STRIPE_X", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &blocksPerStripeX_);
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
  attribute_id = H5Aopen(dataset_id, "STRIPES_PER_MODULE", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &stripesPerModule_);
  // Close the attribute.
  status = H5Aclose(attribute_id);
  // Open the attribute.
  attribute_id = H5Aopen(dataset_id, "STRIPES_PER_IMAGE", H5P_DEFAULT);
  // Read the attribute data.
  status = H5Aread(attribute_id, H5T_STD_U32LE, &stripesPerImage_);
  // Close the attribute.
  status = H5Aclose(attribute_id);

  // Close to the dataset.
  status = H5Dclose(dataset_id);
}

void Configurator::scrambleOddStripe(uint32_t imageNo, uint32_t pixelsPerStripe, uint32_t stripe, uint32_t *out32, uint16_t *out16, uint8_t *out8)
{
  static const char *functionName = "Configurator::scrambleOddStripe";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

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

void Configurator::scrambleEvenStripe(uint32_t imageNo, uint32_t pixelsPerStripe, uint32_t stripe, uint32_t *out32, uint16_t *out16, uint8_t *out8)
{
  static const char *functionName = "Configurator::scrambleEvenStripe";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

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
  static const char *functionName = "Configurator::allocateDataArrays";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

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
  static const char *functionName = "Configurator::freeDataArrays";
  if (debug_ > 2){
    std::cout << "DEBUG " << functionName << std::endl;
  }

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
    std::cout << "ERROR " << functionName << " - Called on arrays that are not allocated" << std::endl;
  }
}

void Configurator::copyScrambledSectionUInt32(uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint32_t *buffer)
{
  uint32_t yIndex;
  uint32_t width;
  width = botRightX - topLeftX + 1;
  // Just copy frame zero currently
  uint32_t *ptr1 = buffer;
  uint32_t *ptr2 = scrambledDataUInt32_;
  for (yIndex = topLeftY; yIndex <= botRightY; yIndex++){
    ptr1 = buffer + ((yIndex-topLeftY) * width);
    ptr2 = scrambledDataUInt32_ + (yIndex * imageWidth_) + topLeftX;
    memcpy(ptr1, ptr2, (width * sizeof(uint32_t)));
  }
}

void Configurator::copyScrambledSectionUInt16(uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint16_t *buffer)
{
  uint32_t yIndex;
  uint32_t width;
  width = botRightX - topLeftX + 1;
  // Just copy frame zero currently
  uint16_t *ptr1 = buffer;
  uint16_t *ptr2 = scrambledDataUInt16_;
  for (yIndex = topLeftY; yIndex <= botRightY; yIndex++){
    ptr1 = buffer + ((yIndex-topLeftY) * width);
    ptr2 = scrambledDataUInt16_ + (yIndex * imageWidth_) + topLeftX;
    memcpy(ptr1, ptr2, (width * sizeof(uint16_t)));
  }
}

void Configurator::copyScrambledSectionUInt8(uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint8_t *buffer)
{
  uint32_t yIndex;
  uint32_t width;
  width = botRightX - topLeftX + 1;
  // Just copy frame zero currently
  uint8_t *ptr1 = buffer;
  uint8_t *ptr2 = scrambledDataUInt8_;
  for (yIndex = topLeftY; yIndex <= botRightY; yIndex++){
    ptr1 = buffer + ((yIndex-topLeftY) * width);
    ptr2 = scrambledDataUInt8_ + (yIndex * imageWidth_) + topLeftX;
    memcpy(ptr1, ptr2, (width * sizeof(uint8_t)));
  }
}

