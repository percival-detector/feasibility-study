#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 6666
#define PACKET 8000

int main(int argc, char* argv[])
{
    int listener = 0, connection = 0, rd = 0;
    unsigned int counter = 0, bytes = 0;
    double mbytes = 0.0;
    char packet[PACKET];
    time_t start_time = 0, stop_time = 0;
    double seconds, bitrate;
    struct sockaddr_in receiver_addr;

    memset(&receiver_addr, '0', sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    receiver_addr.sin_port = htons(PORT);

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error creating listener socket.\n");
        return(-1);
    }

    if (bind(listener, (struct sockaddr*)&receiver_addr, sizeof(receiver_addr)) == -1)
    {
        close(listener);
        printf("Error binding listener socket to port.\n");
        return(-1);
    }

    if (listen(listener, 1) == -1)
    {
        close(listener);
        printf("Error starting listener.\n");
        return(-1);
    }

    printf("Listening...\n");
    connection = accept(listener, (struct sockaddr*)NULL, NULL);
    printf("\tconnection accepted.\n");

    time(&start_time);
    while (((rd = recv(connection, packet, PACKET, 0)) > 0) && (packet[0] != '0')) counter++;
    time(&stop_time);

    close(connection);
    close(listener);
    printf("Connection closed.\n");

    seconds = stop_time - start_time;

    if (counter == 0) bitrate = 0.0;
    else if (seconds == 0.0) bitrate = counter;
    else bitrate = (double)counter / seconds;
    bitrate = bitrate * PACKET * 8;
    bitrate = bitrate / 1000000.0;
    printf("Received %u packets in %.f seconds at a rate of %.2f Mbit/s.\n",
           counter, seconds, bitrate);

    bytes = counter * PACKET;
    mbytes = (double)bytes / 1000000.0;
    printf("Received %u Bytes or %.f MB of data.\n", bytes, mbytes);

    return(0);
}