#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define     SLAVE_QTY       5

//prueba brunch

int main(int argc, char *argv[]) {
    int pid;
    for(int i = 1; i <= SLAVE_QTY; i++){
        pid = fork();

        if(pid < 0) {
            perror("Error al crear el proceso");
            return 1;
        } else if (pid == 0) {
            // Este es el proceso hijo, se ejecuta el codigo del hijo
            char *args[] = {"./slave", argv[i], NULL };
            execve(args[0], args, NULL);
            perror("Error al ejecutar el proceso hijo"); // si execve falla
            exit(1); // Salir si execve falla
        }
    }
    return 0;
}