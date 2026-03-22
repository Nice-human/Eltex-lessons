#include "intrface.h"

void* pthread_loader(void* arg)
{
    while(1)
    {
        // Спим 1 секунду перед началом работы
        sleep(1);
        
        // Проходим по всем магазинам по порядку
        for (int shop_index = 0; shop_index < S; shop_index++)
        {
            // Пытаемся захватить мьютекс магазина
            if (pthread_mutex_trylock(&shop_mutex[shop_index]) == 0)
            {
                printf("Погрузчик зашел в магазин %d (было товаров: %ld)\n", 
                       myShop[shop_index].id, myShop[shop_index].product);
                
                myShop[shop_index].product += 200;
                
                printf("Погрузчик вышел из магазина %d и добавил 200 товаров (стало: %ld)\n\n", 
                       myShop[shop_index].id, myShop[shop_index].product);
                
                pthread_mutex_unlock(&shop_mutex[shop_index]);
            }
            else
            {
                // Если магазин занят, пробуем следующий
                printf("Погрузчик: магазин %d занят, иду дальше\n", myShop[shop_index].id);
                usleep(100000); // небольшая задержка 0.1 сек
            }
        }
        // После обхода всех магазинов погрузчик продолжает цикл
    }
    
    return NULL;
}