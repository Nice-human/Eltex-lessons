#include "intrface.h"

// Разбираем первую команду на аргументы
void parse_arguments(char *str, char **args)
{
    int arg_count = 0;
    char *token = strtok(str, " "); // ищем пробелы и заменяем на '\0'
    
    while (token != NULL && arg_count < MAX_ARGS - 1) 
    {
        args[arg_count] = token; // записываем в args строчку без '\0'
        arg_count++;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL; // для execvp
}