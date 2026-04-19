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
    int sig_num;
    
    sigemptyset(&set);          // сброс маски в 0
    sigaddset(&set, SIGUSR1);   // добавляем в маску сигнал

    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if (ret < 0)
    {
       perror("Can't set signal handler!\n");
       exit(EXIT_FAILURE);
    }

    printf("Receiver started. PID: %d\n", getpid());  // Показываем PID
    
    while (1)
    {
        sigwait(&set, &sig_num);
        printf("Sig number %d\n", sig_num);
    }
    
    exit(EXIT_SUCCESS);
}