// udp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXLINE 1024

// Hàm chat với server (giống TCP)
void func(int sockfd, struct sockaddr_in *servaddr, socklen_t len)
{
    char buff[MAXLINE];
    int n;
    
    for (;;) {
        // Xóa buffer
        memset(buff, 0, sizeof(buff));
        printf("Enter the string: ");
        n = 0;
        
        // Client nhập tin nhắn
        while ((buff[n++] = getchar()) != '\n')
            ;
        
        // Gửi tin nhắn đến server
        sendto(sockfd, buff, strlen(buff), 0,
               (const struct sockaddr *) servaddr, len);
        if ((strncmp("exit", buff, 4)) == 0) {
        printf("Client Exit...\n");
        close(sockfd);
        exit(0);
    	}
        // Xóa buffer để nhận phản hồi
        memset(buff, 0, sizeof(buff));
        
        // Nhận phản hồi từ server
        n = recvfrom(sockfd, buff, MAXLINE, 0,
                    (struct sockaddr *) servaddr, &len);
        buff[n] = '\0';
        
        printf("From Server: %s", buff);
        
        // Nếu server gửi "exit" thì client thoát
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Server asked to exit. Client Exit...\n");
            break;
        }
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len;
    
    // Tạo socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created..\n");
    
    // Xóa cấu trúc địa chỉ
    memset(&servaddr, 0, sizeof(servaddr));
    
    // Thiết lập thông tin server
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    printf("Connected to the server..\n");
    len = sizeof(servaddr);
    
    // Gửi tin nhắn đầu tiên
    char initial_buff[MAXLINE];
    printf("Enter the string: ");
    int n = 0;
    while ((initial_buff[n++] = getchar()) != '\n')
        ;
    
    // Gửi tin nhắn đầu tiên đến server
    sendto(sockfd, initial_buff, strlen(initial_buff), 0,
           (const struct sockaddr *) &servaddr, len);
    if ((strncmp(initial_buff, "exit", 4)) == 0)
	{
		printf("Client Exit...\n");
		close(sockfd);
		exit(0);
	}
    // Nhận phản hồi đầu tiên
    memset(initial_buff, 0, MAXLINE);
    n = recvfrom(sockfd, initial_buff, MAXLINE, 0,
                (struct sockaddr *) &servaddr, &len);
    initial_buff[n] = '\0';
    
    printf("From Server: %s", initial_buff);
    
    // Nếu server gửi "exit" ngay lần đầu
    if ((strncmp("exit", initial_buff, 4)) == 0) {
        printf("Client Exit...\n");
        close(sockfd);
        return 0;
    }
    
    // Bắt đầu chat liên tục
    func(sockfd, &servaddr, len);
    
    // Đóng socket
    close(sockfd);
    return 0;
}
