#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARG 64
#define MAX_INPUT 1024

int main()
{
    char input[MAX_INPUT];
    char* arg[MAX_ARG];
    pid_t pid;
    int status;

    while (1)
    {
        printf("myshell-> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) // Вводим новую строку и проверяем на ошбку
        {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0; // Убираем переход на новую строку
        
        if (strlen(input) == 0) // Проверка на пустую строку
        {
            continue;
        }

        if (strcmp(input, "exit") == 0) // Сравниваем вводимую строку с exit
        {
            printf("Exit from the interpreter\n\n");
            break;
        }
 
        int count_args = 0;
        char* tokken = strtok(input, " "); // Разделяем строку и заменяем пробелы на '\0'
          
        while (tokken != NULL && count_args < MAX_ARG -1)
        {
            arg[count_args] = tokken;
            count_args++;
            tokken = strtok(NULL, " "); // Начинаем с от туда где закончили
        }

        arg[count_args] = NULL; // Присваевам NULL для execvp

        printf("\n");
        pid = fork(); // Вход в дочерний процесс

        if (pid == 0)
        {
            if (execvp(arg[0], arg) == -1) // Запуск программы и проверка на ошибку
            {
                printf("Error args\n");   
                exit(1); // Выход из дочернего процесса и говорит об ошибке
            }   
        }
        else if(pid > 0)
        {
            wait(&status); // Завершенный процесс со статусом

            if (WIFEXITED(status)) // Проверка что процесс завершился через exit или return
            {
                if (WEXITSTATUS(status) == 0)
                {
                    printf("The process is complete, status = %d\n\n", WEXITSTATUS(status)); // Успех
                }           
                else
                {
                    printf("Program failed, exit code = %d\n\n", WEXITSTATUS(status)); // Ошибка
                } 
            }              
        }
        else
        {
            printf("Fork failed\n"); // Ошибка fork
        }
    }   
}