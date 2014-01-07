#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//#define HOST "172.23.138.78"
#define HOST "192.168.138.78"
#define PORT 6666
#define PACKET 8000

int main(int argc, char* argv[])
{
    int connection = 0;
    unsigned int counter = 0, packet_limit;
    unsigned int bytes = 0;
    double mbytes = 0.0;
    char packet[PACKET];
    char end[1] = { '0' };
    time_t start_time = 0, stop_time = 0;
    double seconds, bitrate;
    struct sockaddr_in receiver_addr;

    if (argc == 2) packet_limit = atoi(argv[1]);
    else packet_limit = 10000;

    memset(packet, 'X', sizeof(packet)-1);
    packet[PACKET] = '\0';

    memset(&receiver_addr, '0', sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, HOST, &receiver_addr.sin_addr) == -1)
    {
        printf("Error setting receiver address.\n");
        return(-1);
    }

    if ((connection = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error creating sender socket.\n");
        return(-1);
    }
    if (connect(connection, (struct sockaddr*)&receiver_addr, sizeof(receiver_addr)) == -1)
    {
        printf("Error connecting to receiver.\n");
        return(-1);
    }

    printf("Sending now...\n");
    time(&start_time);
    while (counter < packet_limit)
    {
        send(connection, packet, PACKET, 0);
        counter++;
    }
    time(&stop_time);

    send(connection, end, 1, 0);
    printf("\tdone sending.\n");

    close(connection);

    seconds = stop_time - start_time;

    if (counter == 0) bitrate = 0.0;
    else if (seconds == 0.0) bitrate = counter;
    else bitrate = (double)counter / seconds;
    bitrate = bitrate * PACKET * 8;
    bitrate = bitrate / 1000000.0;
    printf("Sent %u packets in %.f seconds at a rate of %.2f Mbit/s.\n",
           counter, seconds, bitrate);

    bytes = counter * PACKET;
    mbytes = (double)bytes / 1000000.0;
    printf("Sent %u Bytes or %.f MB of data.\n", bytes, mbytes);

    return(0);
}
