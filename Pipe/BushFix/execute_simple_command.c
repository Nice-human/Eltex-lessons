#include "intrface.h"

// Простая команда
void execute_simple_command(char* input)
{
    char *args[MAX_ARGS];
    pid_t pid;
    int status;

    parse_arguments(input, args);
            
    // Создаем процесс
    pid = fork();
    
    // Дочерний процесс
    if (pid == 0) 
    {          
        if (execvp(args[0], args) == -1) 
        {
            printf("Ошибка: программа '%s' не найдена\n", args[0]);
            exit(1);
        }
    } 
    else if (pid > 0) // Родительский процесс
    {       
        wait(&status);// ждет завершения дочернего процесса
    } 
    else 
    {
        printf("Ошибка при создании процесса\n");
    }
}