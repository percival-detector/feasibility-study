/*
 * testConfigurator.cpp
 *
 *  Created on: 6th Aug 2013
 *      Author: gnx91527
 */

#include <iostream>
#include <iomanip>
#include <math.h>

#include "Configurator.h"

using namespace std;


int main(int argc, char *argv[])
{
  Configurator *ptr = new Configurator();
  ptr->setDebugLevel(0);
  ptr->setRepeatX(256);
  ptr->setRepeatY(256);
  ptr->setPattern(ellipse);
  ptr->setNoOfImages(10);
  ptr->setMinValue(0);
  ptr->setMaxValue(30000);
  ptr->setDataType(UnsignedInt16);
  ptr->setPixelsPerChipX(64);
  ptr->setPixelsPerChipY(64);
  ptr->setChipsPerBlockX(4);
  ptr->setChipsPerBlockY(4);
  ptr->setBlocksPerStripeX(1);
  ptr->setBlocksPerStripeY(1);
  ptr->setStripesPerImageX(1);
  ptr->setStripesPerImageY(1);
  ptr->setScrambleType(percival);

  ptr->generateConfiguration("/home/gnx91527/testgains.hdf5");


  std::cout << "*** End of Configurator test" << std::endl;
  
  return 0;
}



