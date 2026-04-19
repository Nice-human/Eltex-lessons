#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 7777
#define BROADCAST_ADDR "255.255.255.255"

int main() 
{
    int server_fd;
    struct sockaddr_in broadcast_addr;
    
    // Создание UDP сокета
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("Error Socket");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) < 0) 
    {
        perror("Error setsockopt");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_ADDR);
    
    
    // Отправка 10 широковещательных сообщений
    for (int i = 0; i < 10; i++)
    {
        const char *message = "Hi";
        ssize_t sent = sendto(server_fd, message, strlen(message), 0, 
                            (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));
        
        if (sent > 0) 
        {
            printf("[UDP Server] отправил broadcast %d: %s\n", i + 1, message);
            
        } 
        else 
        {
            perror("[UDP Server] ошибка broadcast");
        }
        
        sleep(1); // Небольшая задержка между сообщениями
    }
      
    close(server_fd);
    printf("[UDP Server] Завершение работы\n");
    return 0;
      
    close(server_fd);
    return 0;
}