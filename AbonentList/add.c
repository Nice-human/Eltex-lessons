#include <stdio.h>
#include <malloc.h>
#include "intrface.h"

// Собственная реализация strncpy
void my_strncpy(char* New_abonent, const char* parametrs, int size)
{
    int i;
    for (i = 0; i < size - 1 && parametrs[i] != '\0'; i++)
    {
        New_abonent[i] = parametrs[i];
    }
    New_abonent[i] = '\0';
}


// Функция создания нового абонента
struct abonent* create_abonent(const char* name, const char* second_name, const char* tel)
{
    struct abonent* new_abonent = (struct abonent*)malloc(sizeof(struct abonent));

    if (new_abonent == NULL)
    {
        printf("Ошибка выделения памяти!\n");
        return NULL;
    }

    my_strncpy(new_abonent->name, name, MAX_STR);
    my_strncpy(new_abonent->second_name, second_name, MAX_STR);
    my_strncpy(new_abonent->tel, tel, MAX_NUMBER);

    new_abonent->next = NULL;
    new_abonent->prev = NULL;

    return new_abonent;
}

// Функция добавления абонента
void add_abonent(struct abonents *abs)
{
    char name[MAX_STR];
    char second_name[MAX_STR];
    char tel[MAX_NUMBER];

    printf("Введите имя (макс 9 символов): ");
    scanf("%9s", name);
    getchar(); // Очистка буфера ввода

    printf("Введите фамилию (макс 9 символов): ");
    scanf("%9s", second_name);
    getchar();

    printf("Введите телефон (макс 11 символов): ");
    scanf("%11s", tel);
    getchar();

    struct abonent* new_abonent = create_abonent(name, second_name, tel);
    if (new_abonent == NULL) return;
    
    // Добавляем в конец двухсвязного списка
    if (abs->head == NULL)
    {
        abs->head = new_abonent;
        abs->tail = new_abonent;
    }
    else
    {
        abs->tail->next = new_abonent;
        new_abonent->prev = abs->tail;
        abs->tail = new_abonent;
    }

    abs->count++;
    printf("\n-------------------------------------------------\n");
    printf("Абонент успешно добавлен! Всего абонентов: %d\n", abs->count);
    printf("-------------------------------------------------\n");
}