#include <stdio.h>

int main (void) 
{
    int arr[10] = {1,2,3,4,5,6,7,8,9,10};
    int* ptr;

    ptr = (int*)&arr;
   
    printf("Вывод элементов массива: ");

    for (int i = 0; i < sizeof(arr)/sizeof(int); i++)
    {
        printf("%d ", *(ptr + i));
    }

    printf("\n\n");
    
    return 0;
}

// #include <stdio.h>

// int main() 
// {
//     int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
//     printf("Элементы массива: ");
    
//     for (int *p = arr; p < arr + 10; p++) 
//     {
//         printf("%d ", *p);
//     }
    
//     printf("\n\n");
//     return 0;
// }