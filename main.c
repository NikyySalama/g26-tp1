#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <string.h>

#define     SLAVE_QTY           5
#define     PERCENTAJE_INITIAL  0.1
#define     W_END               1
#define     R_END               0

void sendFile (int pipe_fd, char *arg);
void sendFiles(int pipe_fd, char *arg[], int qty);
void setup_pipes(int pipes[SLAVE_QTY][2][2]);

struct slaveInfo {
    int pipe[2][2];
    int filesToProcess;
};

int main(int argc, char *argv[]) {
    int pipes[SLAVE_QTY][2][2];
    int pid;
    int total_files = argc - 1;

    if(total_files == 0){
        printf("no se ingresaron archivos a procesar\n");
        return 1;
    }

    int initial_files_qty = total_files * PERCENTAJE_INITIAL;
    if (initial_files_qty == 0)
    {
        initial_files_qty = SLAVE_QTY;
    }

    int files_per_slave = initial_files_qty / SLAVE_QTY;
    int remaining_files = total_files - initial_files_qty;

    printf("total files: %d\n", total_files);
    printf("initial files: %d\n", initial_files_qty);
    printf("files per slave: %d\n", files_per_slave);
    printf("remaining files: %d\n", remaining_files);

    setup_pipes(pipes);
    
    int current_index = 1; // indice del archivo a procesar

    for(int i = 0; i < SLAVE_QTY && i < initial_files_qty ; i++){
        pid = fork();

        if(pid < 0) {
            perror("Error al crear el proceso");
            return 1;
        } else if (pid == 0) { // Este es el proceso hijo
            // pipes[i][0] de main a slave
            // pipes[i][1] de slave a main
            close(pipes[i][0][W_END]); 
            close(pipes[i][1][R_END]);

            close(STDIN_FILENO);
            dup2(pipes[i][0][R_END], STDIN_FILENO);
            close(pipes[i][0][R_END]);

            //para ver la salida del hijo, comentar el pipeo de su salida
            close(STDOUT_FILENO);
            dup2(pipes[i][1][W_END], STDOUT_FILENO);
            close(pipes[i][1][W_END]);

            char *args[] = {"./slave", NULL};
            execve(args[0], args, NULL);
            perror("Error al ejecutar el proceso hijo"); // si execve falla
            exit(EXIT_FAILURE); // Salir si execve falla
        }
        // Proceso padre
        close(pipes[i][0][R_END]);
        close(pipes[i][1][W_END]);
    }

    // se envian los archivos iniciales
    for (int i = 0; i < SLAVE_QTY; i++) {
        for (int j = 0; j < files_per_slave; j++) {
            if (current_index <= total_files) {
                sendFile(pipes[i][0][1], argv[current_index]);
                current_index++;
            }
        }
    }
    /*while (remaining_files > 0)
    {
        if (se recibe DONE por STDIN)
        {
            enviarle un nuevo archivo al slave que mando DONE
        }
    }*/

    return 0;
}

void setup_pipes(int pipes[SLAVE_QTY][2][2]) {
    for (int i = 0; i < SLAVE_QTY; i++) {
        if (pipe(pipes[i][0]) == -1) {  // Pipe main->slave
            perror("Error al crear el pipe main->slave");
            exit(EXIT_FAILURE);
        }
        if (pipe(pipes[i][1]) == -1) {  // Pipe slave->main
            perror("Error al crear el pipe slave->main");
            exit(EXIT_FAILURE);
        }
    }
}

void sendFile(int pipe_fd, char *arg) {
    if (write(pipe_fd, arg, strlen(arg) + 1) == -1) {
        perror("Error al escribir en el pipe");
        exit(1);
    }
    write(pipe_fd, "\n", 1);
}

/* void sendFiles(int pipe_fd, char *arg[], int qty){
    for (int i = 0; i < qty; i++)
    {
        sendFile(pipe_fd, arg[i]);
    }
} no uso por ahora*/