#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("execve realizado\n");

    bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Recibido por slave con pid %d: %s\n", getpid(), buffer);
    } else if (bytes_read == -1) {
        perror("Error al leer de stdin");
        exit(EXIT_FAILURE);
    }    
    return 0;
}