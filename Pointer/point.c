#include <stdio.h>

int main (void) 
{
    // Число для теста 270533154 = 0x34023216

    int number;
    int new_byte;
    char* ptr;

    ptr = (char*)&number;

    printf("Введите число: ");
    scanf("%d", &number);

    if (number < 0)
    {
        printf("Введите положительное число! \n\n");
        return 1;
    }
    
    printf("Вывод четырех байтов: ");
    for (char i = 0; i <= 3; i++)
    {
        printf("%d ", *ptr);
        ptr++;
    }
    ptr = (char*)&number; // Возвращаемся на начало

    printf("\n\n");

    printf("Введите число третьего байта: ");
    scanf("%d", &new_byte);
    printf("\n\n");

    ptr ++; // Переходим к третьему байту
    *ptr = new_byte; // Меняем значение третьего байта
    ptr = (char*)&number; // Возвращаемся на начало

    printf("Итоговое число: %d\n", number);
    printf("Итоговый вывод четырех байтов: ");
    
    for (char i = 0; i <= 3; i++)
    {
        printf("%d ", *ptr);
        ptr++;
    }

    printf("\n\n");

    return 0;
}

// Вариант 2

// #include <stdio.h>

// int main (void)
// {
//     // Число для теста 270533154 = 0x34023216

//     int number;
//     int new_byte;
//     char* ptr;

//     ptr = (char*)&number;

//     printf("Введите число: ");
//     scanf("%d", &number);

//     if (number < 0)
//     {
//         return 1;
//     }

//     printf("Вывод четырех байтов: ");  
//     for (char i = 0; i < sizeof(int); i++)
//     {
//         printf("%d", ptr[i]);
//     }

//     printf("\n\n");

//     printf("Введите число третьего байта: ");
//     scanf("%d", &new_byte);
//     printf("\n\n");

//     //Простой вариант ptr[1] = new_byte;
//     //Продвинутый вариант ptr[sizeof(int)-3] = new_byte;

//     printf("Итоговое число: %d\n", number);
//     printf("Итоговый вывод четырех байтов: ");  
//     for (char i = 0; i < sizeof(int); i++)
//     {
//         printf("%d", ptr[i]);
//     }

//     printf("\n\n");

//     return 0;
// }