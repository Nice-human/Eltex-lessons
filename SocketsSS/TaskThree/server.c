#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define MAIN_PORT 10100
#define START_PORT 10101
#define MAX_CLIENTS 3
#define BUFFER_SIZE 256
#define QUEUE_SIZE 10

typedef struct 
{
    int port;
    int is_busy;
    int socket_fd;
    pthread_t tid;  
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} service_server_t;

typedef struct 
{
    int client_fd;
    struct sockaddr_in client_addr;
    char message[BUFFER_SIZE];
} client_queue_item_t;

service_server_t servers[MAX_CLIENTS];
client_queue_item_t client_queue[QUEUE_SIZE];
int queue_front = 0;
int queue_rear = 0;
int queue_count = 0;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

// Функция добавления клиента в очередь
void enqueue_client(int client_fd, struct sockaddr_in client_addr, char* message) 
{
    pthread_mutex_lock(&queue_mutex);
    
    if (queue_count < QUEUE_SIZE) 
    {
        client_queue[queue_rear].client_fd = client_fd;
        client_queue[queue_rear].client_addr = client_addr;
        strcpy(client_queue[queue_rear].message, message);
        queue_rear = (queue_rear + 1) % QUEUE_SIZE;
        queue_count++;
        printf("[Server] - клиент добавлен в очередь с сообщением: %s\n", message);
        pthread_cond_signal(&queue_cond);
    } 
    else 
    {
        printf("[Server] - очередь переполнена!\n");
        char* msg = "ERROR: Queue full\n";
        send(client_fd, msg, strlen(msg), 0);
        close(client_fd);
    }
    
    pthread_mutex_unlock(&queue_mutex);
}

// Функция извлечения клиента из очереди
int dequeue_client(struct sockaddr_in* client_addr, char* message) 
{
    pthread_mutex_lock(&queue_mutex);
    
    while (queue_count == 0) 
    {
        pthread_cond_wait(&queue_cond, &queue_mutex);
    }
    
    int client_fd = client_queue[queue_front].client_fd;
    *client_addr = client_queue[queue_front].client_addr;
    strcpy(message, client_queue[queue_front].message);
    queue_front = (queue_front + 1) % QUEUE_SIZE;
    queue_count--;
    
    pthread_mutex_unlock(&queue_mutex);
    return client_fd;
}

// Функция обслуживающего сервера
void* service_server_thread(void* arg) 
{
    service_server_t* server = (service_server_t*)arg;
    struct sockaddr_in client_addr;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    
    while (1) 
    {
        // Ждём освобождения сервера
        pthread_mutex_lock(&server->mutex);
        while (server->is_busy == 1) 
        {
            pthread_cond_wait(&server->cond, &server->mutex);
        }
        server->is_busy = 1;
        pthread_mutex_unlock(&server->mutex);
        
        // Берём клиента из очереди вместе с его сообщением
        int client_fd = dequeue_client(&client_addr, message);
        
        printf("[Serving server (%d)] - получен клиент %s:%d с сообщением: %s", 
               server->port,
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               message);
        
        // Проверяем сообщение на "TIME"
        if (strncmp(message, "TIME", 4) == 0) 
        {
            time_t now = time(NULL);
            char* time_str = ctime(&now);
            send(client_fd, time_str, strlen(time_str), 0);
            printf("[Serving server (%d)] - клиенту отправлено время: %s", server->port, time_str);
        } 
        else 
        {
            char* error_msg = "ERROR: STRING TIME\n";
            send(client_fd, error_msg, strlen(error_msg), 0);
            printf("[Serving server (%d)] - не верная команда: %s", server->port, message);
        }
        
        // Закрываем соединение с клиентом
        close(client_fd);
        
        // Освобождаем сервер
        pthread_mutex_lock(&server->mutex);
        server->is_busy = 0;
        pthread_cond_signal(&server->cond);
        pthread_mutex_unlock(&server->mutex);
        
        printf("[Serving server (%d)] - освободился!\n", server->port);
    }
    
    return NULL;
}

// Инициализация обслуживающих серверов
void init_servers() 
{
    for (int i = 0; i < MAX_CLIENTS; i++) 
    {
        servers[i].port = START_PORT + i;
        servers[i].is_busy = 0;
        servers[i].socket_fd = -1;
        pthread_mutex_init(&servers[i].mutex, NULL);
        pthread_cond_init(&servers[i].cond, NULL);
        
        servers[i].socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (servers[i].socket_fd < 0) 
        {
            perror("Error socket");
            continue;
        }
        
        struct sockaddr_in service_addr;
        memset(&service_addr, 0, sizeof(service_addr));
        service_addr.sin_family = AF_INET;
        service_addr.sin_addr.s_addr = INADDR_ANY;
        service_addr.sin_port = htons(servers[i].port);
        
        if (bind(servers[i].socket_fd, (struct sockaddr*)&service_addr, sizeof(service_addr)) < 0) 
        {
            perror("Error bind");
            close(servers[i].socket_fd);
            continue;
        }
        
        if (listen(servers[i].socket_fd, 1) < 0) 
        {
            perror("Error listen");
            close(servers[i].socket_fd);
            continue;
        }
        
        if (pthread_create(&servers[i].tid, NULL, service_server_thread, &servers[i]) != 0) 
        {
            perror("Error pthread_create");
            close(servers[i].socket_fd);
            continue;
        }
        
        printf("Create [Serving server] on port %d\n", servers[i].port);
    }
}

int main() 
{
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(MAIN_PORT);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 10) < 0) 
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("[Server] - запущен на порту %d\n", MAIN_PORT);

    init_servers();
    
    while (1) 
    {
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) 
        {
            perror("Error accept");
            continue;
        }
        
        printf("\n[Server] - клиент подключился: %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
        
        // Читаем сообщение от клиента
        memset(buffer, 0, BUFFER_SIZE);
        read(client_fd, buffer, BUFFER_SIZE - 1);
        printf("[Server] - клиент прислал сообщение: %s", buffer);
        
        // Добавляем клиента в очередь с его сообщением
        enqueue_client(client_fd, client_addr, buffer);
    }
    
    close(server_fd);
    return 0;
}