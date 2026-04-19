#define _POSIX_C_SOURCE 200809L // Обязательно для sigaction
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>  // для memset
#include <errno.h>

void sig_handler(int sig_num, siginfo_t* info, void* args)
{
    printf("Signal SIGUSR1! - %d %d\n", sig_num, info->si_signo);
    fflush(stdout);  // Принудительный вывод на экран
}

int main(void)
{
    struct sigaction handler;
    sigset_t set;               // маска
    int ret;

    // обнуляем структуру перед использованием
    memset(&handler, 0, sizeof(handler));
    
    sigemptyset(&set);          // сброс маски в 0
    sigaddset(&set, SIGUSR1);   // добавляем в маску сигнал

    handler.sa_sigaction = sig_handler;
    handler.sa_mask = set;
    handler.sa_flags = SA_SIGINFO;  // устанавливаем флаг

    ret = sigaction(SIGUSR1, &handler, NULL);
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