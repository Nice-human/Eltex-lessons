#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

#define SOCKET_PATH "/tmp/mysocket"
#define BUFFER_SIZE 256

int main() 
{
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    ssize_t num_bytes;

    // Удаляем старый сокет, если существует
    unlink(SOCKET_PATH);
    

    // Создаём сокет
    server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (server_fd == -1) 
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
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Привязываем сокет к адресу
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
    {
        perror("bind: не удалось привязать сокет");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Начинаем слушать
    if (listen(server_fd, 2) == -1) 
    {
        perror("listen: не удалось начать прослушивание");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Сервер успешно запущен. Сокет: %s\n", SOCKET_PATH);
    printf("Ожидание подключения клиента...\n");

    // Принимаем подключение
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd == -1) 
    {
        perror("accept: не удалось принять подключение");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Клиент успешно подключился\n");

    // Получаем данные от клиента
    num_bytes = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (num_bytes == -1) 
    {
        perror("read: ошибка при чтении данных");
        close(client_fd);
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }
    
    if (num_bytes == 0) 
    {
        printf("Клиент закрыл соединение\n");
        close(client_fd);
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_SUCCESS);
    }

    buffer[num_bytes] = '\0';
    printf("Получено от клиента: %s\n", buffer);

    // Отправляем ответ клиенту
    const char* reply = "Hi!";
    if (write(client_fd, reply, strlen(reply)) == -1) 
    {
        perror("write: ошибка при отправке данных");
        close(client_fd);
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Отправлено клиенту: %s\n", reply);

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    

    printf("Сервер завершил работу\n");
    return 0;
}