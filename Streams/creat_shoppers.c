#include "intrface.h"

void creat_shoppers()
{
    for (int i = 0; i < N; i++)
    {
        myShopper[i].need = 9100 + rand() % 201;
        myShopper[i].id = i + 1;
        printf("Покупатель %d создан с потребностью %ld\n", myShopper[i].id, myShopper[i].need);
    }
    printf("__________________________________________________\n\n");
}