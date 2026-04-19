#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TCP_PORT 10100
#define BUFFER_SIZE 1024

int main() 
{
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Создание TCP сокета
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) 
    {
        perror("Error socket");
        exit(EXIT_FAILURE);
    }
    
    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Подключение к серверу
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Error connect");
        exit(EXIT_FAILURE);
    }
    
    printf("[TCP клиент] Подключен к серверу\n");
    
    // Отправка запроса TIME
    char *request = "TIME";
    write(client_fd, request, strlen(request));
    printf("[TCP клиент] Отправлен запрос: %s\n", request);
    
    // Чтение ответа
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) 
    {
        buffer[bytes_read] = '\0';
        printf("[TCP клиент] Получен ответ: %s\n", buffer);
    } 
    else 
    {
        printf("[TCP клиент] Ошибка чтения ответа\n");
    }
    
    // Закрытие соединения
    close(client_fd);
    printf("[TCP клиент] Завершение работы\n");
    
    return 0;
}