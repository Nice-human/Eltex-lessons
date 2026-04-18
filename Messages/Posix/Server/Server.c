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
    mqd_t mq_to_client, mq_from_client;
    char buffer[MAX_MSG_SIZE];
    unsigned int priority;
    struct mq_attr attr;

    // Настройка атрибутов очереди
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Создание очереди для отправки клиенту
    mq_to_client = mq_open(QUEUE_FROM_SERVER, O_CREAT | O_RDWR, 0666, &attr);
    if (mq_to_client == (mqd_t)-1) 
    {
        perror("mq_open (to_client)");
        exit(EXIT_FAILURE);
    }

    // Создание очереди для получения от клиента
    mq_from_client = mq_open(QUEUE_FROM_CLIENT, O_CREAT | O_RDWR, 0666, &attr);
    if (mq_from_client == (mqd_t)-1) 
    {
        perror("mq_open (from_client)");
        mq_close(mq_to_client);
        mq_unlink(QUEUE_FROM_SERVER);
        exit(EXIT_FAILURE);
    }

    printf("Сервер: Cоздана очередь для отправки: %s\n", QUEUE_FROM_SERVER);
    printf("Сервер: Cоздана очередь для получения: %s\n", QUEUE_FROM_CLIENT);

    // Отправка сообщения клиенту
    strcpy(buffer, "Hi!");
    if (mq_send(mq_to_client, buffer, strlen(buffer) + 1, 1) == -1) 
    {
        perror("mq_send");
        mq_close(mq_to_client);
        mq_close(mq_from_client);
        mq_unlink(QUEUE_FROM_SERVER);
        mq_unlink(QUEUE_FROM_CLIENT);
        exit(EXIT_FAILURE);
    }

    printf("Сервер: Отправлено сообщение '%s' в очередь %s\n", buffer, QUEUE_FROM_SERVER);
    printf("Сервер: Ожидание ответа в очереди %s...\n", QUEUE_FROM_CLIENT);

    // Ожидание ответа от клиента
    ssize_t bytes_read = mq_receive(mq_from_client, buffer, MAX_MSG_SIZE, &priority);
    if (bytes_read == -1) 
    {
        perror("mq_receive");
        mq_close(mq_to_client);
        mq_close(mq_from_client);
        mq_unlink(QUEUE_FROM_SERVER);
        mq_unlink(QUEUE_FROM_CLIENT);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';

    printf("Сервер: Получен ответ от клиента: '%s' (приоритет: %u)\n", buffer, priority);

    // Закрытие и удаление очередей
    mq_close(mq_to_client);
    mq_close(mq_from_client);
    mq_unlink(QUEUE_FROM_SERVER);
    mq_unlink(QUEUE_FROM_CLIENT);
    
    printf("Сервер: Очереди удалены\n");

    return 0;
}