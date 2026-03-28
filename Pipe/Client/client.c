#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define DEFAULT_FIFO "/tmp/myfifo"
#define BUFFER_SIZE 10

int main() 
{
    char buffer[BUFFER_SIZE];

    int fd = open(DEFAULT_FIFO, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    buffer[bytes] = '\0';
    
    printf("Received: %s\n", buffer);
    
    close(fd);
    unlink(DEFAULT_FIFO);
    
    return 0;
}