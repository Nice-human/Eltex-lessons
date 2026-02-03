#include <stdio.h>
#include <math.h>
#include <locale.h>
#include "intrface.h"

int main()
{
    setlocale(LC_ALL, "Russian");
    int num1 = 0, num2 = 0;
    int choice = 0;
    
    printf("Добро пожаловать в « Калькулятор - Макаренко » \n\n");

    do
    {
        printf("Выбор операции: \n");
        printf("1) Сложение \n");
        printf("2) Вычитание \n");
        printf("3) Умножение \n");
        printf("4) Деление \n");
        printf("5) Выход \n\n");

        printf("Выбор операция: ");
        scanf("%d", &choice);

        if (choice < 1 || choice > 5)
        {
            printf("\n--------------------------------\n");
            printf("Error - введите число от 1 до 5! \n");
            printf("--------------------------------\n\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            add(num1,num2);
            break;
        case 2:
            sub(num1, num2);
            break;
        case 3:
            mul(num1, num2);
            break;
        case 4:
                div(num1, num2);
            break;
        case 5:
            printf("\n-------------------\n");
            printf("Выход из программы!\n");
            printf("-------------------\n\n");
            return 1;
        default:
            printf("\n--------------------------------\n");
            printf("Error - введите число от 1 до 5! \n");
            printf("--------------------------------\n\n");
        }

    } while (choice != 5);
    
    return 0;
}