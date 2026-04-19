#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define UDP_PORT 7777
#define BROADCAST_ADDR "255.255.255.255"
#define BUFFER_SIZE 1024

int main() 
{
    int client_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in local_addr, sender_addr;   
    socklen_t sender_addr_len = sizeof(sender_addr);
    
    // Создание UDP сокета
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) 
    {
        perror("Error socket");
        exit(EXIT_FAILURE);
    }
    
    // Настройка адреса сервера
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(UDP_PORT);
    local_addr.sin_addr.s_addr = inet_addr(BROADCAST_ADDR);

    // Привязка сокета к адресу
    if (bind(client_fd, (const struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) 
    {
        perror("Error Bind");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    int received = 0;
    while (received < 10)
    {
        // Чтение ответа
        ssize_t bytes_read = recvfrom(client_fd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr*)&sender_addr, &sender_addr_len);
        if (bytes_read > 0) 
        {
            buffer[bytes_read] = '\0';
            printf("[UDP Client] Получен ответ %d: %s\n", received + 1, buffer);
            received++;
        } 
        else 
        {
            continue;
        }
    }
       
    // Закрытие сокета
    close(client_fd);
    printf("[UDP Client] Завершение работы\n");
    
    return 0;
}