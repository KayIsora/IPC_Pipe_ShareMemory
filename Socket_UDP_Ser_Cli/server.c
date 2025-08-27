// udp_server.c
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

// Hàm chat giữa server và client (giống TCP)
void func(int sockfd, struct sockaddr_in *cliaddr, socklen_t len)
{
    char buff[MAXLINE];
    int n;
    
    // Vòng lặp vô hạn để chat
    for (;;) {
        // Xóa buffer
        memset(buff, 0, MAXLINE);
        
        // Nhận tin nhắn từ client
        n = recvfrom(sockfd, buff, MAXLINE, 0, 
                    (struct sockaddr *) cliaddr, &len);
        buff[n] = '\0';
        
        // In tin nhắn từ client
        printf("From client: %s\t To client: ", buff);
        
        // Kiểm tra nếu client gửi "exit"
        if (strncmp("exit", buff, 4) == 0) {
            printf("exit\n");
            printf("Server Exit...\n");
            break;
        }
        
        // Xóa buffer để nhập phản hồi
        memset(buff, 0, MAXLINE);
        n = 0;
        
        // Server nhập tin nhắn phản hồi
        while ((buff[n++] = getchar()) != '\n')
            ;
        
        // Gửi phản hồi cho client
        sendto(sockfd, buff, strlen(buff), 0,
               (const struct sockaddr *) cliaddr, len);
        
        // Nếu server gõ "exit" thì thoát
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    
    // Tạo socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created..\n");
    
    // Xóa cấu trúc địa chỉ
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    // Thiết lập thông tin server
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    
    // Bind socket với địa chỉ server
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully binded..\n");
    
    printf("Server listening..\n");
    len = sizeof(cliaddr);
    
    // Chờ tin nhắn đầu tiên từ client để biết địa chỉ client
    char initial_buff[MAXLINE];
    int n = recvfrom(sockfd, initial_buff, MAXLINE, 0,
                    (struct sockaddr *) &cliaddr, &len);
    initial_buff[n] = '\0';
    
    printf("Client connected..\n");
    printf("From client: %s\t To client: ", initial_buff);
    
    // Kiểm tra nếu tin nhắn đầu tiên là "exit"
    if (strncmp("exit", initial_buff, 4) == 0) {
        printf("exit\n");
        printf("Server Exit...\n");
        close(sockfd);
        return 0;
    }
    
    // Server nhập phản hồi đầu tiên
    memset(initial_buff, 0, MAXLINE);
    n = 0;
    while ((initial_buff[n++] = getchar()) != '\n')
        ;
    
    // Gửi phản hồi đầu tiên
    sendto(sockfd, initial_buff, strlen(initial_buff), 0,
           (const struct sockaddr *) &cliaddr, len);
    
    // Nếu server gõ "exit" ngay lần đầu
    if (strncmp("exit", initial_buff, 4) == 0) {
        printf("Server Exit...\n");
        close(sockfd);
        return 0;
    }
    
    // Bắt đầu chat liên tục
    func(sockfd, &cliaddr, len);
    
    // Đóng socket
    close(sockfd);
    return 0;
}
