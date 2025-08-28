#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>
#include <fcntl.h>

#define PORT 5000
#define MAXLINE 1024

int check_TCP = 1; // 全局TCP服务开关
int check_UDP = 1; // 全局UDP服务开关

void sig_chld(int signo) {
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

int main() {
    int listenfd, udpfd, connfd, maxfdp1;
    int nready;
    char buffer[MAXLINE];
    pid_t childpid;
    fd_set rset;
    socklen_t len;
    struct sockaddr_in cliaddr, servaddr;
    char *message_reply_TCP = "Apply message from TCP!";
	char *message_reply_UDP = "Apply message from UDP!";

    // SIGCHLD handler (回收子进程，防止僵尸进程)
    signal(SIGCHLD, sig_chld);

    // 创建TCP listening socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket TCP");
        exit(1);
    }
    printf("TCP socket created.\n");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // 允许地址复用，方便调试
    int reuse = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
        exit(1);
    }

    // 绑定TCP socket
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind TCP");
        exit(1);
    }
    printf("TCP socket binded.\n");
    // 开始监听
    if (listen(listenfd, 10) < 0) {
        perror("listen");
        exit(1);
    }
    printf("TCP server listening...\n");

    // 创建UDP socket
    if ((udpfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket UDP");
        exit(1);
    }
    printf("UDP socket created.\n");

    // 绑定UDP socket
    if (bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind UDP");
        exit(1);
    }
    printf("UDP socket binded.\n");

    FD_ZERO(&rset);
    maxfdp1 = (listenfd > udpfd ? listenfd : udpfd) + 1;

    while (check_TCP || check_UDP) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);

        // 等待事件发生
        nready = select(maxfdp1, &rset, NULL, NULL, NULL);
        if (nready < 0) {
            perror("select");
            close(listenfd);
            close(udpfd);
            exit(1);
        }

        // TCP有连接事件
				if (FD_ISSET(listenfd, &rset) && check_TCP) {
			len = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
			if (connfd < 0) {
				perror("accept");
				continue;
			}
			printf("TCP connection accepted from %s:%d\n",
				inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

			// 使用fork处理每个TCP client
			if ((childpid = fork()) == 0) {   // 子进程
				close(listenfd); // 子进程不需要listenfd

				while (1) {
					bzero(buffer, MAXLINE);
					ssize_t n = read(connfd, buffer, MAXLINE);
					if (n <= 0) {
						// Client disconnected or error
						perror("TCP read failed or client disconnected");
						break;
					}
					printf("Message from TCP client: %s\n", buffer);

					// 自动回复固定消息（不需人工输入）
					strcpy(buffer, message_reply_TCP);
					write(connfd, buffer, strlen(buffer) + 1);
					printf("Sent reply to TCP client: %s\n", buffer);

					// 如果收到"exit"，关闭本连接
					if (strncmp("exit", buffer, 4) == 0) {
						printf("TCP client requested exit, closing this connection.\n");
						break;
					}
				}

				close(connfd);
				exit(0);
			} else if (childpid < 0) {
				perror("fork");
				close(connfd);
				continue;
			}
			// 父进程继续loop，不需要connfd
			close(connfd);
		}


        // UDP有数据到达
        if (FD_ISSET(udpfd, &rset) && check_UDP) {
            len = sizeof(cliaddr);
            bzero(buffer, MAXLINE);
            ssize_t n = recvfrom(udpfd, buffer, MAXLINE, 0,
                                 (struct sockaddr *)&cliaddr, &len);
            if (n < 0) {
                perror("UDP recvfrom");
                continue;
            }
            printf("Message from UDP client: %s\n", buffer);

            // 自动回复固定消息（不需人工输入）
            strcpy(buffer, message_reply_UDP);
            n = sendto(udpfd, buffer, strlen(buffer) + 1, 0,
                       (struct sockaddr *)&cliaddr, len);
            if (n < 0) {
                perror("UDP sendto");
                continue;
            }
            printf("Sent reply to UDP client: %s\n", buffer);

            // 如果收到"exit"，设置全局标志，停止UDP服务，不影响TCP
            if (strncmp("exit", buffer, 4) == 0) {
                printf("UDP client requested exit, closing UDP service.\n");
                check_UDP = 0;
                close(udpfd);
            }
        }
    }

    // 服务结束，清理资源
    printf("Server shutdown.\n");
    if (listenfd >= 0) close(listenfd);
    if (udpfd >= 0) close(udpfd);
    return 0;
}
