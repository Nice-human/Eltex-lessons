#include <stdio.h>
#include <math.h>

void add(int num1, int num2)
{
    printf("Введите 1 значение: ");
    scanf("%d", &num1);
    printf("Введите 2 значение: ");
    scanf("%d", &num2);

    printf("\n---------------------------\n");
    printf("Сложение двух чисел = %d\n", num1 + num2);
    printf("---------------------------\n\n");
}