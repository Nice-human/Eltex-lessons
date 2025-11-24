#include <stdio.h>

#define N 3

int main (void)
{
    int matrx[N][N] = {
    {1,2,3},
    {4,5,6},
    {7,8,9}
    };

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d ", matrx[i][j]);
        }

            printf("\n");   
    }
    
    printf("\n");   
    
    return 0;
}