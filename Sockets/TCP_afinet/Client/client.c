#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    // 1. Создание сокета
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // 2. Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // Преобразование IP-адреса (127.0.0.1 - localhost)
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    // 3. Подключение к серверу
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to server\n");
    
    // 4. Отправка сообщения серверу
    char *message = "Hello!";
    send(sock_fd, message, strlen(message), 0);
    printf("Sent to server: %s\n", message);
    
    // 5. Получение ответа от сервера
    int bytes_read = read(sock_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Received from server: %s\n", buffer);
    }
    
    // 6. Закрытие соединения
    close(sock_fd);
    
    return 0;
}