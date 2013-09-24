
#include <ctime>
#include <iostream>
#include <string>
#include <iomanip>

#include "PercivalPacketChecker.h"

int main()
{
  PercivalPacketChecker *cPtr = new PercivalPacketChecker();
  std::cout << "Init 4 parcels of 8 packets..." << std::endl;
  cPtr->init(4, 8);
  cPtr->report();
  std::cout << "Checking : " << cPtr->checkAll() << std::endl;
  std::cout << "Setting parcel 2, packet 3" << std::endl;
  cPtr->set(2, 3);
  cPtr->report();
  std::cout << "Checking : " << cPtr->checkAll() << std::endl;
  std::cout << "Checking (2, 3): " << cPtr->check(2, 3) << std::endl;
  std::cout << "Setting parcel 1" << std::endl;
  cPtr->set(1, 0);
  cPtr->set(1, 1);
  cPtr->set(1, 2);
  cPtr->set(1, 3);
  cPtr->set(1, 4);
  cPtr->set(1, 5);
  cPtr->set(1, 6);
  cPtr->set(1, 7);
  cPtr->report();
  std::cout << "Checking : " << cPtr->checkAll() << std::endl;
  std::cout << "Setting parcel 0" << std::endl;
  cPtr->set(0, 0);
  cPtr->set(0, 1);
  cPtr->set(0, 2);
  cPtr->set(0, 3);
  cPtr->set(0, 4);
  cPtr->set(0, 5);
  cPtr->set(0, 6);
  cPtr->set(0, 7);
  cPtr->report();
  std::cout << "Checking : " << cPtr->checkAll() << std::endl;
  int ret = 0;
  std::cout << "Setting parcel 2" << std::endl;
  ret = cPtr->set(2, 0);
  std::cout << "Return value: " << ret << std::endl;
  ret = cPtr->set(2, 1);
  std::cout << "Return value: " << ret << std::endl;
  ret = cPtr->set(2, 2);
  std::cout << "Return value: " << ret << std::endl;
  ret = cPtr->set(2, 3);
  std::cout << "Return value: " << ret << std::endl;
  ret = cPtr->set(2, 4);
  std::cout << "Return value: " << ret << std::endl;
  ret = cPtr->set(2, 5);
  std::cout << "Return value: " << ret << std::endl;
  ret = cPtr->set(2, 6);
  std::cout << "Return value: " << ret << std::endl;
  ret = cPtr->set(2, 7);
  std::cout << "Return value: " << ret << std::endl;
  cPtr->report();
  std::cout << "Checking : " << cPtr->checkAll() << std::endl;
  std::cout << "Setting parcel 3" << std::endl;
  cPtr->set(3, 0);
  cPtr->set(3, 1);
  cPtr->set(3, 2);
  cPtr->set(3, 3);
  cPtr->set(3, 4);
  cPtr->set(3, 5);
  cPtr->set(3, 6);
  cPtr->set(3, 7);
  cPtr->report();
  std::cout << "Checking : " << cPtr->checkAll() << std::endl;
  std::cout << "Resetting..." << std::endl;
  cPtr->resetAll();
  cPtr->report();
  std::cout << "Checking : " << cPtr->checkAll() << std::endl;



  return 0;
}
