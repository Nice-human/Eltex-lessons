#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_socket"
#define CLIENT_SOCKET_PATH "/tmp/my_client_socket"
#define BUFFER_SIZE 256

int main() 
{
    int client_fd;
    struct sockaddr_un server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    socklen_t server_addr_len = sizeof(server_addr);

    // Создание сокета
    client_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (client_fd == -1) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Удаляем старый клиентский сокет, если существует
    unlink(CLIENT_SOCKET_PATH);

    // Настройка адреса клиента (обязательно для получения ответа)
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_LOCAL;
    strncpy(client_addr.sun_path, CLIENT_SOCKET_PATH, sizeof(client_addr.sun_path) - 1);

    // Привязка клиентского сокета к адресу
    if (bind(client_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) 
    {
        perror("bind");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Отправка сообщения серверу
    const char* message = "Hello!";
    if (sendto(client_fd, message, strlen(message), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
    {
        perror("sendto");
        close(client_fd);
        unlink(CLIENT_SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Клиент отправил: %s\n", message);

    // Получение ответа от сервера
    bytes_received = recvfrom(client_fd, buffer, BUFFER_SIZE - 1, 0,
                               (struct sockaddr*)&server_addr, &server_addr_len);
    if (bytes_received == -1) 
    {
        perror("recvfrom");
        close(client_fd);
        unlink(CLIENT_SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    printf("Клиент получил: %s\n", buffer);

    // Завершение работы
    close(client_fd);
    unlink(CLIENT_SOCKET_PATH);

    return 0;
}