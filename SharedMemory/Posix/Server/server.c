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

    // Удаляем старые объекты, если есть (на случай предыдущего запуска)
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_SERVER_TO_CLIENT);
    sem_unlink(SEM_CLIENT_TO_SERVER);

    // Создаём и открываем разделяемую память
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) 
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Устанавливаем размер
    if (ftruncate(shm_fd, SHM_SIZE) == -1) 
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Отображаем память
    shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) 
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Создаём семафоры
    sem_s2c = sem_open(SEM_SERVER_TO_CLIENT, O_CREAT, 0666, 0);
    sem_c2s = sem_open(SEM_CLIENT_TO_SERVER, O_CREAT, 0666, 0);
    if (sem_s2c == SEM_FAILED || sem_c2s == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Пишем сообщение "Hi!" в разделяемую память
    snprintf(shm_ptr, SHM_SIZE, "Hi!");
    printf("Server wrote: %s\n", shm_ptr);

    // Сигналим клиенту, что данные готовы
    sem_post(sem_s2c);

    // Ждём ответа от клиента
    sem_wait(sem_c2s);
    printf("Server received: %s\n", shm_ptr);

    // Очистка
    munmap(shm_ptr, SHM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);

    sem_close(sem_s2c);
    sem_close(sem_c2s);
    sem_unlink(SEM_SERVER_TO_CLIENT);
    sem_unlink(SEM_CLIENT_TO_SERVER);

    return 0;
}