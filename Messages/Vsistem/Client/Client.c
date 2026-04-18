#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <locale.h>

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

    // Подключение к существующей очереди сообщений
    msgid = msgget(key, 0666);
    if (msgid == -1)
    {
        perror("key");
        exit(EXIT_FAILURE);
    }

    // Получение сообщения с типом 1 (от сервера)
    if (msgrcv(msgid, &msg, MSG_SIZE, 1, 0) == -1)
    {
        perror("msgrcv");
        msgctl(msgid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    printf("Client: получил сообщение (%s) от сервера\n", msg.mtext);



    msg.mtype = 2; // установка типа сообщения
    strcpy(msg.mtext, "Hello!");

    // Отправка ответного сообщения серверу
    if(msgsnd(msgid, &msg, strlen(msg.mtext) + 1, 0) == -1)
    {
        perror("msgsnd");
        msgctl(msgid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    printf("Client: отправил сообщение (%s) серверу\n", msg.mtext);

    


    return 0;
}