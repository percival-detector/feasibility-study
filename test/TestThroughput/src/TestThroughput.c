/*
 ============================================================================
 Name        : TestThroughput.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* The packet as a structure
 */
#define PAYLOAD_SIZE 8192
struct packet_t
{
	long packetNumber;
	char payload[PAYLOAD_SIZE];
};

/* How often to print stats
 */
#define STATS_INTERVAL 10000

/* How big is the Percival frame in bytes.
 */
#define BYTES_PER_FRAME (4096.0*4096.0*4.0)

/* How many bytes in a megabyte
 */
#define BYTES_PER_MEGABYTE (1024.0*1024.0)

/* How many nanoseconds in a second
 */
#define NANOSECONDS_PER_SECOND 1000000000

/* Calculate the difference, in seconds, between
 * two timevals.
 */
double timeDiff(struct timeval* start, struct timeval* end)
{
	long seconds;
	long microseconds;
	seconds = end->tv_sec - start->tv_sec;
	microseconds = end->tv_usec - start->tv_usec;
	return (double)seconds + (double)microseconds/1000000.0;
}

/* Set the CPU affinity of the current process.
 */
void setCpuAffinity(int cpuNum)
{
	cpu_set_t mask;
	int status;
	printf("Setting CPU affinity to %d\n", cpuNum);
	CPU_ZERO_S(sizeof(mask), &mask);
	CPU_SET_S(cpuNum, sizeof(mask), &mask);
	status = sched_setaffinity(0, sizeof(mask), &mask);
	if(status != 0)
	{
		perror("sched_setaffinity");
	}
}

/* Set the current process to SCHED_FIFO
 */
void setSchedFifo(void)
{
	struct sched_param param;
	int status;
	printf("Setting scheduling policy to SCHED_FIFO\n");
	param.__sched_priority = 10;
	status = sched_setscheduler(0 /*this process*/, SCHED_FIFO, &param);
	if(status != 0)
	{
		perror("sched_setscheduler");
	}
}

/* Generate data and send it to the specified UDP
 * address and port forever.
 */
void doGenerator(const char* ipAddress, int port, int blockSize, int blockPause)
{
    struct sockaddr_in siTarget;
    struct sockaddr_in siMe;
    int handle;
    struct packet_t aPacket;
    long txFailCount;
    long goodPacketCount;
    long totalPacketCount;
    long packetsToNextStats;
    struct timeval startTime;
    struct timeval nowTime;
    double period;
    double bps;
    struct timespec sleepTime;
    int i;

    // Banner
	printf("Starting generator sending to %s, port %d...\n", ipAddress, port);
	setSchedFifo();

	// Initialise the packet
	memset(aPacket.payload, 0x55, PAYLOAD_SIZE);
	aPacket.packetNumber = 0;

	// Open a socket for the transmission
	handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(handle < 0)
	{
		printf("Failed to open socket\n");
		return;
	}
	memset(&siMe, 0, sizeof(siMe));
    siMe.sin_family = AF_INET;
    siMe.sin_port = htons(port);
    siMe.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(handle, (struct sockaddr*)&siMe, sizeof(siMe)) == -1)
    {
    	printf("Failed to bind socket\n");
    	return;
    }
	memset(&siTarget, 0, sizeof(siTarget));
	siTarget.sin_family = AF_INET;
	siTarget.sin_port = htons(port);
    if(inet_aton(ipAddress, &siTarget.sin_addr) == 0)
    {
    	printf("Not an IP address\n");
        return;
    }

#if 0
    // *** Start of raw ethernet socket trial, doesn't work 'cos opening
    // *** the socket in this mode requires root privileges.
    // Open a raw ethernet socket for transmission
    {
		int rawHandle;
		struct ifreq buffer;
		int ifindex;
		unsigned char source[ETH_ALEN];
		struct sockaddr_ll saddrll;
		unsigned char dest[] = { 0x00, 0x02, 0xC9, 0xEE, 0x14, 0xF1 };
		rawHandle = socket(AF_PACKET, SOCK_RAW, htons(0x88a3));
		if(rawHandle < 0)
		{
			printf("Failed to open raw handle %d\n", (int)errno);
		}
		memset(&buffer, 0x00, sizeof(buffer));
		strncpy(buffer.ifr_name, "p2p2", IFNAMSIZ);
		ioctl(rawHandle, SIOCGIFINDEX, &buffer);
		ifindex = buffer.ifr_ifindex;
		ioctl(rawHandle, SIOCGIFHWADDR, &buffer);
		memcpy((void*)source, (void*)(buffer.ifr_hwaddr.sa_data),
			   ETH_ALEN);
		memset((void*)&saddrll, 0, sizeof(saddrll));
		saddrll.sll_family = PF_PACKET;
		saddrll.sll_ifindex = ifindex;
		saddrll.sll_halen = ETH_ALEN;
		memcpy((void*)(saddrll.sll_addr), (void*)dest, ETH_ALEN);
    }
#endif

    // Initialise tracking information
    txFailCount = 0;
    goodPacketCount = 0;
    totalPacketCount = 0;
    packetsToNextStats = STATS_INTERVAL;
    gettimeofday(&startTime, 0);
	printf(" Packets   TxFail     MBps      Fps\n");

	// Send packets until killed
    while(1)
    {
    	// Blast a blocks worth of packets at the interface
    	for(i=0; i<blockSize; i++)
    	{
			if(sendto(handle, &aPacket, sizeof(aPacket), 0,
					(struct sockaddr*)&siTarget, sizeof(siTarget)) == -1)
			{
				txFailCount++;
			}
			else
			{
				goodPacketCount++;
			}
			aPacket.packetNumber++;
			totalPacketCount++;
			packetsToNextStats--;

			// Time to print stats?
			if(packetsToNextStats <= 0)
			{
				packetsToNextStats = STATS_INTERVAL;
				gettimeofday(&nowTime, 0);
				period = timeDiff(&startTime, &nowTime);
				bps = (double)(STATS_INTERVAL*sizeof(aPacket))/period;
				printf("%8ld %8ld %8.2f %8.2f\n",
						totalPacketCount, txFailCount,
						bps/BYTES_PER_MEGABYTE, bps/BYTES_PER_FRAME);
				gettimeofday(&startTime, 0);
			}
    	}

        // Wait a bit before sending the next
        sleepTime.tv_nsec = blockPause % NANOSECONDS_PER_SECOND;
        sleepTime.tv_sec = blockPause / NANOSECONDS_PER_SECOND;
        nanosleep(&sleepTime, 0);
    }
}

/* Receive packets forever.
 */
void doProcessor(int port)
{
    struct sockaddr_in siMe;
    int handle;
    struct packet_t aPacket;
    ssize_t rxSize;
    long nextPacketNumber;
    long rxFailCount;
    long packetWrongSizeCount;
    long missingPacketNumberCount;
    long latePacketNumberCount;
    long goodPacketCount;
    long totalPacketCount;
    long packetsToNextStats;
    struct timeval startTime;
    struct timeval nowTime;
    double period;
    double bps;
    int bufferSize;
	socklen_t len;

    // Banner
	printf("Starting processor listening on port %d\n", port);
	setSchedFifo();

	// Open a socket for reception
	handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(handle < 0)
	{
		printf("Failed to open socket\n");
		return;
	}
	bufferSize = 30000000;
	if(setsockopt(handle, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0)
	{
		printf("Failed to set receive buffer size\n");
	}
	getsockopt(handle, SOL_SOCKET, SO_RCVBUF, &bufferSize, &len);
	printf("Receive buffer size is %d\n", bufferSize);
	memset(&siMe, 0, sizeof(siMe));
    siMe.sin_family = AF_INET;
    siMe.sin_port = htons(port);
    siMe.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(handle, (struct sockaddr*)&siMe, sizeof(siMe)) == -1)
    {
    	printf("Failed to bind socket\n");
    	return;
    }

    // Initialise tracking information
    nextPacketNumber = 0;
    rxFailCount = 0;
    packetWrongSizeCount = 0;
    missingPacketNumberCount = 0;
    latePacketNumberCount = 0;
    goodPacketCount = 0;
    totalPacketCount = 0;
    packetsToNextStats = STATS_INTERVAL;
    gettimeofday(&startTime, 0);
	printf(" Packets  MissNmbr WrngSize   RxFail     MBps      Fps     LateNmbr\n");

    // Receive packets until killed
    while(1)
    {
    	// Wait for a packet
        rxSize = recvfrom(handle, &aPacket, sizeof(aPacket), 0, 0, 0);
        // Check for faults
        if(rxSize == -1)
        {
        	rxFailCount++;
        }
        else if(rxSize != sizeof(aPacket))
        {
        	packetWrongSizeCount++;
        }
        else if(aPacket.packetNumber > nextPacketNumber)
        {
        	time_t curtime;
        	struct tm *loctime;
        	missingPacketNumberCount++;

        	curtime = time(NULL);
        	loctime = localtime(&curtime);
        	printf("%02d:%02d:%02d packet loss: got %ld exp %ld delta %ld count %ld\n",
        			loctime->tm_hour, loctime->tm_min, loctime->tm_sec,
        			aPacket.packetNumber, nextPacketNumber,
        			aPacket.packetNumber - nextPacketNumber, missingPacketNumberCount);
        	packetsToNextStats = 0;
        	nextPacketNumber = aPacket.packetNumber;
        }
        else if(aPacket.packetNumber < nextPacketNumber)
        {
        	latePacketNumberCount++;
        	nextPacketNumber = aPacket.packetNumber;
        }
        else
        {
        	goodPacketCount++;
        }
        nextPacketNumber++;
        totalPacketCount++;
        // packetsToNextStats--;

        // Time to print stats?
        if(packetsToNextStats <= 0)
        {
            packetsToNextStats = STATS_INTERVAL;
        	gettimeofday(&nowTime, 0);
        	period = timeDiff(&startTime, &nowTime);
        	bps = (double)(STATS_INTERVAL*sizeof(aPacket))/period;
        	printf("%8ld %8ld %8ld %8ld %8.2f %8.2f %8ld\n",
        			totalPacketCount, missingPacketNumberCount,
        			packetWrongSizeCount, rxFailCount,
        			bps/BYTES_PER_MEGABYTE, bps/BYTES_PER_FRAME,
        			latePacketNumberCount);
            gettimeofday(&startTime, 0);
        }
    }
}

/* Entry point.
 */
int main(int argc, char **argv)
{
	if(argc > 5 && strcmp(argv[1], "-g") == 0)
	{
		if(argc > 6)
		{
			setCpuAffinity(atoi(argv[6]));
		}
		doGenerator(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	}
	else if(argc > 2 && strcmp(argv[1], "-p") == 0)
	{
		if(argc > 3)
		{
			setCpuAffinity(atoi(argv[3]));
		}
		doProcessor(atoi(argv[2]));
	}
	else
	{
		printf("Throughput testing utility.\n");
		printf("Start a processor then a generator on different machines.\n");
		printf("The generator (-g option) sends blockSize 8K packets as fast as it can with blockPause\n");
		printf("delay between.  The processor (-p option) receives the packets and checks the sequence\n");
		printf("number sent in them.  Both sides print reports every 10000 packets.\n");
		printf("Usage: \'TestThroughput -g <ipAddress> <port> <blockSize> <blockPause:ns> [<cpuNum>]\'\n");
		printf("       \'TestThroughput -p <port> [<cpuNum>]\'\n");
	}
	return EXIT_SUCCESS;
}
