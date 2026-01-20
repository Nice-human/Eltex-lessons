#include <stdio.h>
#include "intrface.h"

// Функция вывода всех записей
void display_all(struct abonents* abs)
{
    if (abs->count == 0)
    {
        printf("Справочник пуст!\n");
        return;
    }

    printf("\nСписок всех абонентов (всего: %d):\n", abs->count);
    printf("-------------------------------------------------\n");
    printf("№  | %-9s    | %-12s      | %-12s      |\n", "Имя", "Фамилия", "Телефон");
    printf("-------------------------------------------------\n");

    struct abonent* current = abs->head;
    int i = 0;

    while (current != NULL)
    {
        printf("%-2d | %-9s | %-12s | %-12s |\n",
            i + 1,
            current->name, current->second_name, current->tel);
        current = current->next;
        i++;
    }

    printf("-------------------------------------------------\n");
}

// Функция вывода всех записей в обратном порядке (демонстрация двунаправленности)
void display_all_reverse(struct abonents* abs)
{
    if (abs->count == 0)
    {
        printf("Справочник пуст!\n");
        return;
    }

    printf("\nСписок всех абонентов в обратном порядке (всего: %d):\n", abs->count);
    printf("-------------------------------------------------\n");
    printf("№  | %-9s | %-12s | %-12s |\n", "Имя", "Фамилия", "Телефон");
    printf("-------------------------------------------------\n");

    struct abonent* current = abs->tail;
    int i = 0;

    while (current != NULL)
    {
        printf("%-2d | %-9s | %-12s | %-12s |\n",
            i + 1,
            current->name, current->second_name, current->tel);
        current = current->prev;
        i++;
    }

    printf("-------------------------------------------------\n");
}