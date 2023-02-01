#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define BUFFERSIZE 1024
#define PORT 8080

int main() {
	int sock = 0, valread;
	struct sockaddr_in address;
	int address_length = sizeof(address);
	char buffer[BUFFERSIZE] = {0};
	if ((sock = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("\n Socket creation error \n");
		return -1;
	}

	memset(&address,'0',address_length);

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);

	if(inet_pton(AF_INET,"127.0.0.1",&address.sin_addr) <= 0) {
		perror("\nInvalid address/ Address not supported \n");
		return -1;
	}
	if (connect(sock,(struct sockaddr *)&address,address_length) < 0) {
		perror("\nConnection Failed \n");
		return -1;
	}
    while(1) {
        printf("Enter Message: ");
        memset(buffer,0,BUFFERSIZE);
        fgets(buffer,BUFFERSIZE, stdin);

        send(sock,buffer,BUFFERSIZE,0);
        //printf("Message sent\n");
    }
    return 0;
}
