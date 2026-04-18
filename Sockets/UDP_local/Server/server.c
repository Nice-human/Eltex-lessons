#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_socket"
#define BUFFER_SIZE 256

int main() 
{
    int server_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Создание сокета
    server_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (server_fd == -1) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Удаляем старый сокет, если существует
    unlink(SOCKET_PATH);

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Привязка сокета к адресу
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен, ожидает сообщения...\n");

    // Получение сообщения от клиента
    bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                               (struct sockaddr*)&client_addr, &client_addr_len);
    if (bytes_received == -1) 
    {
        perror("recvfrom");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    printf("Сервер получил: %s\n", buffer);
    printf("Адрес клиента: %s\n", client_addr.sun_path);

    // Отправка ответа клиенту
    const char* response = "Hi!";
    if (sendto(server_fd, response, strlen(response), 0, (struct sockaddr*)&client_addr, client_addr_len) == -1) 
    {
        perror("sendto");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Сервер отправил: %s\n", response);

    // Завершение работы
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}