#include <stdio.h>                                      // для ввода/вывода
#include <stdlib.h>                                     // для exit()
#include <unistd.h>                                     // для pipe(), fork(), read(), write(), close()
#include <string.h>                                     // для strlen()
#include <sys/wait.h>                                   // wait()

int main()                                              
{                                                       
    int pipefd[2];                                     
    pid_t pid;                                         
    char buf;                                           
    char* msg = "Hi";                                   

    // Создаём канал                                    
    if (pipe(pipefd) == -1) 
    {                           
        perror("pipe");                                 
        exit(1);                             
    }                                                   

    // Создание дочернего процесса
    pid = fork();                                       
    if (pid == -1) 
    {                                    
        perror("fork");                                 
        exit(1);                             
    }                                                   

    // Дочерний процесс
    if (pid == 0) 
    {                                     
        close(pipefd[1]);                               // Закрываем канал на запись

        while (read(pipefd[0], &buf, 1) > 0)            // Читаем по одному байту в buf
        {          
            write(STDOUT_FILENO, &buf, 1);              // Выводим каждый прочитанный байт в stdout
        }                                               

        close(pipefd[0]);                               // Закрываем канал на чтение
        exit(0);                             // Успешное завершение дочернего процесса
    }                                                   
    else // Родительский процесс
    {                                              
        close(pipefd[0]);                               // Закрываем ненужный конец для чтения
        write(pipefd[1], msg, strlen(msg));             // Запись строки Hi в канал
        close(pipefd[1]);                               // Закрываем канал на запись

        wait(NULL);                                     // Ждём завершения дочернего процесса 
        printf("\n\n");                                    
    }                                                   

    return 0;                                           
}   