#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t Proc1_pid, Proc2_pid, Proc3_pid, Proc4_pid, Proc5_pid;
    int status1, status2, status3, status4, status5;

    Proc1_pid = fork();

    if (Proc1_pid == 0)
    {
        Proc3_pid = fork();
        
        if (Proc3_pid == 0)
        {
            sleep(1);
            printf("Proc3_pid: PID = %d; PPID = %d\n", getpid(), getppid());
            exit(3);
        }

        Proc4_pid = fork();

        if(Proc4_pid == 0)
        {
            sleep(1);
            printf("Proc4_pid: PID = %d; PPID = %d\n", getpid(), getppid());
            exit(4);
        }

        wait(&status3);
        wait(&status4);

        printf("Proc1_pid: PID = %d; PPID = %d\n", getpid(), getppid());     
        printf("Status = %d\n", WEXITSTATUS(status3)); 
        printf("Status = %d\n", WEXITSTATUS(status4));
        printf("\n");
        exit(1);  
    }

    Proc2_pid = fork();

    if(Proc2_pid == 0)
    {
        Proc5_pid = fork();

        if (Proc5_pid == 0)
        {
            sleep(1);
            printf("Proc5_pid: PID = %d; PPID = %d\n", getpid(), getppid());
            exit(5);
        }

        wait(&status5);

        printf("Proc2_pid: PID = %d; PPID = %d\n", getpid(), getppid()); 
        printf("Status = %d\n", WEXITSTATUS(status5));
        printf("\n");

        exit(2); 
    }


    wait(&status1);
    wait(&status2);

    printf("Parent: PID = %d; PPID = %d\n", getpid(), getppid());      
    printf("Status = %d\n", WEXITSTATUS(status1));
    printf("Status = %d\n", WEXITSTATUS(status2));
    printf("\n");
    
    return 0;   
}