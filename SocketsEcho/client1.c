#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 10200
#define BUFFER_SIZE 1024
#define CLIENT_SHUTDOWN "CLIENT_SHUTDOWN"
#define SERVER_SHUTDOWN "SERVER_SHUTDOWN"

int sockfd;
struct sockaddr_in server_addr;
volatile sig_atomic_t client_running = 1;

// Обработчик сигнала для штатного завершения
void signal_handler(int sig)
{
    (void)sig;
    printf("\n[UDP Client]: shutting down...\n");
    
    // Отправляем сообщение о закрытии серверу
    sendto(sockfd, CLIENT_SHUTDOWN, strlen(CLIENT_SHUTDOWN), 0,
           (const struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("[UDP Client]: sent close message to server\n");
    
    client_running = 0;
    close(sockfd);
    exit(0);
}

int main()
{
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);
    
    // Устанавливаем обработчик сигнала для Ctrl+C
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Создание UDP сокета
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    printf("==========================================\n");
    printf("[UDP Client]: Echo Client Started\n");
    printf("[UDP Client]: Connected to server %s:%d\n", 
           inet_ntoa(server_addr.sin_addr), SERVER_PORT);
    printf("[UDP Client]: Send 'exit' to disconnect\n");
    printf("==========================================\n");
    
    while (client_running)
    {
        printf("\nEnter message >> ");
        fflush(stdout);
        
        memset(buffer, 0, BUFFER_SIZE);
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
            break;
        
        // Удаляем символ новой строки
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (strlen(buffer) == 0)
            continue;
        
        // Проверка на команду выхода
        if (strcmp(buffer, "exit") == 0)
        {
            signal_handler(SIGINT);
        }
        
        // Отправка сообщения серверу
        sendto(sockfd, buffer, strlen(buffer), 0,
               (const struct sockaddr *)&server_addr, sizeof(server_addr));
        printf("\n[UDP Client]: sent to server: \"%s\"\n", buffer);
        
        // Получение ответа
        memset(buffer, 0, BUFFER_SIZE);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                        (struct sockaddr *)&server_addr, &addr_len);

        if (n < 0)
        {
            perror("[UDP Client]: receive failed");
            continue;
        }
        
        // Проверка на сообщение закрытия сервера
        if (strcmp(buffer, SERVER_SHUTDOWN) == 0)
        {
            printf("[UDP Client]: server %s:%d disconnected\n", 
                   inet_ntoa(server_addr.sin_addr), SERVER_PORT);
            client_running = 0;
            break;
        }
        
        printf("[UDP Client]: response from server: \"%s\"\n", buffer);
    }
    
    close(sockfd);
    printf("[UDP Client]: client stopped\n");
    return 0;
}