/*
 * PercivalDebug.cpp
 *
 *  Created on: 26th Aug 2013
 *      Author: gnx91527
 */

#include "PercivalDebug.h"
#include <boost/exception/diagnostic_information.hpp>

boost::mutex *PercivalDebug::access_ = new boost::mutex();

PercivalDebug::PercivalDebug(uint32_t level, const std::string& functionName)
	: level_(level),
    functionName_(functionName)
{
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
  startTime_ = now.time_of_day().total_microseconds();
  if (level_ > 2){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] ==> Entering " << functionName_ << std::endl;
  }
}

PercivalDebug::~PercivalDebug()
{
  if (level_ > 2){
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    long duration = now.time_of_day().total_microseconds() - startTime_;
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " duration: " << duration << " usec" << std::endl;
    std::cout << "[DEBUG] <== Exiting " << functionName_ << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::stringstream& str)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str.str() << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::string& str)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, boost::exception& e)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - Exception: " << diagnostic_information(e) << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::string& str, uint32_t val)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str << " [" << val << "]" << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::string& str, uint16_t val)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str << " [" << val << "]" << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::string& str, uint8_t val)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str << " [" << val << "]" << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::string& str, int64_t val)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str << " [" << val << "]" << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::string& str1, uint32_t val1, const std::string& str2, uint32_t val2)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str1 << " [" << val1 << "] " << str2 << " [" << val2 << "]" << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::string& str, const std::string& val)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str << " [" << val << "]" << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::string& str, boost::thread::id val)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str << " [" << val << "]" << std::endl;
  }
}

void PercivalDebug::log(uint32_t level, const std::string& str, bool val)
{
  if (level_ >= level){
    boost::lock_guard<boost::mutex> lock(*PercivalDebug::access_);
    std::cout << "[DEBUG] " << functionName_ << " - " << str << " [" << val << "]" << std::endl;
  }
}





