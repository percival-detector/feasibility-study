/*
 * nodeConfig.h
 *
 *  Created on: Dec 9, 2014
 *      Author: tcn45
 */

#ifndef NODECONFIG_H_
#define NODECONFIG_H_

#include <string>
using namespace std;

#define TX_MAX_DESTS 8
#define TX_MAX_ADDR_SIZE 18

class NodeConfig
{
public:
	enum nodeMode
	{
		rx_mode = 0,
		tx_mode = 1
	};

	NodeConfig()
	{
		this->mode = rx_mode;
		this->cpuNum = -1;
		this->rxPort = 8989;
		this->txPort = 9999;
		this->txBlockSize = 8192;
		this->txBlockPause = 0;
		this->txNumDests = 1;

		for (int i = 0; i < TX_MAX_DESTS; i++)
		{
			this->txDestAddr[i] = "127.0.0.1";
			this->txDestPort[i] = 8989+i;
		}

		this->reportInterval = 0;
	}

	nodeMode mode;
	int      cpuNum;
	int      txPort;
	int      txBlockSize;
	int      txBlockPause;
	int      txNumDests;
	string   txDestAddr[TX_MAX_DESTS];
	int      txDestPort[TX_MAX_DESTS];
	int      rxPort;
	int      reportInterval;
};



#endif /* NODECONFIG_H_ */
