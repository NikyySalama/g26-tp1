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

#define     APP_TO_SLAVE        0
#define     SLAVE_TO_APP        1

typedef struct {
    int fdR;
    int fdW;
} TPipe;

typedef struct {
    TPipe pipes[2];
    int filesToProcess;
} TSlaveInfo;

void sendFile (int pipe_fd, char *arg);
void sendFiles(int pipe_fd, char *arg[], int qty);
void setup_pipes(TSlaveInfo* slavesInfo);

void printPipeStatuses(TSlaveInfo slavesInfo[]);

int main(int argc, char *argv[]) {
    int total_files = argc - 1;
    TSlaveInfo slavesInfo[SLAVE_QTY];

    if(total_files == 0){
        printf("no se ingresaron archivos a procesar\n");
        return 1;
    }

    int initial_files_qty = total_files * PERCENTAJE_INITIAL;

    if (initial_files_qty == 0) {
        initial_files_qty = SLAVE_QTY;
    }

    int files_per_slave = initial_files_qty / SLAVE_QTY;
    int remaining_files = total_files - initial_files_qty;

    printf("Total files: %d\n", total_files);
    printf("Initial files: %d\n", initial_files_qty);
    printf("Files per slave: %d\n", files_per_slave);
    printf("Remaining files: %d\n", remaining_files);

    setup_pipes(slavesInfo);
    printPipeStatuses(slavesInfo);
    
    int current_index = 1; // indice del archivo a procesar

    for(int i = 0; i < SLAVE_QTY && i < initial_files_qty ; i++){
        int pid = fork();

        if(pid < 0) {
            perror("Error al crear el proceso");
            return 1;
        } else if (pid == 0) { // Este es el proceso hijo
            dup2(slavesInfo[i].pipes[APP_TO_SLAVE].fdR, STDIN_FILENO); // Lo que escriba la app se redirige a STDIN para que el eslcavo lo levante de ahi            
            close(slavesInfo[i].pipes[APP_TO_SLAVE].fdR);

            dup2(slavesInfo[i].pipes[SLAVE_TO_APP].fdW, STDOUT_FILENO); // El proceso slave escribe en STDOUT, y queremos que esto se pipee al fdW correspondiente
            close(slavesInfo[i].pipes[SLAVE_TO_APP].fdW);

            char *args[] = {"./slave", NULL};
            execve(args[0], args, NULL);

            // Ante un fallo de execve
            perror("Error al ejecutar el proceso hijo");
            exit(EXIT_FAILURE);
        }
    }

    // se envian los archivos iniciales
    for (int i = 0; i < SLAVE_QTY; i++) {
        for (int j = 0; j < files_per_slave; j++) {
            if (current_index <= total_files) {
                sendFile(slavesInfo[i].pipes[APP_TO_SLAVE].fdW, argv[current_index]);
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

void printPipeStatuses(TSlaveInfo slavesInfo[]) {
    for (int i = 0; i < SLAVE_QTY; i++) {
        TSlaveInfo currentSlave = slavesInfo[i];
        printf("Slave N%d:\n", i+1);
        printf("\t- Pipe APP->SLAVE: R[%d] W[%d]\n", currentSlave.pipes[APP_TO_SLAVE].fdR, currentSlave.pipes[APP_TO_SLAVE].fdW);
        printf("\t- Pipe SLAVE->APP: R[%d] W[%d]\n", currentSlave.pipes[SLAVE_TO_APP].fdR, currentSlave.pipes[SLAVE_TO_APP].fdW);
        printf("Files to process: %d\n", currentSlave.filesToProcess);
        printf("\n");
    }
    
}

void setup_pipes(TSlaveInfo* slavesInfo) {
    for (int i = 0; i < SLAVE_QTY; i++) {
        int pipe1[2];
        if (pipe(pipe1) == -1) {  // Pipe main->slave
            perror("Error al crear el pipe app->slave");
            exit(EXIT_FAILURE);
        }
        slavesInfo[i].pipes[APP_TO_SLAVE].fdR = pipe1[R_END];
        slavesInfo[i].pipes[APP_TO_SLAVE].fdW = pipe1[W_END];

        int pipe2[2];
        if (pipe(pipe2) == -1) {  // Pipe slave->main
            perror("Error al crear el pipe slave->app");
            exit(EXIT_FAILURE);
        }

        slavesInfo[i].pipes[SLAVE_TO_APP].fdR = pipe2[R_END];
        slavesInfo[i].pipes[SLAVE_TO_APP].fdW = pipe2[W_END];
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