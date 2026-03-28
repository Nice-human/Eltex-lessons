#include "intrface.h"

// Убираем лишние пробелы в начале и конце команды
char* trim (char* str)
{   
    while (*str == ' ') str++;  // пропускаем пробелы в начале cmd1
    char *end = str + strlen(str) - 1;
    while (end > str && *end == ' ') end--; // пропускаем пробелы в конце cmd1
    *(end + 1) = '\0'; // ставим 0 в конце строки

    return str;
}