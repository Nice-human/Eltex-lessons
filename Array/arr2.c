#include <stdio.h>

#define N 5

int main (void)
{
    int matrx[N] = {1,2,3,4,5};
    int count = 4;
    int temp;

    for (int i = 0; i < N; i++)
    {
        if (i < 2)
        {
            temp = matrx[i];  
            matrx[i] = matrx[count - i];
            matrx[count - i] = temp;
        }
        
        printf("%d ", matrx[i]);  
    }
    
    printf("\n\n");
    
    return 0;
}