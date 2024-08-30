#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define     SLAVE_QTY       5
#define     PERCENTAJE_INITIAL  0.1

int main(int argc, char *argv[]) {
    int pid;
    int total_files = argc - 1;
    printf("total files: %d\n", total_files);

    int initial_files_qty = total_files * PERCENTAJE_INITIAL; 
    printf("initial files: %d\n", initial_files_qty);

    int files_per_slave = initial_files_qty / SLAVE_QTY > 0 ? initial_files_qty / SLAVE_QTY : 1;
    printf("files per slave: %d\n", files_per_slave);

    int remaining_files = total_files - files_per_slave * SLAVE_QTY;
    printf("remaining files: %d \n", remaining_files);

    int current_index = 1; // indice del archivo a procesar

    for(int i = 0; i < SLAVE_QTY; i++){
        pid = fork();

        if(pid < 0) {
            perror("Error al crear el proceso");
            return 1;
        } else if (pid == 0) {
            // Este es el proceso hijo, se ejecuta el codigo del hijo
            int start_index = i * files_per_slave + 1;
            int end_index = start_index + files_per_slave;

            //char *args[] = {"./slave", argv[i], NULL };
            char *args[files_per_slave + 2];
            args[0] = "./slave";
            for (int j = start_index, k = 1; j < end_index; j++, k++) {
                args[k] = argv[j];
            }
            args[files_per_slave + 1] = NULL;

            execve(args[0], args, NULL);
            perror("Error al ejecutar el proceso hijo"); // si execve falla
            exit(1); // Salir si execve falla
        }
    }

    for (int i = 0; i < SLAVE_QTY; i++) {
        wait(NULL);
    }

    return 0;
}