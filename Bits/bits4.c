#include <stdio.h>

// Task 4

int main (void)
{
    int number;
    unsigned int new_byte;

    printf("Введите целое положительное число: ");
    scanf("%d", &number);

    if (number < 0)
    {
        printf("Введите целое положительное число: ");
        return 1;
    }    

    printf("Введите новое значение для третьего байта (0-255): ");
    scanf("%d", &new_byte);

    printf("\n");
    
    printf("Исходное число: %d\n", number);
    printf("Исходное число в шеснадцатеричном виде: 0x%08X\n", number);
    printf("\n");

    int mask = ~(0xFF << 16);

    number = (number & mask) | (new_byte << 16);

    printf("Результат: %d\n", number);
    printf("Результат в шеснадцатеричном виде: 0x%08X\n", number);

    printf("\n\n");

    return 0;
}