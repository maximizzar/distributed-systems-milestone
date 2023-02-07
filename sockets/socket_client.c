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

#define BUFFERSIZE 1024
#define PORT 8080

//in bytes
#define KIB 1024
#define MIB 1048576

int main(int argc,char* argv[]) {
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

    if(inet_pton(AF_INET,argv[1],&address.sin_addr) <= 0) {
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
        read(sock,buffer,BUFFERSIZE);
    }
    gettimeofday(&end,NULL);
    memset(buffer,0,BUFFERSIZE);

    //get mean latancy
    double latency = (end.tv_sec - start.tv_sec) * 1000.0;
    latency += (end.tv_usec - start.tv_usec) / 1000.0;
    latency = latency / 1000;
    printf("Latency: %f ms\n\n", latency);

    //start bandwidth measurement..
    for(int i = KIB; i <= 16 * MIB; i = i * 2) {
        printf("File-size: %d KiB ",i / KIB);

        struct timeval start,end;
        gettimeofday(&start,NULL);

        for(int j = 1; j <= i; j = j + KIB) {
            send(sock,buffer,BUFFERSIZE,0);
            read(sock,buffer,BUFFERSIZE);
        }
        gettimeofday(&end,NULL);

        //calc average bandwidth..
        double time = (end.tv_sec - start.tv_sec) * 1000.0;
        time += (end.tv_usec - start.tv_usec) / 1000.0;

        double bandwidth = i; //kilobyte
        bandwidth = bandwidth / time;

        //print out current average bandwidth!
        printf("--> bandwidth: %f KB/s\n",bandwidth);
    }
    close(sock);
    return 0;
}