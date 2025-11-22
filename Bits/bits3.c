#include <stdio.h>

// Task 3

int main (void)
{
    int number = 0;
    int bits;
    int count = 0;

    printf("Введите целое положительное число: ");
    scanf("%d", &number);
    printf("Вы ввели: %d\n", number);

    bits = sizeof(number) * 8;
    printf("\n");

    for (int i = bits - 1; i >= 0; i--)
    {
        printf("%d", (number >> i) & 1);

        if (i % 4 == 0 && i != 0)
        {
            printf(" ");
        }   
    }

    printf("\n\n");
    
    printf("Найдено единиц: ");
    
    if (number > 0)
    {
        for (int i = bits - 1; i >= 0; i--)
        {
            if ((number >> i) & 1 == 1)
            {
                count++;
            }   
        }

        printf("%d", count);
    }
    else printf("Error - Введите целое положительное число");   

    printf("\n\n");
    
    return 0;
}