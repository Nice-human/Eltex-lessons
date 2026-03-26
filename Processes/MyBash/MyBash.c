#include <stdio.h>      // НУЖНА: для printf, fgets, fflush
#include <unistd.h>     // НУЖНА: для fork, execvp
#include <stdlib.h>     // НУЖНА: для exit, malloc (косвенно), но фактически используется только exit
#include <sys/types.h>  // НУЖНА: для pid_t (тип идентификатора процесса)
#include <sys/wait.h>   // НУЖНА: для wait, WIFEXITED, WEXITSTATUS
#include <string.h>     // НУЖНА: для strcspn, strlen, strcmp, strtok

#define MAX_INPUT_SIZE 1024   // максимальная длина вводимой строки
#define MAX_ARGS 64           // максимальное количество аргументов

int main()
{
    char input[MAX_INPUT_SIZE];  // буфер для ввода пользователя
    char *args[MAX_ARGS];        // массив для хранения аргументов команды
    pid_t pid;                   // идентификатор процесса
    int status;                  // статус завершения дочернего процесса
    
    while(1)  
    {
        // Выводим приглашение командной строки
        printf("myshell> ");
        fflush(stdout);  // принудительно выводим приглашение
        
        // Считываем введенную строку
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\n");
            break;
        }
        
        // Убираем символ новой строки (\n) в конце ввода
        input[strcspn(input, "\n")] = 0;
        
        // Проверяем, не ввели ли пустую строку
        if (strlen(input) == 0)
        {
            continue;  
        }
        
        // Проверяем команду выхода
        if (strcmp(input, "exit") == 0)
        {
            printf("Выход из интерпретатора\n");
            break;  
        }
        
        // Разбиваем введенную строку на аргументы
        int arg_count = 0;
        char *token = strtok(input, " ");  // первый аргумент - имя программы
        
        while (token != NULL && arg_count < MAX_ARGS - 1)
        {
            args[arg_count] = token;  // сохраняем аргумент
            arg_count++;
            token = strtok(NULL, " ");  // следующий аргумент
        }
        args[arg_count] = NULL;  // последний элемент должен быть NULL для execvp
        
        // Создаем новый процесс
        pid = fork();
        
        if (pid == 0)  // Дочерний процесс
        {
            // В дочернем процессе выполняем введенную программу
            // execvp принимает имя программы и массив аргументов
            if (execvp(args[0], args) == -1)
            {
                printf("Ошибка: программа '%s' не найдена или не может быть выполнена\n", args[0]);
                exit(1);  
            }
        }
        else if (pid > 0)  // Родительский процесс
        {
            // Ждет завершения дочернего процесса
            wait(&status);
            
            // Выводим информацию о завершении
            if (WIFEXITED(status))
            {
                printf("Дочерний процесс завершился с кодом: %d\n", WEXITSTATUS(status)); // Код завершения
            }
        }
        else  // Ошибка при создании процесса
        {
            printf("Ошибка при создании процесса\n");
        }
    }
    
    return 0;
}