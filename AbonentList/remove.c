#include <stdio.h>
#include <malloc.h>
#include "intrface.h"

// Функция удаления абонента
void delete_abonent(struct abonents* abs)
{
    if (abs->count == 0)
    {
        printf("Справочник пуст!\n");
        return;
    }

    char name[MAX_STR];
    char second_name[MAX_STR];

    printf("Введите имя абонента для удаления: ");
    scanf("%9s", name);
    getchar();

    printf("Введите фамилию абонента для удаления: ");
    scanf("%9s", second_name);
    getchar();

    struct abonent* current = abs->head;
    int found = 0;

    // Поиск абонента
    while (current != NULL)
    {
        if (my_strcmp(current->name, name) == 0 &&
            my_strcmp(current->second_name, second_name) == 0)
        {
            found = 1;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("Абонент не найден!\n");
        return;
    }

    // Удаляем из двухсвязного списка
    if (current->prev != NULL)
    {
        current->prev->next = current->next;
    }
    else
    {
        // Удаляем первый элемент
        abs->head = current->next;
    }

    if (current->next != NULL)
    {
        current->next->prev = current->prev;
    }
    else
    {
        // Удаляем последний элемент
        abs->tail = current->prev;
    }

    printf("\n-------------------------------------------------\n");
    printf("Абонент %s %s успешно удален!\n", current->name, current->second_name);
    printf("-------------------------------------------------\n");
    free(current);
    abs->count--;
}