#include "intrface.h"

struct Shop myShop[S];
struct Shopper myShopper[N];
pthread_mutex_t shop_mutex[S];

int main()
{
    srand(time(NULL));
    setlocale(LC_ALL, "Russian");
    
    printf("=== Программа моделирования работы магазинов ===\n\n");
    
    creat_shop();
    creat_shoppers();
    
    pthread_t shoppers[N];
    pthread_t loader;
    
    // Создаем потоки покупателей
    for (int i = 0; i < N; i++)
    {
        pthread_create(&shoppers[i], NULL, pthread_shopper, &myShopper[i]);
    }
    
    // Создаем поток погрузчика
    pthread_create(&loader, NULL, pthread_loader, NULL);
    
    // Ожидаем завершения всех покупателей
    for (int i = 0; i < N; i++)
    {
        pthread_join(shoppers[i], NULL);
    }
    
    // Завершаем поток погрузчика (принудительно, так как он в бесконечном цикле)
    pthread_cancel(loader);
    pthread_join(loader, NULL);
    
    printf("\n=== Все покупатели насытились! Программа завершена. ===\n\n");
    
    // Очищаем мьютексы
    for (int i = 0; i < S; i++)
    {
        pthread_mutex_destroy(&shop_mutex[i]);
    }
    
    return 0;
}