#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t Child_pid;
    int status;

    printf("Hello\n");
    fflush(NULL);

    Child_pid = fork();

    if (Child_pid == 0)
    {
        printf("Child_pid = %d; Child_ppid = %d\n", getpid(), getppid());
        exit(5);
    }
    else
    {
        printf("Parent_pid = %d; Parent_pid = %d\n", getpid(), getppid());
        wait(&status);
        printf("Status = %d\n", WEXITSTATUS(status));
    }

    printf("\n");

    return 0;   
}