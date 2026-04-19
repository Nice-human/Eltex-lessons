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

typedef struct 
{
    int port;
    int is_busy;
    pthread_t tid;
    int socket_fd;
    pthread_mutex_t mutex;
} service_server_t;

service_server_t servers[MAX_CLIENTS];
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

// Функция обслуживающего сервера (работает в отдельном потоке)
void* service_server_thread(void* arg) 
{
    service_server_t* server = (service_server_t*)arg;
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    while (1) 
    {
        // Принимаем подключение от клиента
        client_fd = accept(server->socket_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) 
        {
            perror("Error service accept");
            continue;
        }

        // Читаем запрос от клиента
        memset(buffer, 0, BUFFER_SIZE);
        read(client_fd, buffer, BUFFER_SIZE - 1);
        printf("[Serving Server (%d)] - клиент подключился с сообщением: %s\n", server->port, buffer);

        // Если клиент прислал "TIME", отправляем текущее время
        if (strncmp(buffer, "TIME", 4) == 0) 
        {
            time_t now = time(NULL);
            char* time_str = ctime(&now);
            send(client_fd, time_str, strlen(time_str), 0);
            printf("[Serving Server (%d)] - клиенту отправлено время: %s", server->port, time_str);
        }

        // Закрываем соединение с клиентом
        close(client_fd);
        
        // Уведомляем главный сервер, что сервер свободен
        pthread_mutex_lock(&server->mutex);
        server->is_busy = 0;
        pthread_mutex_unlock(&server->mutex);
        
        printf("[Serving Server (%d)] - освободился!\n\n", server->port);
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
        
        // Создаём сокет для обслуживающего сервера
        servers[i].socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (servers[i].socket_fd < 0) 
        {
            perror("Error service socket");
            continue;
        }
        
        // Настраиваем адрес обслуживающего сервера
        struct sockaddr_in service_addr;
        memset(&service_addr, 0, sizeof(service_addr));
        service_addr.sin_family = AF_INET;
        service_addr.sin_addr.s_addr = INADDR_ANY;
        service_addr.sin_port = htons(servers[i].port);
        
        // Привязываем сокет к порту
        if (bind(servers[i].socket_fd, (struct sockaddr*)&service_addr, sizeof(service_addr)) < 0) 
        {
            perror("Error service bind");
            close(servers[i].socket_fd);
            continue;
        }
        
        // Начинаем слушать подключения
        if (listen(servers[i].socket_fd, 1) < 0) 
        {
            perror("Error Service listen");
            close(servers[i].socket_fd);
            continue;
        }
        
        // Создаём поток для обслуживающего сервера
        if (pthread_create(&servers[i].tid, NULL, service_server_thread, &servers[i]) != 0) 
        {
            perror("Error pthread_create");
            close(servers[i].socket_fd);
            continue;
        }
        
        printf("Create [Serving Server] on port %d\n", servers[i].port);
    }
    printf("\n");
}

// Поиск свободного обслуживающего сервера
service_server_t* find_free_server() 
{
    for (int i = 0; i < MAX_CLIENTS; i++) 
    {
        pthread_mutex_lock(&servers[i].mutex);
        if (servers[i].is_busy == 0) 
        {
            servers[i].is_busy = 1;
            pthread_mutex_unlock(&servers[i].mutex);
            return &servers[i];
        }
        pthread_mutex_unlock(&servers[i].mutex);
    }
    return NULL;
}

int main() 
{
    int main_fd, client_fd;
    struct sockaddr_in main_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    // Создаём главный серверный сокет
    main_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (main_fd < 0) 
    {
        perror("Error main socket");
        exit(EXIT_FAILURE);
    }
    
    // Настройка адреса главного сервера
    memset(&main_addr, 0, sizeof(main_addr));
    main_addr.sin_family = AF_INET;
    main_addr.sin_addr.s_addr = INADDR_ANY;
    main_addr.sin_port = htons(MAIN_PORT);
    
    // Привязываем сокет
    if (bind(main_fd, (struct sockaddr*)&main_addr, sizeof(main_addr)) < 0) 
    {
        perror("Error Main bind");
        exit(EXIT_FAILURE);
    }
    
    // Начинаем слушать (очередь 10)
    if (listen(main_fd, 10) < 0) 
    {
        perror("Error Main listen");
        exit(EXIT_FAILURE);
    }
    
    printf("\n[Server] - запущен на порту %d\n\n", MAIN_PORT);

    // Инициализируем обслуживающие серверы
    init_servers();
    
    while (1) 
    {
        // Принимаем подключение клиента
        client_fd = accept(main_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) 
        {
            perror("Error accept");
            continue;
        }
        
        printf("[Server] - клиент подключился: %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
        
        // Ищем свободный обслуживающий сервер
        service_server_t* free_server = find_free_server();
        
        if (free_server == NULL) 
        {
            // Нет свободных серверов
            char* msg = "ERROR: No free servers available\n";
            send(client_fd, msg, strlen(msg), 0);
            printf("[Server] - нет свободных обслуживающих серверов для клиента\n");
        } 
        else 
        {
            // Отправляем клиенту адрес свободного сервера
            char endpoint[BUFFER_SIZE];
            snprintf(endpoint, BUFFER_SIZE, "127.0.0.1:%d\n", free_server->port);
            send(client_fd, endpoint, strlen(endpoint), 0);
            printf("[Server] - отправлен эндпоинт клиенту: %s\n", endpoint);
        }
        
        close(client_fd);
    }
    
    close(main_fd);
    return 0;
}