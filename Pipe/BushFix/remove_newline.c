#include "intrface.h"

// Функция для удаления символа новой строки
void remove_newline(char *str) 
{
    str[strcspn(str, "\n")] = 0;
}