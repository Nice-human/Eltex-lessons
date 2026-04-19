#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define MAIN_PORT 10888
#define MAX_CLIENTS 3
#define BUFFER_SIZE 256

// Структура для передачи данных в поток обслуживающего сервера
typedef struct 
{
    int service_port;       // порт, на котором будет работать обслуживающий сервер
} service_thread_args_t;

// Глобальная переменная для хранения ID третьего потока
pthread_t third_thread;

// Функция обслуживающего сервера (работает в отдельном потоке)
void* service_server_thread(void* arg) 
{
    service_thread_args_t* args = (service_thread_args_t*)arg;
    int service_fd, client_fd;
    struct sockaddr_in service_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Создаём сокет для обслуживающего сервера
    service_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (service_fd < 0) 
    {
        perror("Error service socket");
        pthread_exit(NULL);
    }

    // Настраиваем адрес обслуживающего сервера (INADDR_ANY + заданный порт)
    memset(&service_addr, 0, sizeof(service_addr));
    service_addr.sin_family = AF_INET;
    service_addr.sin_addr.s_addr = INADDR_ANY;
    service_addr.sin_port = htons(args->service_port);

    // Привязываем сокет к порту
    if (bind(service_fd, (struct sockaddr*)&service_addr, sizeof(service_addr)) < 0) 
    {
        perror("Error service bind");
        close(service_fd);
        pthread_exit(NULL);
    }

    // Начинаем слушать подключения (очередь 1)
    if (listen(service_fd, 1) < 0) 
    {
        perror("Error service listen");
        close(service_fd);
        pthread_exit(NULL);
    }

    // Принимаем подключение от клиента
    client_fd = accept(service_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) 
    {
        perror("Error service accept");
        close(service_fd);
        pthread_exit(NULL);
    }

    // Читаем запрос от клиента
    memset(buffer, 0, BUFFER_SIZE);
    read(client_fd, buffer, BUFFER_SIZE - 1);
    printf("[Serving Server] - получен запрос: %s\n", buffer);

    // Если клиент прислал "TIME", отправляем текущее время
    if (strncmp(buffer, "TIME", 4) == 0) 
    {
        time_t now = time(NULL);
        char* time_str = ctime(&now);
        send(client_fd, time_str, strlen(time_str), 0);
        printf("[Serving Server] - отправлено время: %s", time_str);
    }

    // Закрываем соединения
    close(client_fd);
    close(service_fd);
    printf("[Serving Server] - завершил работу на порту %d\n", args->service_port);
    free(args);
    return NULL;
}

// Функция для получения свободного порта
int get_free_port() 
{
    static int current_port = 10900;
    return current_port++;
}

int main() 
{
    int server_fd, client_fd;
    int clients_served = 0;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    pthread_t tid;

    // Создаём главный серверный сокет
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
    {
        perror("Error socket");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса главного сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(MAIN_PORT);

    // Привязываем сокет
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Error bind");
        exit(EXIT_FAILURE);
    }

    // Начинаем слушать (очередь 5)
    if (listen(server_fd, 3) < 0) 
    {
        perror("Error listen");
        exit(EXIT_FAILURE);
    }

    printf("Главный сервер запущен на порту %d\n", MAIN_PORT);

    while (clients_served < MAX_CLIENTS) 
    {
        // Принимаем подключение клиента
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) 
        {
            perror("Error accept");
            continue;
        }

        printf("\n[Server] - клиент %d подключился: %s:%d\n", 
               clients_served + 1, 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port));

        // 1) Создаём поток для обслуживающего сервера
        service_thread_args_t* args = malloc(sizeof(service_thread_args_t));
        args->service_port = get_free_port(); // получаем свободный порт

        if (pthread_create(&tid, NULL, service_server_thread, (void*)args) != 0) 
        {
            perror("Error pthread_create");
            free(args);
            close(client_fd);
            continue;
        }

        //    Передаём клиенту эндпоинт: "IP:PORT"
        char endpoint[BUFFER_SIZE];
        snprintf(endpoint, BUFFER_SIZE, "127.0.0.1:%d\n", args->service_port);
        send(client_fd, endpoint, strlen(endpoint), 0);
        printf("[Server] - отправлен эндпоинт клиенту: %s\n", endpoint);

        close(client_fd);

        if (clients_served == 2)  // третий клиент
        {
            third_thread = tid;  // Сохраняем ID третьего потока
        }
        else
        {
            pthread_detach(tid);  // Первые два потока работают в фоне
        }

        clients_served++;
    }

    pthread_join(third_thread, NULL);
    printf("\nВсе три клиента обслужены. [Server] - завершает работу!\n");
    close(server_fd);
    return 0;
}