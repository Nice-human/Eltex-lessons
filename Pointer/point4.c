#include <stdio.h>

int main() 
{
    char str[100];
    char substr[100];
    char* ptr = NULL;
    
    printf("Введите строку: ");
    fgets(str, sizeof(str), stdin);
    printf("\n");

    printf("Введите подстроку: ");
    fgets(substr, sizeof(substr), stdin);

    for (char i = 0; str[i] != '\0'; i++)
    {  
        int found = 1;

        for (char j = 0; substr[j] != '\0' && substr[j] != '\n'; j++)
        {
            if ( str[i + j] == '\0'|| str[i + j] != substr[j])
            {
                found = 0;
                break;
            } 
        }

        if (found == 1)
        {
            ptr = &str[i];
            break;
        }  
    }  
    
    printf("\n");

    if (ptr)
    {
        printf("Найдены совпадения: %s", ptr);
        printf("Начинается с символа: '%c'\n", *ptr);
        printf("Позиция в строке: %ld", ptr - str); // Высчитывает разницу между 
    }
    else
    {
        printf("Подстрока не найдена. Указатель: NULL\n");
    }
    
    printf("\n\n");

    return 0;
}


