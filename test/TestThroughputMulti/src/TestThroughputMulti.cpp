/*
 * TestThroughputMulti.cpp
 *
 *  Created on: Dec 10, 2014
 *      Author: tcn45
 */

#include "TestThroughputMulti.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <iterator>
using namespace std;

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
namespace pt = boost::property_tree;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sched.h>

// The packet as a structure
#define PAYLOAD_SIZE 8192
struct packet_t
{
	long frameNumber;
	long packetNumber;
	char payload[PAYLOAD_SIZE];
};

// How big is the Percival frame in bytes.
#define BYTES_PER_FRAME (4096.0*4096.0*4.0)

// How many bytes in a megabyte
#define BYTES_PER_MEGABYTE (1024.0*1024.0)

// How many nanoseconds in a second
#define NANOSECONDS_PER_SECOND 1000000000

bool TestThroughputMulti::mRunEventLoop = true;

TestThroughputMulti::TestThroughputMulti(void) :
		mNode(1)
{

	TestThroughputMulti::mRunEventLoop = true;

	// Retreive a logger instance
	mLogger = Logger::getLogger("TestThroughputMulti");
}


TestThroughputMulti::~TestThroughputMulti()
{

}

int TestThroughputMulti::parseArguments(int argc, char** argv)
{

	int rc = 0;
	try
	{
		// Create and parse options
		po::options_description prog_desc("Program options");
		prog_desc.add_options()
				("help",
					"Print this help message")
				("node,n",      po::value<unsigned int>()->default_value(1),
					"Set node ID")
				("config,c",    po::value<string>(),
					"Specify program configuration file")
				("logconfig,l", po::value<string>(),
					"Specify log4cxx configuration file");

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, prog_desc), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			cout << prog_desc << endl;
			exit(0);
		}
		if (vm.count("logconfig"))
		{
			PropertyConfigurator::configure(vm["logconfig"].as<string>());
			LOG4CXX_DEBUG(mLogger, "log4cxx config file is set to " << vm["logconfig"].as<string>());
		}
		if (vm.count("node"))
		{
			mNode = vm["node"].as<unsigned int>();
			LOG4CXX_DEBUG(mLogger, "Node ID is set to " << mNode);
		}
		if (vm.count("config"))
		{
			this->parseConfigFile(vm["config"].as<string>());
		}
	}
	catch (Exception &e)
	{
		LOG4CXX_ERROR(mLogger, "Got Log4CXX exception: " << e.what());
		rc = 1;
	}
	catch (exception &e)
	{
		LOG4CXX_ERROR(mLogger, "Got exception:" << e.what());
		rc = 1;
	}
	catch (...)
	{
		LOG4CXX_ERROR(mLogger, "Exception of unknown type!");
		rc = 1;
	}

	return rc;

}

void TestThroughputMulti::parseConfigFile(const string& configFileName )
{

	LOG4CXX_DEBUG(mLogger, "Parsing configuration file " << configFileName);

	// Parse the configuration INI file into a property tree
	pt::ptree pt;
	pt::ini_parser::read_ini(configFileName, pt);

	// Parse parameters from global section - these can be overridden on a node by node
	// basis in the node section below
	const int unsetGlobalReportInterval = -1;
	int globalReportInterval = pt.get("global.reportInterval", unsetGlobalReportInterval);
	if (globalReportInterval != unsetGlobalReportInterval)
	{
		LOG4CXX_DEBUG(mLogger, "Global reporting interval set to " << globalReportInterval);
		mNodeConfig.reportInterval = globalReportInterval;
	}

	// Build the node section name for all parameters
	ostringstream nodeSectionStrm;
	nodeSectionStrm << "node" << mNode << ".";
	string nodeSection = nodeSectionStrm.str();

	// Parse CPU number for affinity setting
	mNodeConfig.cpuNum = pt.get(nodeSection + "cpuNum", mNodeConfig.cpuNum);
	LOG4CXX_DEBUG(mLogger, "CPU affinity set to " << mNodeConfig.cpuNum);

	// Parse the mode setting
	string modeString = pt.get(nodeSection + "mode", "rx");
	if (modeString == "rx")
	{
		mNodeConfig.mode = NodeConfig::rx_mode;
	}
	else if (modeString == "tx")
	{
		mNodeConfig.mode = NodeConfig::tx_mode;
	}
	else
	{
		LOG4CXX_ERROR(mLogger, "Unrecognised mode parameter: " << modeString << ", defaulting to rx");
		mNodeConfig.mode = NodeConfig::rx_mode;
	}
	LOG4CXX_DEBUG(mLogger, "Mode set to " << modeString << " (" << (int)mNodeConfig.mode << ")");

	// Parse packet reporting interval
	mNodeConfig.reportInterval = pt.get(nodeSection + "reportInterval", mNodeConfig.reportInterval);
	LOG4CXX_DEBUG(mLogger, "Reporting interval set to " << mNodeConfig.reportInterval);

	// Parse remaining parameters based on mode
	switch (mNodeConfig.mode)
	{
	case NodeConfig::rx_mode:

		mNodeConfig.rxPort = pt.get(nodeSection + "rxPort", mNodeConfig.rxPort);
		break;

	case NodeConfig::tx_mode:

		mNodeConfig.txPort       = pt.get(nodeSection + "txPort",       mNodeConfig.txPort);
		mNodeConfig.txBlockSize  = pt.get(nodeSection + "txBlockSize",  mNodeConfig.txBlockSize);
		mNodeConfig.txBlockPause = pt.get(nodeSection + "txBlockPause", mNodeConfig.txBlockPause);
		mNodeConfig.txNumDests   = pt.get(nodeSection + "txNumDests",   mNodeConfig.txNumDests);

		LOG4CXX_DEBUG(mLogger, "TX port set to " << mNodeConfig.txPort);
		LOG4CXX_DEBUG(mLogger, "TX block size set to " << mNodeConfig.txBlockSize);
		LOG4CXX_DEBUG(mLogger, "TX block pause set to " << mNodeConfig.txBlockPause);
		LOG4CXX_DEBUG(mLogger, "TX number of destinations set to " << mNodeConfig.txNumDests);

		for (int dest = 0; dest < mNodeConfig.txNumDests; dest++)
		{
			ostringstream destAddr;
			destAddr << nodeSection << "txDestAddr" << dest;
			mNodeConfig.txDestAddr[dest] = pt.get(destAddr.str(), mNodeConfig.txDestAddr[dest]);

			ostringstream destPort;
			destPort << nodeSection << "txDestPort" << dest;
			mNodeConfig.txDestPort[dest] = pt.get(destPort.str(), mNodeConfig.txDestPort[dest]);

			LOG4CXX_DEBUG(mLogger, "TX destination " << dest << " set to address "
					<< mNodeConfig.txDestAddr[dest] << " port " << mNodeConfig.txDestPort[dest]);
		}
		break;

	default:
		LOG4CXX_ERROR(mLogger, "Mode set to illegal value: " << mNodeConfig.mode << " (" << modeString << ")");
		break;
	}
}

void TestThroughputMulti::run(void)
{

	// If CPU affinity requested (cpuNum != -1), set it
	if (mNodeConfig.cpuNum != -1)
	{
		this->setCpuAffinity();
	}

	// Set scheduler to SCHED_FIFO for current process
	this->setSchedFifo();

	switch (mNodeConfig.mode)
	{
	case NodeConfig::rx_mode:

		this->runReceiver();
		break;

	case NodeConfig::tx_mode:

		this->runGenerator();
		break;

	default:
		LOG4CXX_ERROR(mLogger, "Cannot run tests, illegal mode set: " << mNodeConfig.mode);
		break;
	}
}

void TestThroughputMulti::stop(void)
{
	TestThroughputMulti::mRunEventLoop = false;
}

/* Set the CPU affinity of the current process.
 */
void TestThroughputMulti::setCpuAffinity(void)
{
	LOG4CXX_INFO(mLogger, "Setting CPU affinity to " << mNodeConfig.cpuNum);

	cpu_set_t mask;
	CPU_ZERO_S(sizeof(mask), &mask);
	CPU_SET_S(mNodeConfig.cpuNum, sizeof(mask), &mask);
	int status = sched_setaffinity(0, sizeof(mask), &mask);

	if (status != 0)
	{
		perror("sched_setaffinity");
		LOG4CXX_WARN(mLogger, "Cannot set CPU affinity: " << strerror(errno));
	}
}


// Set the current process to SCHED_FIFO

void TestThroughputMulti::setSchedFifo(void)
{
	LOG4CXX_INFO(mLogger, "Setting scheduling policy to SCHED_FIFO");

	struct sched_param param;
	param.__sched_priority = 10;
	int status = sched_setscheduler(0 /*this process*/, SCHED_FIFO, &param);

	if (status != 0)
	{
		LOG4CXX_WARN(mLogger, "Cannot set scheduler to SCHED_FIFO: " << strerror(errno));
	}
}

// Calculate the difference, in seconds, between two timespecs
double TestThroughputMulti::timeDiff(struct timespec* start, struct timespec* end)
{

	double startNs = ((double)start->tv_sec * NANOSECONDS_PER_SECOND) + start->tv_nsec;
	double endNs   = ((double)  end->tv_sec * NANOSECONDS_PER_SECOND) +   end->tv_nsec;

	return (endNs - startNs) / NANOSECONDS_PER_SECOND;
}

void TestThroughputMulti::runGenerator(void)
{

	ostringstream destInfo;
	destInfo << "Starting generator sending to " <<  mNodeConfig.txNumDests << " destination"
			<< ((mNodeConfig.txNumDests == 1) ? "" : "s") << ": ";
	for (int dest = 0; dest < mNodeConfig.txNumDests; dest++) {
		destInfo << mNodeConfig.txDestAddr[dest] << ":" << mNodeConfig.txDestPort[dest];
		destInfo << (dest == mNodeConfig.txNumDests-1) ? "" : ", ";
	}
	LOG4CXX_INFO(mLogger,destInfo.str());


	// Initialise the packet
    struct packet_t aPacket;
	memset(aPacket.payload, 0x55, PAYLOAD_SIZE);
	aPacket.frameNumber  = 0;
	aPacket.packetNumber = 0;

	// Open a socket for the transmission
	int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (handle < 0)
	{
		printf("Failed to open socket\n");
		return;
	}

	// Bind socket to outgoing port
	struct sockaddr_in sockSrc;
	memset(&sockSrc, 0, sizeof(sockSrc));

    sockSrc.sin_family = AF_INET;
    sockSrc.sin_port = htons(mNodeConfig.txPort);
    sockSrc.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(handle, (struct sockaddr*)&sockSrc, sizeof(sockSrc)) == -1)
    {
    	LOG4CXX_ERROR(mLogger, "Failed to bind socket: " << strerror(errno));
    	return;
    }

    // Create the list of destination socket addresses
    struct sockaddr_in sockDest[TX_MAX_DESTS];
    for (int dest = 0; dest < mNodeConfig.txNumDests; dest++)
    {
		memset(&(sockDest[dest]), 0, sizeof(sockDest[dest]));
		sockDest[dest].sin_family = AF_INET;
		sockDest[dest].sin_port = htons(mNodeConfig.txDestPort[dest]);
		if (inet_aton((mNodeConfig.txDestAddr[dest]).c_str(), &sockDest[dest].sin_addr) == 0)
		{
			LOG4CXX_ERROR(mLogger, "Invalid IP address specified for destination " << dest << " : " << mNodeConfig.txDestAddr[dest]);
			return;
		}
    }

    // Initialise tracking information
    long txFailCount = 0;
    long goodPacketCount = 0;
    long totalPacketCount = 0;
    long packetsToNextStats = mNodeConfig.reportInterval;

    struct timespec startTime;
    clock_gettime(CLOCK_REALTIME, &startTime);


	int dest = 0;

	// Send packets until killed
    while (TestThroughputMulti::mRunEventLoop)
    {
    	// Blast a blocks worth of packets at the current destination
    	for(int pkt=0; pkt < mNodeConfig.txBlockSize; pkt++)
    	{
			if (sendto(handle, &aPacket, sizeof(aPacket), 0,
					(struct sockaddr*)&(sockDest[dest]), sizeof(sockDest[dest])) == -1)
			{
				txFailCount++;
			}
			else
			{
				goodPacketCount++;
			}
			aPacket.packetNumber++;
			totalPacketCount++;

			if (mNodeConfig.reportInterval)
			{
				packetsToNextStats--;

				// Time to print stats?
				if (packetsToNextStats <= 0)
				{

					struct timespec nowTime;
					clock_gettime(CLOCK_REALTIME, &nowTime);

					double period = timeDiff(&startTime, &nowTime);
					double bps = (double)(mNodeConfig.reportInterval*sizeof(aPacket))/period;

					LOG4CXX_INFO(mLogger, "TX stats: TotalPkts=" << totalPacketCount
							<< " TxFail=" << txFailCount
							<< " MBps=" << fixed << setprecision(2) << bps/BYTES_PER_MEGABYTE
							<< " FPS="  << fixed << setprecision(2) << bps/BYTES_PER_FRAME);

					packetsToNextStats = mNodeConfig.reportInterval;
					clock_gettime(CLOCK_REALTIME, &startTime);
				}
			}
    	}

    	aPacket.frameNumber++;

    	// Increment destination counter modulo number of destinations
    	dest++;
    	if (dest == mNodeConfig.txNumDests) { dest = 0; };

        // Wait a bit before sending the next
        struct timespec sleepTime;

        sleepTime.tv_nsec = mNodeConfig.txBlockPause % NANOSECONDS_PER_SECOND;
        sleepTime.tv_sec = mNodeConfig.txBlockPause / NANOSECONDS_PER_SECOND;
        nanosleep(&sleepTime, 0);
    }
    LOG4CXX_INFO(mLogger, "Event loop terminated, total packets sent = " << totalPacketCount);

}

void TestThroughputMulti::runReceiver(void)
{

	LOG4CXX_INFO(mLogger, "Starting receiver listening on port " << mNodeConfig.rxPort);

	// Open a socket for reception
	int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (handle < 0)
	{
		LOG4CXX_ERROR(mLogger, "Failed to open socket: " << strerror(errno));
		return;
	}

	// Set the socket receive buffer size
	int bufferSize = 30000000;
	if (setsockopt(handle, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0)
	{
		LOG4CXX_ERROR(mLogger, "Failed to set receive buffer size: " << strerror(errno));
	}

	// Read it back and display
	socklen_t len;
	getsockopt(handle, SOL_SOCKET, SO_RCVBUF, &bufferSize, &len);
	LOG4CXX_DEBUG(mLogger, "Receive buffer size is " << bufferSize);

	// Set a timeout on the socket so we can be responsive to interruption
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0; // 10000;
	if (setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		LOG4CXX_ERROR(mLogger, "Failed to set timeout on socket: " << strerror(errno));
	}

	// Bind the socket to the specified port
    struct sockaddr_in siMe;
	memset(&siMe, 0, sizeof(siMe));
    siMe.sin_family = AF_INET;
    siMe.sin_port = htons(mNodeConfig.rxPort);
    siMe.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(handle, (struct sockaddr*)&siMe, sizeof(siMe)) == -1)
    {
    	LOG4CXX_ERROR(mLogger, "Failed to bind socket: " << strerror(errno));
    	return;
    }

    // Initialise tracking information
    long lastFrameNumber = -1;
    long nextPacketNumber = 0;
    long rxFailCount = 0;
    long packetWrongSizeCount = 0;
    long lossEventCount = 0;
    long lostPacketCount = 0;
    long latePacketNumberCount = 0;
    long goodPacketCount = 0;
    long totalPacketCount = 0;
    long packetsToNextStats = mNodeConfig.reportInterval;

    struct timespec startTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    // Receive packets until killed
    while (TestThroughputMulti::mRunEventLoop)
    {
        struct packet_t aPacket;

    	// Wait for a packet
        ssize_t rxSize = recvfrom(handle, &aPacket, sizeof(aPacket), 0, 0, 0);

        // Check for faults
        if (rxSize == -1)
        {
           	if (TestThroughputMulti::mRunEventLoop == false) break;
           	if (errno == EAGAIN) continue;

			rxFailCount++;
        }
        else if (rxSize != sizeof(aPacket))
        {
        	packetWrongSizeCount++;
        }
        else
        {
        	if (aPacket.frameNumber != lastFrameNumber)
        	{
        		lastFrameNumber = aPacket.frameNumber;
        		nextPacketNumber = aPacket.packetNumber;
        	}
			if (aPacket.packetNumber > nextPacketNumber)
			{
				long delta = aPacket.packetNumber - nextPacketNumber;
				lostPacketCount += delta;
				lossEventCount++;

				LOG4CXX_WARN(mLogger, "Packet loss detected: got packet num " << aPacket.packetNumber
						<< " expected num " << nextPacketNumber
						<< " delta " << delta
						<< " loss events " << lossEventCount
						<< " total loss count " << lostPacketCount);

				nextPacketNumber = aPacket.packetNumber;
			}
			else if (aPacket.packetNumber < nextPacketNumber)
			{
				latePacketNumberCount++;
				nextPacketNumber = aPacket.packetNumber;
			}
			else
			{
				goodPacketCount++;
			}
			// Report on first packet so there is some feedback if reporting is heavily suppressed
			if (totalPacketCount == 0)
			{
				LOG4CXX_INFO(mLogger, "First packet detected, receiving data");
			}
        }
        nextPacketNumber++;
        totalPacketCount++;

        if (mNodeConfig.reportInterval)
        {
        	packetsToNextStats--;

			// Time to print stats?
			if (packetsToNextStats <= 0)
			{
				struct timespec nowTime;
				clock_gettime(CLOCK_REALTIME, &nowTime);
				double period = this->timeDiff(&startTime, &nowTime);

				double bps = (double)(mNodeConfig.reportInterval * sizeof(aPacket))/period;

				LOG4CXX_INFO(mLogger, "RX stats: TotalPkts=" << totalPacketCount
						<< " LossEvts=" << lossEventCount
						<< " LostPkts=" << lostPacketCount
						<< " WrongSize=" << packetWrongSizeCount
						<< " RxFail=" << rxFailCount
						<< " Late=" << latePacketNumberCount
						<< " MBps=" << fixed << setprecision(2) << bps/BYTES_PER_MEGABYTE
						<< " FPS="  << fixed << setprecision(2) << bps/BYTES_PER_FRAME);

				packetsToNextStats = mNodeConfig.reportInterval;
				clock_gettime(CLOCK_REALTIME, &startTime);
			}
        }
    }

    LOG4CXX_INFO(mLogger, "Event loop terminated, total packets received = " << totalPacketCount);
}
