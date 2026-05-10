#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 10200
#define BUFFER_SIZE 1024

int main() 
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // Создание UDP сокета
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Принимать соединения на всех интерфейсах
    server_addr.sin_port = htons(SERVER_PORT);
    
    // Привязка сокета к адресу
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    printf("UDP Server listening on port %d\n", SERVER_PORT);
    
    // Получение сообщения от клиента
    memset(buffer, 0, BUFFER_SIZE);
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, 
                     (struct sockaddr *)&client_addr, &addr_len);
    if (n < 0) 
    {
        perror("Receive failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    printf("Received from client: %s\n", buffer);
    
    // Отправка ответа клиенту
    const char *response = "Hi!";
    sendto(sockfd, response, strlen(response), 0, 
           (struct sockaddr *)&client_addr, addr_len);
    printf("Sent response: %s\n", response);
    
    close(sockfd);
    return 0;
}