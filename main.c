#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include "globals.h"

#define     SLAVE_QTY           5
#define     PERCENTAJE_INITIAL  0.1
#define     W_END               1
#define     R_END               0

#define     BUFFER_SIZE         MD5_SIZE+1

#define     APP_TO_SLAVE        0
#define     SLAVE_TO_APP        1

#define     S_WAIT_FOR_VIEW     2
#define     SHARED_MEMORY_NAME  "/application_view_shared_memory"

typedef struct {
    int fdR;
    int fdW;
} TPipe;

typedef struct {
    TPipe pipes[2];
    int filesToProcess;
} TSlaveInfo;

void send_file (int pipe_fd, char *arg);
void setup_slaves(TSlaveInfo* slavesInfo, int slotSize);
int is_closed(int fd);

void print_pipe_statuses(TSlaveInfo slavesInfo[]);

int current_index = 1; // indice del archivo a procesar

int main(int argc, char *argv[]) {
    int total_files = argc - 1;
    TSlaveInfo slavesInfo[SLAVE_QTY];


    if(total_files == 0) {
        perror("No se ingresaron archivos a procesar\n");
        return 1;
    }

    int initial_files_qty = total_files * PERCENTAJE_INITIAL;

    if (initial_files_qty == 0) {
        initial_files_qty = SLAVE_QTY; // De no haber suficientes archivos, entonces cada esclavo procesará uno único
    }

    int files_per_slave = initial_files_qty / SLAVE_QTY;
    int remaining_files = total_files;

    setup_slaves(slavesInfo, files_per_slave);

    TSharedData* shm_main_ptr = start_shared_memory(SHARED_MEMORY_NAME);

    sem_t *sem_main = sem_open(SEMAPHORE_NAME, O_CREAT, SEMAPHORE_PERMISSIONS, 1);
    if (sem_main == SEM_FAILED) {
        perror("Error abriendo el semáforo desde view");
        exit(EXIT_FAILURE);
    }

    sleep(S_WAIT_FOR_VIEW); // Esperamos a que haya un proceso vista para conectar la salida
    printf("%s\n",SHARED_MEMORY_NAME);
    fflush(stdout);

    for(int i = 0; i < SLAVE_QTY; i++){
        int pid = fork();

        if(pid < 0) {
            perror("Error al crear el proceso");
            return 1;
        } else if (pid == 0) { // Este es el proceso hijo

            for (int j = 0; j < i; j++) {
                close(slavesInfo[j].pipes[APP_TO_SLAVE].fdR);
                close(slavesInfo[j].pipes[APP_TO_SLAVE].fdW);
                close(slavesInfo[j].pipes[SLAVE_TO_APP].fdR);
                close(slavesInfo[j].pipes[SLAVE_TO_APP].fdW);
            }
        
            close(slavesInfo[i].pipes[APP_TO_SLAVE].fdW);
            close(slavesInfo[i].pipes[SLAVE_TO_APP].fdR);

            dup2(slavesInfo[i].pipes[APP_TO_SLAVE].fdR, STDIN_FILENO); // Lo que escriba la app se redirige a STDIN para que el eslcavo lo levante de ahi            
            close(slavesInfo[i].pipes[APP_TO_SLAVE].fdR);

            dup2(slavesInfo[i].pipes[SLAVE_TO_APP].fdW, STDOUT_FILENO); // El proceso slave escribe en STDOUT, y queremos que esto se pipee al fdW correspondiente
            close(slavesInfo[i].pipes[SLAVE_TO_APP].fdW);

            char *args[] = {"./slave.o", NULL};
            execve(args[0], args, NULL);

            // Ante un fallo de execve
            perror("Error al ejecutar el proceso hijo");
            exit(EXIT_FAILURE);
        } else { // Proceso padre
            close(slavesInfo[i].pipes[APP_TO_SLAVE].fdR);
            close(slavesInfo[i].pipes[SLAVE_TO_APP].fdW);
        }
    }
    // Envio de datos
    for (int i = 0; i < SLAVE_QTY; i++) {
        for(int f = 0; f < files_per_slave; f++) {
            send_file(slavesInfo[i].pipes[APP_TO_SLAVE].fdW, argv[current_index]);
        }
    }

    while (remaining_files > 0) { // TODO Reemplazar por la condición de no haber leido de todos
        fd_set fdSet;
        FD_ZERO(&fdSet);

        for(int i = 0; i < SLAVE_QTY; i++){
            if (!is_closed(slavesInfo[i].pipes[SLAVE_TO_APP].fdR)) FD_SET(slavesInfo[i].pipes[SLAVE_TO_APP].fdR, &fdSet); // Agregamos este file descriptor para que se lo tenga en cuenta a la hora de escuchar cambios
        }

        if (select(slavesInfo[SLAVE_QTY-1].pipes[SLAVE_TO_APP].fdR+1, &fdSet, NULL, NULL, NULL) < 0) {
            perror("Error con el select de FDs");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < SLAVE_QTY; i++) {
            int fdSlave = slavesInfo[i].pipes[SLAVE_TO_APP].fdR;
            if (FD_ISSET(fdSlave, &fdSet)) {

                ssize_t bytesRead;
                while (slavesInfo[i].filesToProcess > 0){ //lee un md5 mas un \n
                    char buffer[BUFFER_SIZE];

                    bytesRead = read(fdSlave, buffer, BUFFER_SIZE);
                    // TODO enviar
                    FILE *file = fopen("resultado.txt", "a");
                    
                    if (file == NULL) {
                        perror("Error al abrir el archivo");
                        exit(EXIT_FAILURE);
                    }

                    fprintf(file, "%s", buffer);

                    fclose(file);

                    shm_main_ptr[100-remaining_files].slaveID = i ; // TODO recibir el PID del slave, no el indice del esclavo
                    
                    strncpy(shm_main_ptr[100-remaining_files].response, buffer, sizeof(shm_main_ptr[100-remaining_files].response) - 1);
                    strncpy(shm_main_ptr[100-remaining_files].fileName, argv[100-remaining_files + 1], sizeof(shm_main_ptr[100-remaining_files].response) - 1);
                    
                    shm_main_ptr[100-remaining_files].response[sizeof(shm_main_ptr[100-remaining_files].response) - 1] = '\0';
                    sem_post(sem_main);

                    remaining_files--;
                    slavesInfo[i].filesToProcess--; //el slave ya proceso un archivo
                }
                if (bytesRead == -1) {
                    perror("Error leyendo el pipe de %d\n");
                    exit(EXIT_FAILURE);
                }
                
                if(slavesInfo[i].filesToProcess == 0) { // el slave ya no tiene archivos a procesar
                    if(current_index <= total_files){
                        send_file(slavesInfo[i].pipes[APP_TO_SLAVE].fdW, argv[current_index]);
                        slavesInfo[i].filesToProcess++;
                    } else{ // no hay mas files para procesar
                        close(slavesInfo[i].pipes[APP_TO_SLAVE].fdW);
                        printf("Closed pipe write end for slave %d\n", i);
                        close(slavesInfo[i].pipes[APP_TO_SLAVE].fdR);
                        close(slavesInfo[i].pipes[SLAVE_TO_APP].fdR);
                        // no es necesario cerrar slavesInfo[i].pipes[SLAVE_TO_APP].fdW porque se cierra automaticamente con la muerte del slave
                    }
                }
            }
        }
    }
    
    end_shared_memory(shm_main_ptr, SHARED_MEMORY_NAME);
    delete_shared_memory(shm_main_ptr, SHARED_MEMORY_NAME);

    sem_close(sem_main);
    sem_destroy(sem_main);

    return 0;
}

void print_pipe_statuses(TSlaveInfo slavesInfo[]) {
    for (int i = 0; i < SLAVE_QTY; i++) {
        TSlaveInfo currentSlave = slavesInfo[i];
        printf("Slave N%d:\n", i+1);
        printf("\t- Pipe APP->SLAVE: R[%d] W[%d]\n", currentSlave.pipes[APP_TO_SLAVE].fdR, currentSlave.pipes[APP_TO_SLAVE].fdW);
        printf("\t- Pipe SLAVE->APP: R[%d] W[%d]\n", currentSlave.pipes[SLAVE_TO_APP].fdR, currentSlave.pipes[SLAVE_TO_APP].fdW);
        printf("Files to process: %d\n", currentSlave.filesToProcess);
        printf("\n");
    }
}

void setup_slaves(TSlaveInfo* slavesInfo, int slotSize) {
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
    
        slavesInfo[i].filesToProcess = slotSize;

    }
}

int is_closed(int fd) {
    return fcntl(fd, F_GETFD) == -1 && errno == EBADF;
}

void send_file(int pipe_fd, char *arg) {
    if (write(pipe_fd, arg, strlen(arg)) == -1) {
        perror("Error al escribir en el pipe");
        exit(1);
    }
    write(pipe_fd, "\n", 1);
    current_index++;
}