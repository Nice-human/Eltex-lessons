#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define UDP_PORT 10200
#define BUFFER_SIZE 1024

int main() 
{
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);
    
    // Создание UDP сокета
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) 
    {
        perror("Error socket");
        exit(EXIT_FAILURE);
    }
    
    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    printf("[UDP клиент] Отправка запроса серверу\n");
    
    // Отправка запроса TIME
    char *request = "TIME";
    sendto(client_fd, request, strlen(request), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("[UDP клиент] Отправлен запрос: %s\n", request);
    
    // Чтение ответа
    ssize_t bytes_read = recvfrom(client_fd, buffer, sizeof(buffer) - 1, 0,
                                   (struct sockaddr*)&server_addr, &addr_len);
    if (bytes_read > 0) 
    {
        buffer[bytes_read] = '\0';
        printf("[UDP клиент] Получен ответ: %s\n", buffer);
    } 
    else 
    {
        printf("[UDP клиент] Ошибка чтения ответа\n");
    }
    
    // Закрытие сокета
    close(client_fd);
    printf("[UDP клиент] Завершение работы\n");
    
    return 0;
}