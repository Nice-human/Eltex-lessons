#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

#define DEFAULT_FIFO "/tmp/myfifo"

int main() 
{  
    // Создаем FIFO
    unlink(DEFAULT_FIFO);
    if (mkfifo(DEFAULT_FIFO, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    
    printf("Server waiting for client...\n");
    
    int fd = open(DEFAULT_FIFO, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    char *msg = "Hi!";
    write(fd, msg, strlen(msg));
    
    close(fd);
    printf("Server done.\n");
    
    return 0;
}