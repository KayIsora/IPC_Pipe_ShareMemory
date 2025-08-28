// UDP client program
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
    socklen_t len = sizeof(servaddr);
    int n;

    // Tạo socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Connected to server. Type 'exit' to quit.\n");

    while (1) {
        n = 0;
        memset(buffer, 0, MAXLINE);
        printf("Enter the string to server: ");
        // Đọc cả dòng, không lưu ký tự newline
        while ((buffer[n] = getchar()) != '\n' && n < MAXLINE - 1)
            n++;
        buffer[n] = '\0'; // Kết thúc chuỗi

        // Gửi tin nhắn đến server
        if (sendto(sockfd, buffer, strlen(buffer), 0,
            (const struct sockaddr *)&servaddr, len) < 0) {
            perror("sendto failed");
            close(sockfd);
            return 1;
        }

        // Thoát nếu nhập "exit"
        if (strncmp("exit", buffer, 4) == 0) {
            printf("UDP Client Exit...\n");
            break;
        }

        memset(buffer, 0, MAXLINE);
        // Đợi phản hồi từ server
        if (recvfrom(sockfd, buffer, MAXLINE, 0,
                (struct sockaddr *)&servaddr, &len) < 0) {
            perror("recvfrom failed");
            close(sockfd);
            return 1;
        }
        printf("Message from server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
