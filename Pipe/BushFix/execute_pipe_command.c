#include "intrface.h"

// Команда конвейера
void execute_pipe_command(char* input, char* pipe_pos)
{
    int pipefd[2];
    int status1, status2;

    *pipe_pos = '\0';  // Заменяем '|' на '\0'

    char *cmd1 = trim(input);
    char *cmd2 = trim(pipe_pos + 1);
                
    char *args1[MAX_ARGS];
    char *args2[MAX_ARGS];
  
    parse_arguments(cmd1, args1);            
    parse_arguments(cmd2, args2);
    
    // Создаем канал (pipe) для связи между процессами
    if (pipe(pipefd) == -1) 
    {
        printf("Ошибка при создании канала\n");
        return;
    }
    
    // Создаем первый процесс (для первой команды)
    pid_t pid1 = fork();
    
    if (pid1 == 0) 
    {             
        close(pipefd[0]);  // закрываем чтение из канала
        dup2(pipefd[1], STDOUT_FILENO);  // Перенаправляем вывод в канал (вместо экрана)
        close(pipefd[1]);  // закрываем запись в канал
        
        // Выполняем первую команду
        if (execvp(args1[0], args1) == -1) 
        {
            printf("Ошибка: программа '%s' не найдена\n", args1[0]);
            exit(1);
        }
    } 
    else if (pid1 < 0) 
    {
        printf("Ошибка при создании первого процесса\n");
        return;
    }
    
    // Создаем второй процесс (для второй команды)
    pid_t pid2 = fork();            
    if (pid2 == 0) 
    {        
        close(pipefd[1]);  // закрываем запись в канал
        dup2(pipefd[0], STDIN_FILENO);  // Перенаправляем ввод из канала
        close(pipefd[0]);  // закрываем чтение из канала
        
        // Выполняем вторую команду
        if (execvp(args2[0], args2) == -1) 
        {
            printf("Ошибка: программа '%s' не найдена\n", args2[0]);
            exit(1);
        }
    } 
    else if (pid2 < 0) 
    {
        printf("Ошибка при создании второго процесса\n");
        return;
    }
    
    // Родительский процесс закрывает оба конца канала
    close(pipefd[0]);
    close(pipefd[1]);
    
    // Ожидаем завершения первого и второго дочернего процесса
    waitpid(pid1, &status1, 0);           
    waitpid(pid2, &status2, 0);
}