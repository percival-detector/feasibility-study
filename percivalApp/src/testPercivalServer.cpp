
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

    PercivalServer *sPtr = new PercivalServer();
    //sPtr->setDebug(5);
    sPtr->setupFullFrame(2048, 1024, UnsignedInt16, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    sPtr->setupSubFrame(0, "127.0.0.1", 9876, 4, 4, 8, 8, 1);
    //sPtr->setupSubFrame(1, "127.0.0.1", 9877, 0, 256, 2047, 511, 2);
    //sPtr->setupSubFrame(3, "127.0.0.1", 9878, 0, 512, 2047, 767, 2);

    sPtr->startAcquisition();

    while (true){
      usleep(10000);
    }

//    sPtr->releaseSubFrame(1);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
