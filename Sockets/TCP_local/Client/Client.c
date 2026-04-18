#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/mysocket"
#define BUFFER_SIZE 256

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    ssize_t num_bytes;

    // Создаём сокет
    client_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (client_fd == -1) 
    {
        perror("socket: не удалось создать сокет");
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    if (strlen(SOCKET_PATH) >= sizeof(server_addr.sun_path)) 
    {
        fprintf(stderr, "Ошибка: путь к сокету слишком длинный\n");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    strcpy(server_addr.sun_path, SOCKET_PATH);

    printf("Попытка подключения к серверу по адресу: %s\n", SOCKET_PATH);

    // Подключаемся к серверу
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
    {
        perror("connect: не удалось подключиться к серверу");
        fprintf(stderr, "Убедитесь, что сервер запущен и сокет существует\n");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Успешно подключено к серверу!\n");

    // Отправляем сообщение серверу
    const char* message = "Hello!";
    if (write(client_fd, message, strlen(message)) == -1) 
    {
        perror("write: ошибка при отправке сообщения");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Отправлено серверу: %s\n", message);

    // Получаем ответ от сервера
    num_bytes = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (num_bytes == -1) 
    {
        perror("read: ошибка при чтении ответа");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    if (num_bytes == 0) 
    {
        printf("Сервер закрыл соединение\n");
        close(client_fd);
        exit(EXIT_SUCCESS);
    }

    buffer[num_bytes] = '\0';
    printf("Получено от сервера: %s\n", buffer);

    // Закрываем соединение
    close(client_fd);

    printf("Клиент завершил работу\n");
    return 0;
}