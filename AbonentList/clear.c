#include <stdio.h>
#include <malloc.h>
#include "intrface.h"

// Функция очистки всей памяти
void clear_directory(struct abonents* abs)
{
    struct abonent* current = abs->head;
    struct abonent* next = NULL;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    abs->head = NULL;
    abs->tail = NULL;
    abs->count = 0;
    printf("Память очищена.\n");
}