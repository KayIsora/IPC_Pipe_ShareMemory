// TCP Client program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORT 5000
#define MAXLINE 1024

int main()
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr;

    // Kiểm tra tạo socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket creation failed");
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Cấu hình địa chỉ server
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Kết nối đến server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("\n Error : Connect Failed \n");
        close(sockfd);
        return 1;
    }

    printf("Connected to server. Type 'exit' to quit.\n");

    while (1) {
        int n;
        memset(buffer, 0, MAXLINE);
        printf("Enter the string to server (type 'exit' to quit): ");
        n = 0;
        // Đọc cả dòng từ người dùng, không thừa ký tự newline trong buffer
        while ((buffer[n] = getchar()) != '\n' && n < MAXLINE - 1)
            n++;
        buffer[n] = '\0'; // Kết thúc chuỗi đúng cách

        // Gửi tin nhắn đến server
        write(sockfd, buffer, strlen(buffer) + 1); // Gửi cả ký tự kết thúc chuỗi

        // Nếu người dùng nhập "exit", thoát
        if (strncmp("exit", buffer, 4) == 0) {
            printf("TCP Client Exit...\n");
            break;
        }

        memset(buffer, 0, MAXLINE);
        // Đọc phản hồi từ server
        int bytes_received = read(sockfd, buffer, MAXLINE);
        if (bytes_received <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        printf("Message from server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
