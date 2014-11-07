/*
 * configFileParser.c
 *
 *  Created on: Nov 7, 2014
 *      Author: tcn45
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configini.h"
#include "configFileParser.h"

void initialiseNodeConfig(nodeConfig* config)
{
	config->mode = rx_mode;
	config->cpuNum = -1;
	config->rxPort = 8989;
	config->txPort = 9999;
	config->txBlockSize = 8192;
	config->txBlockPause = 0;
	config->txNumDests = 1;

	int i;
	for (i = 0; i < TX_MAX_DESTS; i++)
	{
		snprintf(config->txDestAddr[i], TX_MAX_ADDR_SIZE, "127.0.0.1");
		config->txDestPort[i] = 8989+i;
	}
}

int parseConfigFile(char* configFileName, int node, nodeConfig* config)
{

	int rc = CONFIG_PARSE_OK;

	Config *cfg = NULL;
	ConfigRet ret;

	initialiseNodeConfig(config);

	printf("Parsing config file %s for node %d\n", configFileName, node);

	char nodeSection[MAX_SECTION_NAME_SIZE];
	sprintf(nodeSection, "node%d", node);

	/* Read the configuration file */
	ret = ConfigReadFile(configFileName, &cfg);
	if (ret != CONFIG_OK)
	{
		printf("ERROR: config file read failed for %s: ret=%d\n", configFileName, ret);
		rc = CONFIG_PARSE_ERROR;
	}
	else
	{

		// Parse node mode string from config and set accordingly
		char modeString[MAX_NODE_MODE_SIZE];
		ret = ConfigReadString(cfg, nodeSection, "mode", modeString, MAX_NODE_MODE_SIZE, "none");
		if (ret != CONFIG_OK)
		{
			printf("ERROR: failed to read mode for node %d from config file %s\n",
					node, configFileName);
			rc = CONFIG_PARSE_ERROR;
		}
		else
		{
			if (strcmp(modeString, "rx") == 0) {
				config->mode = rx_mode;
			}
			else if (strcmp(modeString, "tx") == 0) {
				config->mode = tx_mode;
			}
			else {
				printf("ERROR: unrecognised mode for node %d from config file %s: %s\n",
						node, configFileName, modeString);
				rc = CONFIG_PARSE_ERROR;
			}
		}

		// Parse CPU number (for affinity)
		ret = ConfigReadInt(cfg, nodeSection, "cpuNum", &(config->cpuNum), config->cpuNum);

		// Parse remaining parameters based on mode
		switch (config->mode) {
		case rx_mode:
			ret = ConfigReadInt(cfg, nodeSection, "rxPort", &(config->rxPort), config->rxPort);
			break;

		case tx_mode:
			ret = ConfigReadInt(cfg, nodeSection, "txPort", &(config->txPort), config->txPort);
			ret = ConfigReadInt(cfg, nodeSection, "txBlockSize", &(config->txBlockSize), config->txBlockSize);
			ret = ConfigReadInt(cfg, nodeSection, "txBlockPause", &(config->txBlockPause), config->txBlockPause);
			ret = ConfigReadInt(cfg, nodeSection, "txNumDests", &(config->txNumDests), config->txNumDests);
			int dest;
			for (dest = 0; dest < config->txNumDests; dest++)
			{
				char txDestAddrName[10];
				sprintf(txDestAddrName, "txDestAddr%d", dest);
				ret = ConfigReadString(cfg, nodeSection, txDestAddrName, config->txDestAddr[dest], 10, config->txDestAddr[dest]);
				char txDestPortName[10];
				sprintf(txDestPortName, "txDestPort%d", dest);
				ret = ConfigReadInt(cfg, nodeSection, txDestPortName, &(config->txDestPort[dest]), config->txDestPort[dest]);
			}

			break
			;
		default:
			// Shouldn't happen - should be trapped by mode string parsing above
			printf("ERROR: node %d mode is set to illegal value %d", node, config->mode);
			rc = CONFIG_PARSE_ERROR;
			break;
		}


		ConfigFree(cfg);
	}

	return rc;
}

