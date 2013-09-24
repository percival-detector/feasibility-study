
#include <ctime>
#include <iostream>
#include <string>
#include <iomanip>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>

#include "PercivalServer.h"

int main()
{
  try
  {

    int value = 32766;
float ADC_low_gain = 8.0;
float ADC_high_gain = 1024.0;

//    if ( value < gainThresholds_[0] ) gain = 0;
//    else if ( value < gainThresholds_[1] ) gain = 1;
//    else if ( value < gainThresholds_[2] ) gain = 2;
//    else gain = 3;
//    value = ( value - stage_offsets[gain*imageSize+index])/stage_gains[gain*imageSize+index] - ADC_offset[ADC];
    uint32_t gain = 2;
    uint32_t ADC_low  = (((uint32_t) (value/ADC_low_gain)) & 0xFF ) << 2;
    uint32_t ADC_high = (((uint32_t) (( value - ( ADC_low >> 2 ) * ADC_low_gain ) / ADC_high_gain )) & 0x1F ) << 10;

    std::cout << "Original ADC_high: " << (ADC_high >> 10) << std::endl;
    std::cout << "ADC_high: " << ADC_high << std::endl;
    std::cout << "Original ADC_low: " << (ADC_low >> 2) << std::endl;
    std::cout << "ADC_low: " << ADC_low << std::endl;
        
    uint16_t outval = gain + ADC_low + ADC_high;
    std::cout << "Test: " << outval << std::endl;

//outval = 21627;

    gain     = outval & 0x3;
    ADC_low  = ( outval >> 2 ) & 0xFF;
    ADC_high = ( outval >> 10 ) & 0x1F;
    std::cout << "Post ADC_high: " << (ADC_high) << std::endl;

    std::cout << "Reverse: " << ((ADC_high) * ADC_high_gain + (ADC_low) * ADC_low_gain) << std::endl;


    //PercivalServer *sPtr = new PercivalServer();
    //sPtr->setDebug(5);
    //sPtr->setupChannel("127.0.0.1", 9876, 10000);
    //sPtr->setupFullFrame(256, 256, UnsignedInt16, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    //sPtr->setupSubFrame(0, 0, 0, 127, 127);
    //sPtr->setupSubFrame(1, "127.0.0.1", 9877, 0, 256, 2047, 511, 2);
    //sPtr->setupSubFrame(3, "127.0.0.1", 9878, 0, 512, 2047, 767, 2);

    //sPtr->startAcquisition();

    //while (true){
    //  usleep(10000);
    //}

//    sPtr->releaseSubFrame(1);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
