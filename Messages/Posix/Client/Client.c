#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>

#define QUEUE_FROM_SERVER   "/from_server"
#define QUEUE_FROM_CLIENT "/from_client"
#define MAX_MSG_SIZE 256

int main() 
{
    mqd_t mq_from_server, mq_from_client;
    char buffer[MAX_MSG_SIZE];
    unsigned int priority;
    
    // Открытие очереди для получения сообщений от сервера
    mq_from_server = mq_open(QUEUE_FROM_SERVER, O_RDWR);
    if (mq_from_server == (mqd_t)-1) 
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // Открытие очереди для отправки ответа серверу
    mq_from_client = mq_open(QUEUE_FROM_CLIENT, O_RDWR);
    if (mq_from_client == (mqd_t)-1) 
    {
        perror("mq_open (from_client)");
        mq_close(mq_from_server);
        exit(EXIT_FAILURE);
    }

    printf("Клиент: Подключен к очереди: %s\n", QUEUE_FROM_SERVER);
    printf("Клиент: Подключен к очереди: %s\n", QUEUE_FROM_CLIENT);

    // Чтение сообщения от сервера
    ssize_t bytes_read = mq_receive(mq_from_server, buffer, MAX_MSG_SIZE, &priority);
    if (bytes_read == -1) 
    {
        perror("mq_receive");
        mq_close(mq_from_server);
        mq_close(mq_from_client);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';

    printf("Клиент: Получено сообщение от сервера: '%s' (приоритет: %u)\n", buffer, priority);

    // Отправка ответа серверу
    strcpy(buffer, "Hello!");
    if (mq_send(mq_from_client, buffer, strlen(buffer) + 1, 1) == -1) 
    {
        perror("mq_send");
        mq_close(mq_from_server);
        mq_close(mq_from_client);
        exit(EXIT_FAILURE);
    }

    printf("Клиент: Отправлен ответ '%s' серверу\n", buffer);

    // Закрытие очередей
    mq_close(mq_from_server);
    mq_close(mq_from_client);
    
    printf("Клиент: Отключен от очередей\n");

    return 0;
}