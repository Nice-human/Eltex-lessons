#include "intrface.h"

void* pthread_shopper(void* arg)
{
    struct Shopper* shopper = (struct Shopper*)arg;
    
    while(shopper->need > 0)
    {
        // Проходим по всем магазинам по порядку
        for (int shop_index = 0; shop_index < S; shop_index++)
        {
            // Пытаемся захватить мьютекс магазина
            if (pthread_mutex_trylock(&shop_mutex[shop_index]) == 0)
            {
                // Проверяем, есть ли товар в магазине
                if (myShop[shop_index].product > 0)
                {
                    printf("Покупатель %d зашел в магазин %d (товаров: %ld, потребность: %ld)\n", 
                           shopper->id, myShop[shop_index].id, myShop[shop_index].product, shopper->need);
                    
                    // Скупаем весь товар
                    shopper->need -= myShop[shop_index].product;
                    myShop[shop_index].product = 0;
                    
                    printf("Покупатель %d вышел из магазина %d (остаток потребности: %ld)\n\n", 
                           shopper->id, myShop[shop_index].id, shopper->need);
                }
                else
                {
                    printf("Покупатель %d зашел в пустой магазин %d и вышел\n\n", 
                           shopper->id, myShop[shop_index].id);
                }
                
                pthread_mutex_unlock(&shop_mutex[shop_index]);
                
                // Если потребность удовлетворена, выходим из цикла
                if (shopper->need <= 0)
                {
                    break;
                }
            }
            else
            {
                // Магазин занят, пробуем следующий
                printf("Покупатель %d: магазин %d занят, иду дальше\n\n", 
                       shopper->id, myShop[shop_index].id);
                usleep(100000); // небольшая задержка 0.1 сек
            }
        }
        
        // Если после обхода всех магазинов потребность не удовлетворена, засыпаем
        if (shopper->need > 0)
        {
            printf("Покупатель %d обошел все магазины, но не насытился\n\n", shopper->id);
            sleep(2);
        }
    }
    
    printf("--------------------------------------------------------\n");
	printf("Клиент %d насытелся и завершил работу!\n", shopper->id);
	printf("--------------------------------------------------------\n\n");
	return NULL;
}