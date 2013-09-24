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

    std::string errorMessage();

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

    void setNumberOfADCs(uint32_t noOfADCs);
    uint32_t getNumberOfADCs();
    void setGainThreshold(uint32_t number, uint32_t threshold);
    uint32_t getGainThreshold(uint32_t number);

    int openHDF5File(const std::string& filename);
    void createHDF5File(const std::string& filename);
    int closeHDF5File();
    int readConfiguration(const std::string& filename);
    void generateConfiguration(const std::string& filename);
    void generateRawImage();
    void generateScrambledImage();
    uint32_t applyGains(uint32_t index,           // Unscrambled index of point
                        uint32_t in_data,         // Input data
                        uint32_t *ADC_index,      // Index of ADC to use for each input data point
                        float    *ADC_low_gain,   // Array of low gain ADC gains, one per ADC
                        float    *ADC_high_gain,  // Array of high gain ADC gains, one per ADC
                        float    *ADC_offset,     // Combined offset for both ADC's
                        float    *stage_gains,    // Gain to apply for each of the output stages
                        float    *stage_offsets); // Offsets to apply for each of the output stages (in scrambled order)

    int readScrambledImage();
    void generateMetaData();
    int readMetaData();
    void scrambleOddStripe(uint32_t imageNo, uint32_t pixelsPerStripe, uint32_t stripe, uint32_t *out32, uint16_t *out16, uint8_t *out8, uint32_t *descrambleArray);
    void scrambleEvenStripe(uint32_t imageNo, uint32_t pixelsPerStripe, uint32_t stripe, uint32_t *out32, uint16_t *out16, uint8_t *out8, uint32_t *descrambleArray);
    void allocateDataArrays();
    void freeDataArrays();
    void copyScrambledSectionUInt32(uint32_t imageNo, uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint32_t *buffer);
    void copyScrambledSectionUInt16(uint32_t imageNo, uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint16_t *buffer);
    void copyScrambledSectionUInt8(uint32_t imageNo, uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint8_t *buffer);
    void copyResetDataSectionUInt16(uint32_t imageNo, uint32_t topLeftX, uint32_t topLeftY, uint32_t botRightX, uint32_t botRightY, uint16_t *buffer);
    void copyDescrambleArray(uint32_t *buffer);
    void copyGainThresholds(uint32_t *buffer);
    void copyADCIndex(uint32_t *buffer);
    void copyADCLowGain(float *buffer);
    void copyADCHighGain(float *buffer);
    void copyADCOffsets(float *buffer);
    void copyStageGains(float *buffer);
    void copyStageOffsets(float *buffer);

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
    // Pointer to the descramble array
    uint32_t *descrambleArray_;
    // Are the arrays allocated
    bool arraysAllocated_;

    // Data structures for gain information
    uint32_t noOfADCs_;
    uint32_t gainThresholds_[4];
    uint32_t *ADC_index_;         // Index of ADC to use for each input data point
    float    *ADC_low_gain_;      // Array of low gain ADC gains, one per ADC
    float    *ADC_high_gain_;     // Array of high gain ADC gains, one per ADC
    float    *ADC_offset_;        // Combined offset for both ADC's
    float    *stage_gains_;       // Gain to apply for each of the output stages
    float    *stage_offsets_;     // Offsets to apply for each of the output stages (in scrambled order)
    uint16_t *resetDataUInt16_;   // Reset data
    //float    gain_thresholds_[4]; // Threshold at which output gain stage switches


    uint32_t    debug_;          // Debug level
    std::string errorMessage_;   // Error message string
};

#endif // CONFIGURATOR_H_

