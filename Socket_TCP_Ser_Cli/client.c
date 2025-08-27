// gcc client.c -o client
#include <arpa/inet.h> // inet_addr()
#include <netdb.h> // gethostbyname()
#include <stdio.h> // printf(), perror()
#include <stdlib.h> // exit()
#include <string.h> // strlen()
#include <strings.h> // bzero()
#include <sys/socket.h> // socket(), bind(), listen(), accept()
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr // Define SA as struct sockaddr for simplicity(don gian hon)

void func(int sockfd) 
{
    char buff[MAX]; // buffer to store data
    int n; // variable to store number of characters
    for (;;) { // infinite loop for chat
        bzero(buff, sizeof(buff)); // Clear the buffer
        printf("Enter the string : "); // enter a string
        n = 0; //
        while ((buff[n++] = getchar()) != '\n') // read characters until newline is encountered
            ;
        write(sockfd, buff, sizeof(buff)); // send message to server
        bzero(buff, sizeof(buff)); // Clear the buffer
        read(sockfd, buff, sizeof(buff)); // read message from server
        printf("From Server : %s", buff); // print message from server
        if ((strncmp(buff, "exit", 4)) == 0) { // if message is "exit", exit the chat
            printf("Client Exit...\n"); // announce exit message
            break; // break loop and exit
        }
    }
}

int main()
{
    int sockfd, connfd; // sockfd: file descriptor for socket, 
	                    // connfd: file descriptor for accept socket when client connect successfully
    struct sockaddr_in servaddr, cli; 
	// sockaddr_in: structure for socket server address, consist IP and port number
	// cli: structure for client address
    // create socket and verify
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) { // if socket creation fails, print error notice and exit by value 0
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n"); // if socket creation is successful, print success notice
    bzero(&servaddr, sizeof(servaddr)); // clear the servaddr structure, prepare for connect to server

    // assign IP, Port 
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address of server, loopback address, localhost
    servaddr.sin_port = htons(PORT); // Port number, Convert port number to network byte order

    // Connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // chat func
    func(sockfd);

    // bai bai socket
    close(sockfd);
}
