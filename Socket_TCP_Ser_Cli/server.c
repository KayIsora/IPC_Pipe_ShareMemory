// gcc server.c -o server
#include <stdio.h>  // Standard I/O library
#include <netdb.h>  // Network database operations like gethostbyname(), 
					//find and solve information about domain names and IP addresses in the network.
#include <netinet/in.h> // Network library for IPv4
#include <stdlib.h> // Standard library support exit()
#include <string.h> // String operations
#include <sys/socket.h> // Socket library for system
#include <sys/types.h> // provide basic system data types for abstraction(truu tuong hoa) of new data types 
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr // Define SA as struct sockaddr for simplicity(don gian hon)

// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX); // Clear the buffer
        read(connfd, buff, sizeof(buff)); // Read message from client and copy it to buffer
        printf("From client: %s\t To client : ", buff); // Print message from client
        bzero(buff, MAX); // Clear the buffer
        n = 0; // Initialize(khoi tao) n to 0
        // Copy characters from standard input to buffer until newline is encountered
        while ((buff[n++] = getchar()) != '\n')
            ;

        // Send message to client
        write(connfd, buff, sizeof(buff));

        // Exit the chat if the message is "exit"
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

// Driver for main function
int main()
{
    int sockfd, connfd, len; // sockfd: file descriptor for socket, connfd: file descriptor for accept socket when client connect successfully, len: length of structure sockaddr_in
							// len: length of structure sockaddr_in
    struct sockaddr_in servaddr, cli; 
	// servaddr: structure for socket server address, consist of IP address and port number, 
	// cli: structure for client address
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // create new socket and verify
    if (sockfd == -1) { // if socket creation fails
        printf("socket creation failed...\n"); // print error notification
        exit(0); // exit program with failure status
    }
    else
        printf("Socket successfully created..\n");// very good he he he
    bzero(&servaddr, sizeof(servaddr)); // Clear the servaddr structure

    // Assign(gan) IP, PORT
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any IP address
    servaddr.sin_port = htons(PORT); // Port Listen, Convert port number to network byte order

    // Binding(lien ket) newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		//sockfd: file descriptor of the socket
		//(struct sockaddr*)&servaddr: pointer to the address structure to store client address
		//sizeof(servaddr): size of the address structure
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {// 5: maximum number of pending connections in the queue
        printf("Listen failed...\n");
        exit(0);
    }
    else // if listening is successful
        printf("Server listening..\n");
    len = sizeof(cli);
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
	// connfd: file descriptor for accept socket when client connect successfully
	// sockfd: file descriptor of the listening socket
	// &len: pointer to the size of the address structure
    if (connfd < 0) {// failed
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
    // Function for chatting between client and server
    func(connfd);
    // close the socket
    close(sockfd);
}
