/*
 * TestThroughputMulti.h
 *
 *  Created on: Dec 10, 2014
 *      Author: tcn45
 */

#ifndef TESTTHROUGHPUTMULTI_H_
#define TESTTHROUGHPUTMULTI_H_

#include <string>
using namespace std;

#include <time.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;

#include "nodeConfig.h"

class TestThroughputMulti
{
public:
	TestThroughputMulti();
	~TestThroughputMulti();

	int parseArguments(int argc, char** argv);
	void parseConfigFile(const string& configFileName);

	void run(void);
	static void stop(void);

private:

	void setCpuAffinity(void);
	void setSchedFifo(void);
	double timeDiff(struct timespec* start, struct timespec* end);

	void runGenerator(void);
	void runReceiver(void);

	LoggerPtr    mLogger;
	unsigned int mNode;
	NodeConfig   mNodeConfig;

	static bool  mRunEventLoop;;
};



#endif /* TESTTHROUGHPUTMULTI_H_ */
