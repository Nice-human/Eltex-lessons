#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAIN_PORT 10888
#define BUFFER_SIZE 256

int main() 
{
    int server_fd, service_fd;
    struct sockaddr_in main_addr, service_addr;
    char buffer[BUFFER_SIZE];
    char ip[16];
    int port;

    // 1) Подключаемся к главному серверу
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
    {
        perror("Error socket");
        exit(EXIT_FAILURE);
    }

    memset(&main_addr, 0, sizeof(main_addr));
    main_addr.sin_family = AF_INET;
    main_addr.sin_port = htons(MAIN_PORT);
    main_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(server_fd, (struct sockaddr*)&main_addr, sizeof(main_addr)) < 0) 
    {
        perror("Error server connect");
        exit(EXIT_FAILURE);
    }

    // 2) Получаем от главного сервера эндпоинт обслуживающего сервера
    memset(buffer, 0, BUFFER_SIZE);
    read(server_fd, buffer, BUFFER_SIZE - 1);
    close(server_fd);

    // Парсим IP и порт
    sscanf(buffer, "%[^:]:%d", ip, &port);
    printf("Получен эндпоинт обслуживающего сервера: %s:%d\n", ip, port);

    // 3) Подключаемся к обслуживающему серверу
    service_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (service_fd < 0) 
    {
        perror("Error service socket");
        exit(EXIT_FAILURE);
    }

    memset(&service_addr, 0, sizeof(service_addr));
    service_addr.sin_family = AF_INET;
    service_addr.sin_port = htons(port);
    service_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(service_fd, (struct sockaddr*)&service_addr, sizeof(service_addr)) < 0) 
    {
        perror("Error connect service");
        exit(EXIT_FAILURE);
    }

    // 4) Отправляем запрос "TIME"
    send(service_fd, "TIME", 4, 0);

    // 5) Получаем ответ (время)
    memset(buffer, 0, BUFFER_SIZE);
    read(service_fd, buffer, BUFFER_SIZE - 1);
    printf("Ответ от обслуживающего сервера: %s", buffer);

    // Завершаем работу
    close(service_fd);
    printf("Клиент завершил работу.\n");
    return 0;
}