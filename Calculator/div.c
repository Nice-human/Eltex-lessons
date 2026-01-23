#include <stdio.h>
#include <math.h>

void div(int num1, int num2)
{
    printf("Введите 1 значение: ");
    scanf("%d", &num1);
    printf("Введите 2 значение: ");
    scanf("%d", &num2);

    if (num2 == 0)
    {
        printf("\n---------------------------\n");
        printf("Error - на 0 делить нельзя! \n");
        printf("---------------------------\n\n");
        return;
    }
    else
    {
        printf("\n---------------------------\n");
        printf("Сложение двух чисел = %d\n", num1 / num2);
        printf("---------------------------\n\n");
    }  
}