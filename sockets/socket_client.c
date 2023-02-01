#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#define BUFFERSIZE 1
#define PORT 8080

# define KIB 1000.0

int main() {
    int sock = 0, valread;
    struct sockaddr_in address;
    int address_length = sizeof(address);
    char buffer[BUFFERSIZE] = {0};

    if ((sock = socket(AF_INET,SOCK_STREAM,0)) < 0) {
        perror("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    memset(&address,'0',address_length);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(inet_pton(AF_INET,"127.0.0.1",&address.sin_addr) <= 0) {
        perror("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
    if (connect(sock,(struct sockaddr *)&address,address_length) < 0) {
        perror("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }

    //client with server via socket connected.
    //start latency measurement..

    buffer[0] = 'M';

    struct timeval start,end;
    gettimeofday(&start,NULL);

    for(int i = 0; i < 1000; i++) {
        send(sock,buffer,BUFFERSIZE,0);
        //printf("%s%d => ",buffer,i);
        memset(buffer,0,BUFFERSIZE);

        read(sock,buffer,BUFFERSIZE);
        //printf("%s\n",buffer);
    }
    gettimeofday(&end,NULL);
    memset(buffer,0,BUFFERSIZE);

    //get mean latancy
    double latency = (end.tv_sec - start.tv_sec) * 1000.0;
    latency += (end.tv_usec - start.tv_usec) / 1000.0;
    latency = latency/1000;
    printf("Latency: %f ms\n", latency);

    //start bandwidth measurement..
    int buffer_sizes[15] = {1024, 2048, 4096, 8192, 16384, 32758, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216};

    for(int i = 0; i < 15; i++) {
        struct timeval start,end;
        gettimeofday(&start,NULL);

        for(int j = 0; j < buffer_sizes[i]; j++) {
            send(sock,buffer,buffer_sizes[i],0);
            //printf("%s%d => ",buffer,i);
            memset(buffer,0,BUFFERSIZE);

            read(sock,buffer,buffer_sizes[i]);
            //printf("%s\n",buffer);
        }

        gettimeofday(&end,NULL);

        //calc average bandwidth..
        double bandwidth = buffer_sizes[i] * KIB;
        double time = (end.tv_sec - start.tv_sec) * KIB;
        time += (end.tv_usec - start.tv_usec) / KIB;
        bandwidth = bandwidth/time;

        //print out current average bandwidth!
        printf("Bandwidth: %f kbit/s \n",bandwidth);
    }
    close(sock);
    return 0;
}
