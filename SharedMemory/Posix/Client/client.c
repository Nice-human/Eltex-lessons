#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

#define SHM_NAME "/myshm"
#define SEM_SERVER_TO_CLIENT "/sem_s2c"
#define SEM_CLIENT_TO_SERVER "/sem_c2s"
#define SHM_SIZE 256

int main() 
{
    int shm_fd;
    char *shm_ptr;
    sem_t *sem_s2c, *sem_c2s;

    // Открываем существующую разделяемую память
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) 
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Отображаем память
    shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) 
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Открываем существующие семафоры
    sem_s2c = sem_open(SEM_SERVER_TO_CLIENT, 0);
    sem_c2s = sem_open(SEM_CLIENT_TO_SERVER, 0);
    if (sem_s2c == SEM_FAILED || sem_c2s == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Ждём, пока сервер запишет сообщение
    sem_wait(sem_s2c);
    printf("Client read: %s\n", shm_ptr);

    // Отвечаем "Hello!"
    snprintf(shm_ptr, SHM_SIZE, "Hello!");
    printf("Client wrote: %s\n", shm_ptr);

    // Сигналим серверу, что ответ готов
    sem_post(sem_c2s);

    // Очистка (но не удаляем память — это делает сервер)
    munmap(shm_ptr, SHM_SIZE);
    close(shm_fd);

    sem_close(sem_s2c);
    sem_close(sem_c2s);

    return 0;
}