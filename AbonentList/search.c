#include <stdio.h>
#include "intrface.h"

int my_strcmp(const char* str1, const char* str2)
{
    if (str1 == NULL || str2 == NULL)
    {
        return 1;
    }

    for (char i = 0; str1[i] != '\0' || str2[i] != '\0'; i++)
    {
        if (str1[i] != str2[i])
        {
            return 1;
        }
    }

    return 0;
}

// Функция поиска абонентов по имени
void search_by_name(struct abonents* abs)
{
    if (abs->count == 0)
    {
        printf("Справочник пуст!\n");
        return;
    }

    char name[MAX_STR];
    printf("Введите имя для поиска: ");
    scanf("%9s", name);
    getchar();

    struct abonent* current = abs->head;
    int found_count = 0;

    printf("\nРезультаты поиска по имени '%s':\n", name);
    printf("-------------------------------------------------\n");
    printf("№   | %-9s    | %-12s      | %-12s      |\n", "Имя", "Фамилия", "Телефон");
    printf("-------------------------------------------------\n");

    int i = 0;

    while (current != NULL)
    {   
        if (my_strcmp(current->name, name) == 0)
        {
            printf("%-2d  | %-9s | %-12s | %-12s |\n", 
                i + 1,
                current->name, current->second_name, current->tel);
            found_count++;
            i++;
        }
        current = current->next;
    }

    printf("-------------------------------------------------\n");

    if (found_count == 0)
    {
        printf("Абоненты с именем '%s' не найдены.\n", name);
    }
    else {
        printf("Найдено абонентов: %d\n", found_count);
    }
}