#define _POSIX_C_SOURCE 200809L // Обязательно для sigaction
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>  // для memset
#include <errno.h>


int main(void)
{
    sigset_t set;    // маска
    int ret;
    
    sigemptyset(&set);          // сброс маски в 0
    sigaddset(&set, SIGINT);   // добавляем в маску сигнал

    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if (ret < 0)
    {
       perror("Can't set signal handler!\n");
       exit(EXIT_FAILURE);
    }

    printf("Receiver started. PID: %d\n", getpid());  // Показываем PID
    
    while (1)
    {
        sleep(1);
    }
    
    exit(EXIT_SUCCESS);
}