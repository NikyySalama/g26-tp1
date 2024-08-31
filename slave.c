
//slave
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    /*bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        //printf("Recibido por slave con pid %d: %s\n", getpid(), buffer);
    } else if (bytes_read == -1) {
        perror("Error al leer de stdin");
        exit(EXIT_FAILURE);
    }    */
    while (read(STDIN_FILENO, buffer, sizeof(buffer)) )
    {
        // procesa los archivos que se mandan por STDIN
        // termina de procesarlos y manda por STDOUT una senal de que termino
        printf("DONE\n"); //senal
    }
    
    return 0;
}