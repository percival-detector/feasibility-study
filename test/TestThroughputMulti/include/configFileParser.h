/*
 * configFileParser.h
 *
 *  Created on: Nov 7, 2014
 *      Author: tcn45
 */

#ifndef CONFIGFILEPARSER_H_
#define CONFIGFILEPARSER_H_

#define CONFIG_PARSE_OK    0
#define CONFIG_PARSE_ERROR 1

#define MAX_SECTION_NAME_SIZE 16
#define MAX_NODE_MODE_SIZE 4

#define TX_MAX_DESTS 8
#define TX_MAX_ADDR_SIZE 18

typedef enum
{
	rx_mode = 0,
	tx_mode = 1
} nodeMode;

typedef struct
{
	nodeMode mode;
	int      cpuNum;
	int      txPort;
	int      txBlockSize;
	int      txBlockPause;
	int      txNumDests;
	char     txDestAddr[TX_MAX_DESTS][TX_MAX_ADDR_SIZE];
	int      txDestPort[TX_MAX_DESTS];
	int      rxPort;

} nodeConfig;

int parseConfigFile(char* configFileName, int node, nodeConfig* config);

#endif /* CONFIGFILEPARSER_H_ */
