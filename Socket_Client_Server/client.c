// gcc client.c -o client
// check
#include <arpa/inet.h> // Library for inet_pton(), convert IPv4 and IPv6 addresses from text to binary
#include <stdio.h> // Standard I/O library
#include <string.h> // String operations
#include <sys/socket.h> // Socket library for system
#include <unistd.h> // library for close(), working with descriptors file
#define PORT 8080 	// Define port number for connect()

// Client function
// argc: number of command line arguments (doi so truyen vao)
// argv: array of command line arguments
int main(int argc, char const* argv[])
{
	// variable declaration, 
	// status: return value of connect() function
	// valread: number of bytes read from server socket
	// client_fd: file descriptor for client socket
    int status, valread, client_fd; 
	// serv_addr: structure for socket server address, consist of IP address and port number
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // AF_INET: IPv4, SOCK_STREAM: TCP
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_port = htons(PORT); // Port number, Convert port number to network byte order

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		// IP server 127.0.0.1: loopback address, localhost
		// &serv_addr.sin_addr: pointer to the structure to store the binary address
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr,
                         sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // send message to server
    send(client_fd, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    valread = read(client_fd, buffer, 1024 - 1); // read message from server
    printf("%s\n", buffer);

    // close the socket
    close(client_fd);
    return 0;
}
