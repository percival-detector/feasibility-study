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
  ptr->generateConfiguration("/home/ajg/testfile.hdf5");


  std::cout << "*** End of Configurator test" << std::endl;
  
  return 0;
}



