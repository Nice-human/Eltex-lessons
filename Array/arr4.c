#include <stdio.h>

#define N 5

int main() 
{
    int matrix [N][N];
    int leght = 0, top = 0;
    int right = N - 1, bottom = N - 1;
    int value = 1;
    int i;

    //Test
    // for (int i = 0; i < N; i++)
    // {
    //     for (int j = 0; j < N; j++)
    //     {
    //         matrix[j][i] = 0;
    //     } 
    // }

    while (value <= N * N)
    {
            // С лево на право
        for (i = leght; i <= right; i++)
        {
            matrix[top][i] = value++;
        }
        top++;

        // С верху в низ
        for (i = top; i <= bottom; i++)
        {
            matrix[i][right] = value++;
        }
        right--;

        // С право на лево
        for (i = right; i >= leght; i--)
        {
            matrix[bottom][i] = value++;
        }
        bottom--;

        // Снизу вверх
        for (i = bottom; i >= top; i--)
        {
            matrix[i][leght] = value++;
        }
        leght++;
    }
    
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%3d ", matrix[i][j]);
        } 

        printf("\n");
    }
    
    printf("\n\n");

    return 0;
}