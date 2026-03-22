#ifndef INTRFACE_H
#define INTRFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>

#define S 5
#define N 3

struct Shop
{
    long product;
    int id;
};

struct Shopper
{
    long need;
    int id;
};

extern struct Shop myShop[S];
extern struct Shopper myShopper[N];
extern pthread_mutex_t shop_mutex[S];

void creat_shop();
void creat_shoppers();
void* pthread_shopper(void* arg);
void* pthread_loader(void* arg);
#endif
