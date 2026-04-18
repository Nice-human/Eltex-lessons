#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() 
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);
    
    // Создание UDP сокета
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Можно заменить на inet_addr("127.0.0.1")
    
    // Отправка сообщения серверу
    const char *message = "Hello!";
    sendto(sockfd, message, strlen(message), 0, 
           (const struct sockaddr *)&server_addr, addr_len);
    printf("Sent to server: %s\n", message);
    
    // Получение ответа от сервера
    memset(buffer, 0, BUFFER_SIZE);
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, 
                     (struct sockaddr *)&server_addr, &addr_len);
    if (n < 0) 
    {
        perror("Receive failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    printf("Received from server: %s\n", buffer);
    
    close(sockfd);
    return 0;
}