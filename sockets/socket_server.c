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
#include <string.h>

#define BUFFERSIZE 1
#define PORT 8080

int main() {
    int server_fd, sock, valread;
    struct sockaddr_in address;
    int opt = 1;
    int address_length = sizeof(address);
    char buffer[BUFFERSIZE] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,address_length) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((sock = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&address_length))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Client connected to Server. Server is ready to recive client messages!\n");

    while((valread = read(sock,buffer,BUFFERSIZE)) > 0) {
        send(sock,buffer,BUFFERSIZE,0);
    }
    close(sock);
    return 0;
}
