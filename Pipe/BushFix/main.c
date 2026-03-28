#include "intrface.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

int main() 
{
    setlocale(LC_ALL, "Russian");

    char input[MAX_INPUT_SIZE];
    
    while (1) 
    {
        printf("myshell> ");
        fflush(stdout);
        
        // Считываем строку введенную с клавиатуры
        if (fgets(input, sizeof(input), stdin) == NULL) 
        {
            printf("\n");
            break;
        }
        
        // Убираем символ новой строки
        remove_newline(input);
        
        // Проверка на пустую строку
        if (strlen(input) == 0) continue;
        
        // Проверка на выход
        if (strcmp(input, "exit") == 0) 
        {
            printf("Выход из интерпретатора\n");
            break;
        }
        
        // Ищем в строке символ '|'
        char *pipe_pos = strchr(input, '|');
        
        // Если нет '|', выполняем как обычно
        if (pipe_pos == NULL) 
        {
            execute_simple_command(input);
        } 
        else // Если есть в строке '|' разделяем строку на две команды
        {          
            execute_pipe_command(input, pipe_pos);
        }
    }
    
    return 0;
}