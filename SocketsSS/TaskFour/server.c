#include <stdio.h>      // printf, perror
#include <stdlib.h>     // exit
#include <string.h>     // memset, strcmp, strlen, strcspn
#include <unistd.h>     // close, read, write
#include <sys/socket.h> // socket, bind, listen, accept, sendto, recvfrom
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // inet_ntoa, ntohs, htons
#include <sys/select.h> // fd_set, FD_ZERO, FD_SET, FD_ISSET, select
#include <time.h>       // time, localtime, strftime

#define TCP_PORT 10100
#define UDP_PORT 10200
#define BUFFER_SIZE 1024

// Функция для получения текущего времени в строковом формате
void get_current_time(char *buffer, size_t size) 
{
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    
    // Безопасное форматирование без символа новой строки
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

// Обработка TCP клиента
void handle_tcp_client(int tcp_fd) 
{
    char buffer[BUFFER_SIZE];
    char time_str[BUFFER_SIZE];
    ssize_t bytes_read;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(tcp_fd, (struct sockaddr*)&client_addr, &addr_len);
    
    if (client_fd < 0) 
    {
        perror("Error accept");
    }
    
    // Читаем запрос от клиента
    bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) 
    {
        if (bytes_read == 0) 
        {
            printf("[Server TCP] - клиент закрыл соединение\n");
        } 
        else 
        {
            perror("[Server TCP] - ошибка чтения");
        }
        close(client_fd);
        return;
    }

    printf("[Server TCP] - клиент подключен из %s:%d\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    buffer[bytes_read] = '\0';
    // Удаляем символ новой строки если есть
    buffer[strcspn(buffer, "\n")] = 0;
    
    printf("[Server TCP] - получен запрос от %s:%d: '%s'\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
    
    // Проверяем запрос TIME
    if (strcmp(buffer, "TIME") == 0) 
    {
        get_current_time(time_str, sizeof(time_str));
        write(client_fd, time_str, strlen(time_str));
        printf("[Server TCP] - отправлено время: %s\n", time_str);
    } 
    else 
    {
        char *error_msg = "ERROR: TIME\n";
        write(client_fd, error_msg, strlen(error_msg));
        printf("[Server TCP] - неизвестная команда\n");
    }
    
    close(client_fd);
    printf("[Server TCP] - соединение закрыто\n\n");
}

// Обработка UDP клиента
void handle_udp_client(int udp_fd) 
{
    char buffer[BUFFER_SIZE];
    char time_str[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    ssize_t bytes_read;
    
    // Читаем датаграмму от клиента
    bytes_read = recvfrom(udp_fd, buffer, sizeof(buffer) - 1, 0,
                          (struct sockaddr*)&client_addr, &addr_len);
    
    if (bytes_read < 0) 
    {
        perror("[Server UDP] - ошибка чтения");
        return;
    }

    // ВЫВОДИМ ИНФОРМАЦИЮ О ПОДКЛЮЧЕНИИ
    printf("[Server UDP] - клиент подключен из %s:%d\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    buffer[bytes_read] = '\0';
    // Удаляем символ новой строки если есть
    buffer[strcspn(buffer, "\n")] = 0;
    
    printf("[Server UDP] - получен запрос от %s:%d: '%s'\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
    
    // Проверяем запрос TIME
    if (strcmp(buffer, "TIME") == 0) 
    {
        get_current_time(time_str, sizeof(time_str));
        sendto(udp_fd, time_str, strlen(time_str), 0,
               (struct sockaddr*)&client_addr, addr_len);
        printf("[Server UDP] - отправлено время: %s\n", time_str);
    } 
    else 
    {
        char *error_msg = "ERROR: Unknown command. Use TIME\n";
        sendto(udp_fd, error_msg, strlen(error_msg), 0,
               (struct sockaddr*)&client_addr, addr_len);
        printf("[Server UDP] - неизвестная команда\n");
    }
    printf("[Server UDP] - соединение закрыто\n\n");
}

int main() 
{
    int tcp_fd, udp_fd, max_fd;
    fd_set read_fds;
    
    // Создание TCP сокета
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_fd < 0) 
    {
        perror("Error TCP socket");
        exit(EXIT_FAILURE);
    }
    
    // Настройка TCP сокета для переиспользования адреса
    int opt = 1;
    if (setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    {
        perror("Error setsockopt TCP");
        exit(EXIT_FAILURE);
    }
    
    // Настройка TCP адреса
    struct sockaddr_in tcp_addr;
    memset(&tcp_addr, 0, sizeof(tcp_addr));
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons(TCP_PORT);
    
    // Привязка TCP сокета
    if (bind(tcp_fd, (struct sockaddr*)&tcp_addr, sizeof(tcp_addr)) < 0) 
    {
        perror("Error bind TCP");
        exit(EXIT_FAILURE);
    }
    
    // Прослушивание TCP сокета
    if (listen(tcp_fd, 5) < 0) 
    {
        perror("Error listen TCP");
        exit(EXIT_FAILURE);
    }
    
    printf("[Server TCP] - создан на порту %d\n", TCP_PORT);
    
    // Создание UDP сокета
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) 
    {
        perror("Error socket UDP");
        exit(EXIT_FAILURE);
    }

    // Настройка SO_REUSEADDR для UDP  ← ДОБАВЛЕНО!
    if (setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    {
        perror("Error setsockopt UDP");
        exit(EXIT_FAILURE);
    }
    
    // Настройка UDP адреса
    struct sockaddr_in udp_addr;
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(UDP_PORT);
    
    // Привязка UDP сокета
    if (bind(udp_fd, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) < 0) 
    {
        perror("Error bind UDP");
        exit(EXIT_FAILURE);
    }
    
    printf("[Server UDP] - создан на порту %d\n\n", UDP_PORT);
    
    // Главный цикл сервера с использованием select
    while (1) 
    {
        FD_ZERO(&read_fds);         // Очищаем список (убираем всех)
        FD_SET(tcp_fd, &read_fds);  // Добавляем TCP сокет в список
        FD_SET(udp_fd, &read_fds);  // Добавляем UDP сокет в список
        
        max_fd = (tcp_fd > udp_fd) ? tcp_fd : udp_fd;
        
        // Ожидание активности на любом из сокетов
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) 
        {
            perror("Error select");
            continue;
        }
        
        // Проверка активности на TCP сокете (новое подключение)
        if (FD_ISSET(tcp_fd, &read_fds)) 
        {                
            handle_tcp_client(tcp_fd);
        }
        
        // Проверка активности на UDP сокете (входящая датаграмма)
        if (FD_ISSET(udp_fd, &read_fds)) 
        {
            handle_udp_client(udp_fd);
        }
    }
    
    // Закрытие сокетов (никогда не достигается в данной реализации)
    close(tcp_fd);
    close(udp_fd);
    
    return 0;
}