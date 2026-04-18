#include <stdio.h>      // для printf(), perror()
#include <stdlib.h>     // для exit(), EXIT_FAILURE
#include <string.h>     // для strcpy(), strlen()
#include <sys/types.h>  // для key_t, size_t (часто нужна для msgget/msgsnd/msgrcv)
#include <sys/ipc.h>    // для ftok(), IPC_CREAT, IPC_RMID
#include <sys/msg.h>    // для msgget(), msgsnd(), msgrcv(), msgctl(), struct msgbuf
#include <errno.h>      // для кодов ошибок error
#include <locale.h>     // для setlocale()

#define MSG_SIZE 100

struct msgbuf
{
    long mtype;
    char mtext[MSG_SIZE];
};

int main()
{
    setlocale(LC_ALL, "Russian");

    struct msgbuf msg;
    int msgid;

    // Генерация уникального ключа
    key_t key = ftok("/tmp/mymsg", 'A');

    // Создание или подключение к очереди сообщений с правами 0666
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        perror("key");
        exit(EXIT_FAILURE);
    }

    msg.mtype = 1;
    strcpy(msg.mtext, "Hi!");

    // Отправка сообщения в очередь
    if(msgsnd(msgid, &msg, strlen(msg.mtext) + 1, 0) == -1)
    {
        perror("msgsnd");
        msgctl(msgid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    printf("Server: отправил сообщение (%s) клиенту\n", msg.mtext);

    // Получение сообщения с типом 2 от клиента
    if (msgrcv(msgid, &msg, MSG_SIZE, 2, 0) == -1)
    {
        perror("msgrcv");
        msgctl(msgid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    printf("Server: получил сообщение (%s) от клиента\n", msg.mtext);

    // Удаление очереди сообщений
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    
     printf("Server: очередь сообщений удалена\n");

    return 0;
}