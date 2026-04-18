#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() 
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};
    
    // 1. Создание сокета
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // 2. Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Принимать соединения с любого интерфейса
    server_addr.sin_port = htons(PORT);
    
    // 3. Привязка сокета к адресу
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    // 4. Ожидание соединений (максимум 3 в очереди)
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d\n", PORT);
    
    // 5. Принятие соединения от клиента
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Client connected from %s:%d\n", 
           inet_ntoa(client_addr.sin_addr), 
           ntohs(client_addr.sin_port));
    
    // 6. Получение сообщения от клиента
    int bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Received from client: %s\n", buffer);
    }
    
    // 7. Отправка ответа клиенту
    char *response = "Hi!";
    send(client_fd, response, strlen(response), 0);
    printf("Sent to client: %s\n", response);
    
    // 8. Закрытие соединений
    close(client_fd);
    close(server_fd);
    
    return 0;
}