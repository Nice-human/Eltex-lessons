#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHM_SIZE 1024

// Семафоры:
// sem[0] - готовность данных от сервера (1 - данные готовы, 0 - нет)
// sem[1] - готовность данных от клиента (1 - данные готовы, 0 - нет)

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

int main() 
{
    key_t key = ftok("/tmp", 'R');
    if (key == -1) 
    {
        perror("ftok");
        exit(1);
    }

    // создаём сегмент разделяемой памяти
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) 
    {
        perror("shmget");
        exit(1);
    }

    // создаём набор семафоров (2 семафора)
    int semid = semget(key, 2, IPC_CREAT | 0666);
    if (semid == -1) 
    {
        perror("semget");
        exit(1);
    }

    // инициализируем семафоры:
    // sem0 = 0 (данных от сервера пока нет)
    // sem1 = 0 (данных от клиента пока нет)
    if (semctl(semid, 0, SETVAL, 0) == -1) 
    {
        perror("semctl sem0");
        exit(1);
    }
    if (semctl(semid, 1, SETVAL, 0) == -1) 
    {
        perror("semctl sem1");
        exit(1);
    }

    // подключаемся к разделяемой памяти
    char *shared_mem = (char *)shmat(shmid, NULL, 0);
    if (shared_mem == (char *)(-1)) 
    {
        perror("shmat");
        exit(1);
    }

    // ----- запись "Hi!" -----
    strcpy(shared_mem, "Hi!");
    printf("Server wrote: %s\n", shared_mem);
    sem_signal(semid, 0);         // сигналим, что данные готовы для клиента

    // ----- ожидание ответа от клиента -----
    printf("Waiting for client response...\n");
    sem_wait(semid, 1);           // ждём, пока клиент запишет ответ
    printf("Server received: %s\n", shared_mem);

    // небольшая задержка, чтобы клиент успел отключиться
    sleep(1);
    
    // отключаемся от памяти
    if (shmdt(shared_mem) == -1) 
    {
        perror("shmdt");
        exit(1);
    }

    // удаляем сегмент памяти
    if (shmctl(shmid, IPC_RMID, NULL) == -1) 
    {
        perror("shmctl");
        exit(1);
    }

    // удаляем семафоры
    if (semctl(semid, 0, IPC_RMID) == -1) 
    {
        perror("semctl IPC_RMID");
        exit(1);
    }

    printf("Shared memory and semaphores removed.\n");
    return 0;
}