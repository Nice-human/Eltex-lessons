#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    pid_t pid;
    int signum = SIGINT;  // сигнал по умолчанию
    
    // Если 3 аргумента, то первый - номер сигнала, второй - PID
    if (argc == 3)
    {
        signum = atoi(argv[1]);
        pid = atoi(argv[2]);
    }
    // Если 2 аргумента, то это PID
    else if (argc == 2)
    {
        pid = atoi(argv[1]);
    }
    else
    {
        perror("Error arg[2]");
        exit(EXIT_FAILURE);
    }
    
    if (pid <= 0)
    {
        perror("Error pid");
        exit(EXIT_FAILURE);
    }
    
    if (kill(pid, signum) == -1)
    {
        perror("kill failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Signal %d sent to PID %d\n", signum, pid);
    return 0;
}