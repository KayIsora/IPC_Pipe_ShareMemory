// gcc server.c -o server
#include <netinet/in.h> // Network library for IPv4
#include <stdio.h> // Standard I/O library
#include <stdlib.h> // Standard library support exit()
#include <string.h> // String operations
#include <sys/socket.h> // Socket library for system
#include <unistd.h> // library for close(), working with descriptors file
#define PORT 8080 // Define port number for listen()
#define SO_REUSEPORT 15 // Define SO_REUSEPORT

int main(int argc, char const* argv[])
{
    int server_fd; // file descriptor(tay cam) for main socket
	int new_socket; // file descriptor for accept socket when client connect successfully
    ssize_t valread; // number of bytes read from client socket
    struct sockaddr_in address; // structure for socket server address, consist of IP address and port number
    int opt = 1; // option for setsockopt(), allow reuse address and port
    socklen_t addrlen = sizeof(address); // size of address structure
    char buffer[1024] = { 0 }; // buffer to store data received from client
    char* hello = "Hello from server"; // message to send to client

    // create new socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // AF_INET: IPv4, SOCK_STREAM: TCP
        perror("socket failed"); // print error message if socket creation fails
        exit(EXIT_FAILURE); // exit program with failure status
    }

	// allow reuse address and port if server is restarted many times 	
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address
    address.sin_port = htons(PORT); // Port Listen, Convert port number to network byte order

    // For binding socket to the port 8080
	// server_fd: file descriptor of the socket
	// (struct sockaddr*)&address: pointer to the address structure to store client address
	// sizeof(address): size of the address structure
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address)) < 0) {
        perror("bind failed");// print error message if binding fails
        exit(EXIT_FAILURE);
    }
    // Start listening for incoming connections
	// 3: maximum number of pending connections in the queue
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // Accept an incoming connection, return a new socket file descriptor for the accepted connection,
	// start communicating with the client
	// server_fd: file descriptor of the listening socket
	// &addrlen: pointer to the size of the address structure
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address,
                            &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Read data sent by the client
	// why 1024 -1 ? To leave space for null terminator, total 1024, read max 1023 
	// Prevents buffer overflow when the string needs to be null-terminated
    valread = read(new_socket, buffer, 1024 - 1);
    printf("%s\n", buffer);
	// send message to the client
	// 0: flags (none)
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    // Close connected socket
    close(new_socket);
    // Close the listening socket
    close(server_fd);
    return 0;
}
