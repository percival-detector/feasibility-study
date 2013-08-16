/*
 * sendTest.cpp
 *
 *  Created on: 5th Aug 2013
 *      Author: gnx91527
 */

#include <iostream>
#include <iomanip>
#include <math.h>

#include "DataSender.h"

using namespace std;


int main(int argc, char *argv[])
{
  //DataSender *sendPtr = new FemDataSender("localhost", "13");
  DataSender *sendPtr = new DataSender();
  sendPtr->setupSocket("127.0.0.1", 7273, "localhost", 9876);

  std::cout << "*** End of DataSender test" << std::endl;
  
  return 0;
}


