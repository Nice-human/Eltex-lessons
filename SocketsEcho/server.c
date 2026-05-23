#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SERVER_PORT 10200
#define BUFFER_SIZE 1024
#define MAX_HISTORY 100
#define CLIENT_SHUTDOWN "CLIENT_SHUTDOWN"
#define SERVER_SHUTDOWN "SERVER_SHUTDOWN"

typedef struct 
{
    uint16_t port;
    uint32_t ip_addr;
    int number; 
} Client;

typedef struct 
{
    Client client[MAX_HISTORY];  
    int count;                         
} history;

int sockfd = -1;
history cl_history = {0};
volatile sig_atomic_t server_running = 1;

// Обработчик сигнала для завершения сервера
void signal_handler(int sig)
{
    printf("\n[UDP Server]: Shutting down...\n");

    // Отправляем сообщение о завершении ВСЕМ клиентам
    for (int i = 0; i < cl_history.count; i++)
    {
        struct sockaddr_in client;
        memset(&client, 0, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_addr.s_addr = cl_history.client[i].ip_addr;
        client.sin_port = htons(cl_history.client[i].port);
        
        sendto(sockfd, SERVER_SHUTDOWN, strlen(SERVER_SHUTDOWN), 0,
               (const struct sockaddr *)&client, sizeof(client));
        printf("[UDP Server]: sent shutdown notification to %s:%d\n", 
               inet_ntoa(*(struct in_addr*)&cl_history.client[i].ip_addr), 
               cl_history.client[i].port);
    }
    
    server_running = 0;
    if (sockfd != -1)
    {
        close(sockfd);
    }
    exit(0);
}

// Поиск клиента по IP и порту
int find_client(uint32_t ip_addr, uint16_t port, history *cl_history)
{
    for (int i = 0; i < cl_history->count; i++)
    {
        if (cl_history->client[i].ip_addr == ip_addr && 
            cl_history->client[i].port == port)
            return i;
    }
    return -1;
}

// Добавление нового клиента
int add_client(uint32_t ip_addr, uint16_t port, history *cl_history)
{
    if (cl_history->count >= MAX_HISTORY) 
        return -1;
    
    cl_history->client[cl_history->count].ip_addr = ip_addr;
    cl_history->client[cl_history->count].port = port;
    cl_history->client[cl_history->count].number = 1;
    cl_history->count++;
    
    return cl_history->count - 1;
}

// Удаление клиента
void remove_client(uint32_t ip_addr, uint16_t port, history *cl_history)
{
    int client_index = find_client(ip_addr, port, cl_history);
    if (client_index == -1) return;
    
    printf("[UDP Server]: removing client %s:%d\n", 
           inet_ntoa(*(struct in_addr*)&ip_addr), port);
    
    for (int i = client_index; i < cl_history->count - 1; i++)
    {
        cl_history->client[i] = cl_history->client[i + 1];
    }
    cl_history->count--;
}


int main() 
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // Установка обработчика сигнала для Ctrl+C
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
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    
    // Привязка сокета к адресу
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Error Bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    printf("==========================================\n");
    printf("[UDP Server]: Echo Server Started\n");
    printf("[UDP Server]: Listening on port %d\n", SERVER_PORT);
    printf("==========================================\n");
    
    while (server_running)
    {
        memset(buffer, 0, BUFFER_SIZE);
        
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, 
                        (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) 
        {
            perror("Error recvfrom");
            continue;
        }
        
        uint32_t client_ip = client_addr.sin_addr.s_addr;
        uint16_t client_port = ntohs(client_addr.sin_port);
        
        printf("\n[UDP Server]: received from client %s:%d: \"%s\"\n", 
               inet_ntoa(client_addr.sin_addr), client_port, buffer);
        
        // Проверка на сообщение закрытия
        if (strcmp(buffer, CLIENT_SHUTDOWN) == 0)
        {
            remove_client(client_ip, client_port, &cl_history);
            printf("[UDP Server]: client %s:%d disconnected\n", 
                   inet_ntoa(client_addr.sin_addr), client_port);
            continue;
        }
        
        // Поиск или добавление клиента
        int client_index = find_client(client_ip, client_port, &cl_history);
        
        if (client_index == -1)
        {
            client_index = add_client(client_ip, client_port, &cl_history);
            if (client_index == -1)
            {
                char* response = "server history is full";
                sendto(sockfd, response, strlen(response), 0, 
                       (struct sockaddr *)&client_addr, addr_len);
                printf("[UDP Server]: history full, rejected client\n");
                continue;
            }
            printf("[UDP Server]: add new client %s:%d\n", 
                   inet_ntoa(client_addr.sin_addr), client_port);
        }
        
        // Получаем номер сообщения
        int msg_number = cl_history.client[client_index].number;
        cl_history.client[client_index].number++;
        
        // Формируем ответ с номером
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "%s %d", buffer, msg_number);
        
        // Отправляем ответ
        sendto(sockfd, response, strlen(response), 0, 
               (struct sockaddr *)&client_addr, addr_len);
        printf("[UDP Server]: sent to client %s:%d: \"%s\"\n", 
               inet_ntoa(client_addr.sin_addr), client_port, response);
    }
    
    close(sockfd);
    return 0;
}