#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>     

#define UDP_PORT 7777
#define MULTICAST_ADDR "224.0.0.1"
#define BUFFER_SIZE 1024

// Явно определяем структуру ip_mreqn
struct my_ip_mreqn 
{
    struct in_addr imr_multiaddr;   /* IP multicast address of group */
    struct in_addr imr_address;     /* local IP address of interface */
    int imr_ifindex;                /* Interface index */
};


int main() 
{
    int client_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in local_addr, sender_addr;   
    socklen_t sender_addr_len = sizeof(sender_addr);
    struct my_ip_mreqn multicast_request;
    
    // Создание UDP сокета
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) 
    {
        perror("Error socket");
        exit(EXIT_FAILURE);
    }
    
    // Настройка адреса для привязки (слушаем все интерфейсы)
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(UDP_PORT);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета к адресу
    if (bind(client_fd, (const struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) 
    {
        perror("Error Bind");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    // Подготовка параметров к подписки multicast
    memset(&multicast_request, 0, sizeof(multicast_request));
    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
    multicast_request.imr_address.s_addr = INADDR_ANY;
    multicast_request.imr_ifindex = 0;
    
    // Подписываемся на multicast группу
    if (setsockopt(client_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
                   &multicast_request, sizeof(multicast_request)) < 0) 
    {
        perror("Error setsockopt IP_ADD_MEMBERSHIP");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    int received = 0;
    while (received < 10)
    {
        // Чтение multicast сообщений
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
            perror("Error recvfrom");
            break;
        }
    }

    // Отписываемся от multicast группы
    if (setsockopt(client_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, 
                   &multicast_request, sizeof(multicast_request)) < 0) 
    {
        perror("Error setsockopt IP_DROP_MEMBERSHIP");
    }
          
    // Закрытие сокета
    close(client_fd);
    printf("[UDP Client] Завершение работы\n");
    
    return 0;
}