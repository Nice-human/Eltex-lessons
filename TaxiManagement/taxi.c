#include <stdio.h>       // printf(), perror(), fgets(), snprintf()
#include <stdlib.h>      // exit(), sscanf()
#include <string.h>      // strlen(), strcmp(), strncmp()
#include <unistd.h>      // fork(), pipe(), read(), write(), close(), kill()
#include <sys/types.h>   // pid_t, time_t
#include <signal.h>      // SIGTERM
#include <sys/wait.h>    // waitpid()
#include <sys/select.h>  // select(), FD_SET(), FD_ISSET()
#include <sys/time.h>    // struct timeval
#include <time.h>        // time(), time_t
#include <errno.h>       // errno

#define MAX_DRIVERS 100  // максимальное количество водителей
#define BUFFER_SIZE 256  // размер буфера для обмена сообщениями
#define PIPE_READ 0      // индекс для чтения из канала (pipe)
#define PIPE_WRITE 1     // индекс для записи в канал (pipe)

// Структура для хранения информации о водителе
typedef struct
{
    pid_t pid;     // PID процесса-водителя
    int write_fd;  // дескриптор для отправки команд водителю
    int read_fd;   // дескриптор для чтения ответов от водителя
    int active;    // 1 - водитель активен, 0 - завершён
} Driver;

Driver drivers[MAX_DRIVERS];  // массив всех водителей
int driver_count = 0;         // количество созданных водителей

// Функция для отправки команды водителю и получения ответа
int send_command(pid_t pid, const char* command, char* response, size_t response_size)
{
    int idx = -1;
    // ищем водителя с указанным PID и активным статусом
    for (int i = 0; i < driver_count; i++)
    {
        if (drivers[i].pid == pid && drivers[i].active)
        {
            idx = i;
            break;
        }
    }

    if (idx == -1)  // водитель не найден
    {
        snprintf(response, response_size, "Driver %d not found", pid);
        return -1;
    }

    // Отправляем команду
    write(drivers[idx].write_fd, command, strlen(command) + 1);

    // Читаем ответ
    int n = read(drivers[idx].read_fd, response, response_size - 1);
    if (n <= 0)  // ошибка чтения или канал закрыт
    {
        snprintf(response, response_size, "Driver %d not responding", pid);
        return -1;
    }
    response[n] = '\0';

    return 0;
}

// Функция водителя (дочерний процесс)
void driver_main(int read_fd, int write_fd)
{
    char buffer[BUFFER_SIZE];
    int status = 0;       // 0 - Available (свободен), 1 - Busy (занят)
    time_t end_time = 0;  // время окончания текущей задачи

    while (1)
    {
        fd_set read_set;
        FD_ZERO(&read_set);          // очищаем множество дескрипторов
        FD_SET(read_fd, &read_set);  // добавляем канал чтения от диспетчера

        struct timeval tv;
        struct timeval* timeout_ptr = NULL;  // NULL = ждать бесконечно

        if (status == 1)  // если водитель занят
        {
            time_t now = time(NULL);
            long remaining = end_time - now;  // сколько осталось секунд

            if (remaining <= 0)  // задача уже завершилась
            {
                status = 0;  // становимся свободным
                end_time = 0;
            }
            else  // задача ещё выполняется
            {
                tv.tv_sec = remaining;  // устанавливаем таймаут на оставшееся время
                tv.tv_usec = 0;
                timeout_ptr = &tv;  // указываем на структуру с таймаутом
            }
        }

        // ждём событие: данные в канале ИЛИ истечение таймаута
        int ret = select(read_fd + 1, &read_set, NULL, NULL, timeout_ptr);

        if (ret < 0)  // ошибка
        {
            perror("select");
            break;
        }

        if (ret > 0 && FD_ISSET(read_fd, &read_set))  // есть данные для чтения
        {
            int n = read(read_fd, buffer, BUFFER_SIZE - 1);
            if (n <= 0) break;  // канал закрыт - завершаем водителя

            buffer[n] = '\0';

            if (strncmp(buffer, "STATUS", 6) == 0)  // запрос статуса
            {
                if (status == 0)
                {
                    write(write_fd, "Available", 10);
                }
                else
                {
                    time_t now = time(NULL);
                    long remaining = end_time - now;
                    if (remaining < 0) remaining = 0;
                    char response[BUFFER_SIZE];
                    snprintf(response, BUFFER_SIZE, "Busy %ld", remaining);
                    write(write_fd, response, strlen(response) + 1);
                }
            }
            else if (strncmp(buffer, "TASK", 4) == 0)  // новая задача
            {
                int task_time;
                sscanf(buffer, "TASK %d", &task_time);

                if (status == 0)  // свободен (принимаем задачу)
                {
                    status = 1;
                    end_time = time(NULL) + task_time;
                    write(write_fd, "OK", 3);
                }
                else  // занят (отказ)
                {
                    time_t now = time(NULL);
                    long remaining = end_time - now;
                    if (remaining < 0) remaining = 0;
                    char response[BUFFER_SIZE];
                    snprintf(response, BUFFER_SIZE, "Busy %ld", remaining);
                    write(write_fd, response, strlen(response) + 1);
                }
            }
        }
        else if (ret == 0 && status == 1)  // таймаут (задача завершена)
        {
            status = 0;
            end_time = 0;
        }
    }

    // закрываем каналы перед выходом
    close(read_fd);
    close(write_fd);
    exit(0);  // завершаем процесс водителя
}

// Создание нового водителя
void create_driver()
{
    int free_slot = -1;

    // Сначала ищем свободный (неактивный) слот
    for (int i = 0; i < driver_count; i++)
    {
        if (!drivers[i].active)
        {
            free_slot = i;
            break;
        }
    }

    // Если свободного слота нет, создаём новый
    if (free_slot == -1)
    {
        if (driver_count >= MAX_DRIVERS)
        {
            printf("Maximum number of drivers reached\n");
            return;
        }
        free_slot = driver_count;
        driver_count++;
    }

    // Создаём каналы
    int parent_to_child[2];
    int child_to_parent[2];

    // Проверка - успешно ли создались каналы
    if (pipe(parent_to_child) < 0 || pipe(child_to_parent) < 0)
    {
        perror("pipe");
        return;
    }

    pid_t pid = fork();  // Создание дочернего процесса

    if (pid < 0)  // Ошибка
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        // Дочерний процесс (водитель)
        close(parent_to_child[PIPE_WRITE]);
        close(child_to_parent[PIPE_READ]);
        driver_main(parent_to_child[PIPE_READ], child_to_parent[PIPE_WRITE]);
    }
    else
    {
        // Родительский процесс (диспетчер)
        close(parent_to_child[PIPE_READ]);
        close(child_to_parent[PIPE_WRITE]);

        // Переиспользуем найденный слот
        drivers[free_slot].pid = pid;
        drivers[free_slot].write_fd = parent_to_child[PIPE_WRITE];
        drivers[free_slot].read_fd = child_to_parent[PIPE_READ];
        drivers[free_slot].active = 1;

        printf("Created driver with PID: %d (slot %d)\n", pid, free_slot);
    }
}

// Отправка задачи водителю
void send_task(pid_t pid, int task_timer)
{
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    snprintf(command, BUFFER_SIZE, "TASK %d", task_timer);

    if (send_command(pid, command, response, BUFFER_SIZE) == 0)
    {
        if (strncmp(response, "Busy", 4) == 0)  // водитель занят
        {
            printf("%s\n", response);
        }
        else  // задача принята
        {
            printf("Task sent to driver %d, will take %d seconds\n", pid, task_timer);
        }
    }
    else  // ошибка (водитель не найден или не отвечает)
    {
        printf("%s\n", response);
    }
}

// Получение статуса водителя
void get_status(pid_t pid)
{
    char response[BUFFER_SIZE];

    if (send_command(pid, "STATUS", response, BUFFER_SIZE) == 0)
    {
        printf("Driver %d: %s\n", pid, response);
    }
    else
    {
        printf("%s\n", response);
    }
}

// Получение списка всех водителей
void get_drivers()
{
    if (driver_count == 0)
    {
        printf("No drivers created\n");
        return;
    }

    for (int i = 0; i < driver_count; i++)
    {
        if (drivers[i].active)  // только активных водителей
        {
            get_status(drivers[i].pid);
        }
    }
}

// Очистка зомби-процессов (проверка, кто из водителей умер)
void cleanup_zombies()
{
    pid_t pid;
    int status;

    // WNOHANG - не блокировать, -1 - любой дочерний процесс
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int i = 0; i < driver_count; i++)
        {
            if (drivers[i].pid == pid)  // нашли умершего водителя
            {
                drivers[i].active = 0;       // помечаем как неактивного
                close(drivers[i].write_fd);  // закрываем каналы
                close(drivers[i].read_fd);
                printf("Driver %d terminated\n", pid);
                break;
            }
        }
    }
}

int main()
{
    char input[BUFFER_SIZE];
    char cmd[BUFFER_SIZE];

    printf("Taxi Dispatcher CLI\n");
    printf("Commands: create_driver, send_task <pid> <time>, get_status <pid>, get_drivers, exit\n\n");

    while (1)
    {
        printf("\n");
        printf("Enter command >> ");
        fflush(stdout);

        if (!fgets(input, BUFFER_SIZE, stdin)) break;
        printf("\n");

        input[strcspn(input, "\n")] = 0;  // удаляем символ новой строки

        cleanup_zombies();  // проверяем, не умер ли кто из водителей

        // проверка команд и парсинг
        if (strcmp(input, "create_driver") == 0)
        {
            create_driver();
        }
        else if (strcmp(input, "get_drivers") == 0)
        {
            get_drivers();
        }
        else if (strncmp(input, "send_task", 9) == 0)
        {
            pid_t pid;
            int timer;
            if (sscanf(input, "send_task %d %d", &pid, &timer) == 2)
            {
                send_task(pid, timer);
            }
            else
            {
                printf("Usage: send_task <pid> <task_timer>\n");
            }
        }
        else if (strncmp(input, "get_status", 10) == 0)
        {
            pid_t pid;
            if (sscanf(input, "get_status %d", &pid) == 1)
            {
                get_status(pid);
            }
            else
            {
                printf("Usage: get_status <pid>\n");
            }
        }
        else if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
        {
            // завершаем всех водителей
            for (int i = 0; i < driver_count; i++)
            {
                if (drivers[i].active)
                {
                    close(drivers[i].write_fd);
                    kill(drivers[i].pid, SIGTERM);  // посылаем сигнал завершения
                }
            }
            printf("Exiting...\n");
            break;
        }
        else if (strlen(input) > 0)
        {
            printf("Unknown command: %s\n", input);
        }
    }

    return 0;
}