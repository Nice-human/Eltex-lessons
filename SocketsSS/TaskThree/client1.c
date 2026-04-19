#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAIN_PORT 10100
#define BUFFER_SIZE 256

int main() 
{
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Подключаемся к главному серверу
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) 
    {
        perror("Error socket");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(MAIN_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Error connect");
        exit(EXIT_FAILURE);
    }
    
    printf("Подключен к главному серверу\n");
    
    // Отправляем сообщение TIME
    char* message = "TIME\n";
    send(client_fd, message, strlen(message), 0);
    printf("Отправлено сообщение: %s", message);
    
    // Получаем ответ (время)
    memset(buffer, 0, BUFFER_SIZE);
    read(client_fd, buffer, BUFFER_SIZE - 1);
    printf("Получен ответ: %s", buffer);
    
    close(client_fd);
    printf("Клиент завершил работу\n");
    
    return 0;
}