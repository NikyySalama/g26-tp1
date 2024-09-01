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
void setup_slaves(TSlaveInfo* slavesInfo, fd_set* fdSet, int slotSize, int current_index);

void printPipeStatuses(TSlaveInfo slavesInfo[]);

int main(int argc, char *argv[]) {
    int total_files = argc - 1;
    TSlaveInfo slavesInfo[SLAVE_QTY];
    fd_set fdSet; // Set de todos los descriptores

    if(total_files == 0){
        printf("no se ingresaron archivos a procesar\n");
        return 1;
    }

    int slot_files_qty = total_files * PERCENTAJE_INITIAL;

    if (slot_files_qty == 0) {
        slot_files_qty = SLAVE_QTY; // De no haber suficientes archivos, entonces cada esclavo procesará uno único
    }

    int files_per_slave = slot_files_qty / SLAVE_QTY;
    //int remaining_files = total_files - slot_files_qty;

    printf("Total files: %d\n", total_files);
    printf("Initial files: %d\n", slot_files_qty);
    printf("Files per slave: %d\n", files_per_slave);
    //printf("Remaining files: %d\n", remaining_files);

    int current_index = 1; // indice del archivo a procesar

    setup_slaves(slavesInfo, &fdSet, files_per_slave, current_index);
    // printPipeStatuses(slavesInfo);

    for(int i = 0; i < SLAVE_QTY; i++){
        int pid = fork();

        if(pid < 0) {
            perror("Error al crear el proceso");
            return 1;
        } else if (pid == 0) { // Este es el proceso hijo
        
            close(slavesInfo[i].pipes[APP_TO_SLAVE].fdW);
            close(slavesInfo[i].pipes[SLAVE_TO_APP].fdR);

            dup2(slavesInfo[i].pipes[APP_TO_SLAVE].fdR, STDIN_FILENO); // Lo que escriba la app se redirige a STDIN para que el eslcavo lo levante de ahi            
            close(slavesInfo[i].pipes[APP_TO_SLAVE].fdR);

            dup2(slavesInfo[i].pipes[SLAVE_TO_APP].fdW, STDOUT_FILENO); // El proceso slave escribe en STDOUT, y queremos que esto se pipee al fdW correspondiente
            close(slavesInfo[i].pipes[SLAVE_TO_APP].fdW);

            char *args[] = {"./slave", NULL};
            execve(args[0], args, NULL);

            // Ante un fallo de execve
            printf("Error\n");
            perror("Error al ejecutar el proceso hijo");
            exit(EXIT_FAILURE);
        }
    }
    // Envio de datos
    for (int i = 0; i < SLAVE_QTY; i++) {
        char i_str[2];
        sprintf(i_str, "%d", i);
        sendFile(slavesInfo[i].pipes[APP_TO_SLAVE].fdW, i_str);
    }

    while (current_index < total_files) { // TODO Reemplazar por la condición de no haber leido de todos
        // Dado que select es destructivo, debemos hacer una copia de seguridad del set de FDs
        // ! Se considera que el mayor fdR estará siempre en el último pipe. ¿Es correcto?
        fd_set fdSetCopy = fdSet;
        if (select(slavesInfo[SLAVE_QTY-1].pipes[SLAVE_TO_APP].fdR+1, &fdSet, NULL, NULL, NULL) < 0) {
            perror("Error con el select de FDs");
            exit(EXIT_FAILURE);
        }

        for (int i = slavesInfo[0].pipes[SLAVE_TO_APP].fdR; i < slavesInfo[SLAVE_QTY-1].pipes[SLAVE_TO_APP].fdR+1; i++) { // El máximo fd lo tendrá el último pipe
            if (FD_ISSET(i, &fdSetCopy)) {
                printf("El FD %d tiene data: ", i);
                char buffer[10];
                int bytesRead = read(i, buffer, 10);
                if (bytesRead == -1) {
                    printf("Error leyendo el pipe de %d\n", i);
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                printf(buffer);
                printf("\n");
        
                FD_CLR(i, &fdSet); // No lo seguimos escuchando (por el momento)
            }
        }

        // for (int i = 0; i < SLAVE_QTY; i++) {
        //     TSlaveInfo currentSlave = slavesInfo[i];
        //     printf("Slave N%d:\n", i+1);
        //     printf("\t- Mensaje SLAVE->APP: ");
        //     char buffer[10];
        //     int bytesRead = read(currentSlave.pipes[SLAVE_TO_APP].fdR, buffer, 10);
        //     if (bytesRead == -1) {
        //         printf("Error leyendo el pipe de %d!\n", i);
        //         perror("read");
        //         exit(EXIT_FAILURE);
        //     }
        //     printf(buffer);
        //     printf("\n");
        // }
    }
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

void setup_slaves(TSlaveInfo* slavesInfo, fd_set* fdSet, int slotSize, int current_index) {
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

        FD_SET(pipe2[R_END], fdSet); // Agregamos este file descriptor para que se lo tenga en cuenta a la hora de escuchar cambios

        slavesInfo[i].filesToProcess = slotSize;

        current_index += slotSize;
    }
}

void sendFile(int pipe_fd, char *arg) {
    if (write(pipe_fd, arg, strlen(arg) + 1) == -1) {
        perror("Error al escribir en el pipe");
        exit(1);
    }
    write(pipe_fd, "\n", 1);
}