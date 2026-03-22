#include "intrface.h"

void creat_shop()
{
    for (int i = 0; i < S; i++)
    {
        myShop[i].product = 900 + rand() % 201;
        myShop[i].id = i + 1;
        pthread_mutex_init(&shop_mutex[i], NULL);
        printf("Магазин %d создан с %ld товарами\n", myShop[i].id, myShop[i].product);
    }
    printf("\n");
}