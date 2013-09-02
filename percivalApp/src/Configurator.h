/*
 * Configurator.h
 *
 *  Created on: 6th Aug 2013
 *      Author: gnx91527
 */

#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_

#include <iostream>
#include <boost/cstdint.hpp>
#include <hdf5.h>
#include <hdf5_hl.h>

#include "PercivalDataType.h"

typedef enum
{
  rectangle,
  triangle,
  ellipse
} PatternType;

typedef enum
{
  excalibur,
  percival
} ScrambleType;

class Configurator
{
  public:

    Configurator();
    virtual ~Configurator();

    void setDebugLevel(uint32_t level);

    uint32_t getImageWidth();
    uint32_t getImageHeight();
    void setRepeatX(uint32_t repeatX);
    uint32_t getRepeatX();
    void setRepeatY(uint32_t repeatY);
    uint32_t getRepeatY();
    void setPattern(PatternType pattern);
    PatternType getPattern();
    void setNoOfImages(uint32_t noOfImages);
    uint32_t getNoOfImages();
    void setMinValue(uint32_t minValue);
    uint32_t getMinValue();
    void setMaxValue(uint32_t maxValue);
    uint32_t getMaxValue();
    void setDataType(DataType dataType);
    DataType getDataType();

    void setPixelsPerChipX(uint32_t ppcx);
    uint32_t getPixelsPerChipX();
    void setPixelsPerChipY(uint32_t ppcy);
    uint32_t getPixelsPerChipY();
    void setChipsPerBlockX(uint32_t cpbx);
    uint32_t getChipsPerBlockX();
    void setChipsPerBlockY(uint32_t cpby);
    uint32_t getChipsPerBlockY();
    void setBlocksPerStripeX(uint32_t bpsx);
    uint32_t getBlocksPerStripeX();
    void setBlocksPerStripeY(uint32_t bpsy);
    uint32_t getBlocksPerStripeY();
    uint32_t getChipsPerStripeX();
    uint32_t getChipsPerStripeY();
    void setStripesPerImageX(uint32_t spix);
    uint32_t getStripesPerImageX();
    void setStripesPerImageY(uint32_t spiy);
    uint32_t getStripesPerImageY();
    void setScrambleType(ScrambleType scramble);
    ScrambleType getScrambleType();

    void openHDF5File(const std::string& filename);
    void createHDF5File(const std::string& filename);
    void closeHDF5File();
    void readConfiguration(const std::string& filename);
    void generateConfiguration(const std::string& filename);
    void generateRawImage();
    void generateScrambledImage();
    void readScrambledImage();
    void generateMetaData();
    void readMetaData();
    void scrambleOddStripe(uint32_t imageNo, uint32_t pixelsPerStripe, uint32_t stripe, uint32_t *out32, uint16_t *out16, uint8_t *out8);
    void scrambleEvenStripe(uint32_t imageNo, uint32_t pixelsPerStripe, uint32_t stripe, uint32_t *out32, uint16_t *out16, uint8_t *out8);
    void allocateDataArrays();
    void freeDataArrays();
    void copyScrambledSectionUInt32(uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint32_t *buffer);
    void copyScrambledSectionUInt16(uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint16_t *buffer);
    void copyScrambledSectionUInt8(uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint8_t *buffer);

  private:
    // Raw image values
    uint32_t    imageWidth_;
    uint32_t    imageHeight_;
    uint32_t    repeatX_;
    uint32_t    repeatY_;
    PatternType pattern_;
    uint32_t    noOfImages_;
    uint32_t    minValue_;
    uint32_t    maxValue_;
    DataType    dataType_;

    // Physical setup values
    uint32_t pixelsPerChipX_;
    uint32_t pixelsPerChipY_;
    uint32_t chipsPerBlockX_;
    uint32_t chipsPerBlockY_;
    uint32_t blocksPerStripeX_;
    uint32_t blocksPerStripeY_;
    uint32_t chipsPerStripeX_;
    uint32_t chipsPerStripeY_;
    uint32_t stripesPerImageX_;
    uint32_t stripesPerImageY_;

    // Which setup should we use
    ScrambleType scramble_;

    // HDF5 file handle
    hid_t       file_id_;

    // Pointer to raw data
    uint32_t *rawDataUInt32_;
    uint16_t *rawDataUInt16_;
    uint8_t *rawDataUInt8_;
    // Pointer to scrambled image
    uint32_t *scrambledDataUInt32_;
    uint16_t *scrambledDataUInt16_;
    uint8_t *scrambledDataUInt8_;
    // Are the arrays allocated
    bool arraysAllocated_;

    // Debug level
    uint32_t debug_;
};

#endif // CONFIGURATOR_H_

