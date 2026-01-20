#include <stdio.h>
#include <malloc.h>
#include <locale.h>
#include "intrface.h"


int main()
{
    // Руссификатор
    setlocale(LC_ALL, "Russian");

    struct abonents abs;
    abs.head = NULL;
    abs.tail = NULL;
    abs.count = 0;

    int choice;

    do
    {
        display_menu();
        scanf("%d", &choice);
        getchar(); // Очистка буфера после scanf

        switch (choice)
        {
        case 1:
            add_abonent(&abs);
            break;
        case 2:
            delete_abonent(&abs);
            break;
        case 3:
            search_by_name(&abs);
            break;
        case 4:
            display_all(&abs);
            break;
        case 5:
            display_all_reverse(&abs);
            break;
        case 6:
            printf("Выход из программы.\n");
            clear_directory(&abs);
            break;
        default:
            printf("Неверный выбор! Пожалуйста, выберите от 1 до 6.\n");
        }

    } while (choice != 6);

    return 0;
}