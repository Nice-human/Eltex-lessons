#include <stdio.h>

#define N 3

int main (void)
{
    int matrx[N][N];

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (i == 0 & j != 2 || i == 1 & j == 0)
            {
                matrx[j][0] = 0;
            }
            else  matrx[j][i] = 1;
            
            
            printf("%d ", matrx[j][i]);
        }
         
        printf("\n"); 
    }
    
    printf("\n\n");   
    
    return 0;
}