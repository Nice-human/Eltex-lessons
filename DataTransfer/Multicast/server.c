#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 7777
#define MULTICAST_ADDR "224.0.0.1"

int main() 
{
    int server_fd;
    struct sockaddr_in multicast_addr;
    
    // Создание UDP сокета
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("Error Socket");
        exit(EXIT_FAILURE);
    }
    
    // Настройка адреса для multicast рассылки
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
    
    // Отправка 10 multicast сообщений
    for (int i = 0; i < 10; i++)
    {
        const char *message = "Hi";
        ssize_t sent = sendto(server_fd, message, strlen(message), 0, 
                            (struct sockaddr *)&multicast_addr, sizeof(multicast_addr));
        
        if (sent > 0) 
        {
            printf("[UDP Server] отправил multicast %d: %s\n", i + 1, message);
        } 
        else 
        {
            perror("[UDP Server] ошибка multicast");
        }
        
        sleep(1); // Небольшая задержка между сообщениями
    }
      
    close(server_fd);
    printf("[UDP Server] Завершение работы\n");
    return 0;
}