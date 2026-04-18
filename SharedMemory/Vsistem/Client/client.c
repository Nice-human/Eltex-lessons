#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHM_SIZE 1024

void sem_wait(int semid, int sem_num) 
{
    struct sembuf op = {sem_num, -1, 0};
    if (semop(semid, &op, 1) == -1) 
    {
        perror("sem_wait");
        exit(1);
    }
}

void sem_signal(int semid, int sem_num) 
{
    struct sembuf op = {sem_num, 1, 0};
    if (semop(semid, &op, 1) == -1) 
    {
        perror("sem_signal");
        exit(1);
    }
}

int main() {
    key_t key = ftok("/tmp", 'R');
    if (key == -1) 
    {
        perror("ftok");
        exit(1);
    }

    // получаем существующий сегмент памяти
    int shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) 
    {
        perror("shmget");
        exit(1);
    }

    // получаем существующий набор семафоров
    int semid = semget(key, 2, 0666);
    if (semid == -1) 
    {
        perror("semget");
        exit(1);
    }

    // подключаемся к разделяемой памяти
    char *shared_mem = (char *)shmat(shmid, NULL, 0);
    if (shared_mem == (char *)(-1)) 
    {
        perror("shmat");
        exit(1);
    }

    // ----- читаем сообщение от сервера -----
    printf("Waiting for server message...\n");
    sem_wait(semid, 0);           // ждём, пока сервер запишет данные
    printf("Client read: %s\n", shared_mem);

    // ----- записываем ответ "Hello!" -----
    strcpy(shared_mem, "Hello!");
    printf("Client wrote: Hello!\n");
    sem_signal(semid, 1);         // сигналим серверу, что ответ готов

    // небольшая задержка перед отключением
    sleep(1);
    
    // отключаемся от памяти
    if (shmdt(shared_mem) == -1) 
    {
        perror("shmdt");
        exit(1);
    }

    printf("Client finished.\n");
    return 0;
}