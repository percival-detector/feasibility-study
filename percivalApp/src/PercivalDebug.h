/*
 * PercivalDebug.h
 *
 *  Created on: 26th Aug 2013
 *      Author: gnx91527
 *
 */

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#ifndef PERCIVALDEBUG_H_
#define PERCIVALDEBUG_H_

class PercivalDebug
{
  public:

	  PercivalDebug(uint32_t level, const std::string& functionName);

    virtual ~PercivalDebug();

    void log(uint32_t level, const std::stringstream& str);

    void log(uint32_t level, const std::string& str);

    void log(uint32_t level, boost::exception& e);

    void log(uint32_t level, const std::string& str, uint32_t val);

    void log(uint32_t level, const std::string& str, int64_t val);

    void log(uint32_t level, const std::string& str1, uint32_t val1, const std::string& str2, uint32_t val2);

    void log(uint32_t level, const std::string& str, const std::string& val);

    void log(uint32_t level, const std::string& str, boost::thread::id val);

    void log(uint32_t level, const std::string& str, bool val);

  private:

    uint32_t            level_;        // Debug level
    std::string         functionName_; // Name of calling function
    long                startTime_;    // Time debug object created
    static boost::mutex *access_;       // Mutex to ensure lines printed correctly
};

#endif  /* PERCIVALDEBUG_H_ */
