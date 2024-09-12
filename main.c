// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include "shared_memory_lib.h"
#include "semaphore_lib.h"
#include "error.h"
#include "pipe_lib.h"

#define     SLAVE_QTY               5
#define     PERCENTAJE_INITIAL      0.1

#define     APP_TO_SLAVE            0
#define     SLAVE_TO_APP            1

#define     S_WAIT_FOR_VIEW         2
#define     APP_VIEW_CONNECTION     "/app_view_connection"

typedef struct {
    TPipe pipes[2];
    int filesToProcess;
} TSlaveInfo;

void send_file (int pipe_fd, char *arg);
void setup_slaves(TSlaveInfo* slavesInfo, int slotSize);
int is_closed(int fd);
void end_data_sending(TSharedData* ptr, TSemaphore* sem, int index);

int current_index = 1; // indice del archivo a procesar

int main(int argc, char *argv[]) {
    int total_files = argc - 1;
    TSlaveInfo slavesInfo[SLAVE_QTY];

    if(total_files == 0)
        ERROR_HANDLING(NO_FILES_ENTERED);

    int initial_files_qty = total_files * PERCENTAJE_INITIAL;

    if (initial_files_qty == 0) {
        initial_files_qty = SLAVE_QTY; // De no haber suficientes archivos, entonces cada esclavo procesará uno único
    }

    int files_per_slave = initial_files_qty / SLAVE_QTY;
    int remaining_files = total_files;

    setup_slaves(slavesInfo, files_per_slave);

    TSharedData* shm_main_ptr = create_shared_memory(APP_VIEW_CONNECTION);
    TSemaphore* sem_main = create_semaphore(APP_VIEW_CONNECTION);

    sleep(S_WAIT_FOR_VIEW); // Esperamos a que haya un proceso vista para conectar la salida
    printf(APP_VIEW_CONNECTION);
    fflush(stdout);

    for(int i = 0; i < SLAVE_QTY; i++){
        int pid = fork();

        if(pid < 0) ERROR_HANDLING(PROCESS_CREATING);

        else if (pid == 0) { // Este es el proceso hijo

            for (int j = 0; j < SLAVE_QTY; j++) {
                if (j != i) { 
                    close(slavesInfo[j].pipes[APP_TO_SLAVE].fdR);
                    close(slavesInfo[j].pipes[APP_TO_SLAVE].fdW);
                    close(slavesInfo[j].pipes[SLAVE_TO_APP].fdR);
                    close(slavesInfo[j].pipes[SLAVE_TO_APP].fdW);
                }
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
            ERROR_HANDLING(CHILD_PROCESS_EXECUTING);

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

    while (remaining_files > 0) {
        fd_set fdSet;
        FD_ZERO(&fdSet);

        for(int i = 0; i < SLAVE_QTY; i++){
            if (!is_closed(slavesInfo[i].pipes[SLAVE_TO_APP].fdR)) FD_SET(slavesInfo[i].pipes[SLAVE_TO_APP].fdR, &fdSet); // Agregamos este file descriptor para que se lo tenga en cuenta a la hora de escuchar cambios
        }

        if (select(slavesInfo[SLAVE_QTY-1].pipes[SLAVE_TO_APP].fdR+1, &fdSet, NULL, NULL, NULL) < 0) ERROR_HANDLING(FDS_SELECT);

        for (int i = 0; i < SLAVE_QTY; i++) {
            int fdSlave = slavesInfo[i].pipes[SLAVE_TO_APP].fdR;
            if (FD_ISSET(fdSlave, &fdSet)) {

                ssize_t bytes_read;
                while (slavesInfo[i].filesToProcess > 0) {
                    char buffer[RESPONSE_SIZE * initial_files_qty];

                    bytes_read = read(fdSlave, buffer, (RESPONSE_SIZE * initial_files_qty));
    
                    if (bytes_read == -1) ERROR_HANDLING(PIPE_READING);
                    else if (bytes_read == 0) break;

                    buffer[bytes_read] = '\0';

                    char *slave_response = strtok(buffer, "\t");
                    while (slave_response != NULL) {
                        int shm_index = total_files - remaining_files;

                        FILE *file = fopen("results.txt", "a");
                        
                        if (file == NULL) ERROR_HANDLING(FILE_OPENING);

                        fprintf(file, "%s", slave_response);

                        fclose(file);

                        // Procesamos la línea (respuesta de un slave)
                        
                        populate_data_from_string(slave_response, DELIMITER, &(shm_main_ptr[shm_index]));
                        
                        printf("\n%s", slave_response);

                        post_semaphore(sem_main);
                        remaining_files--;
                        slavesInfo[i].filesToProcess--;

                        slave_response = strtok(NULL, "\t");
                    }   
                }
                
                if(slavesInfo[i].filesToProcess == 0) { // el slave ya no tiene archivos a procesar
                    if(current_index <= total_files){
                        send_file(slavesInfo[i].pipes[APP_TO_SLAVE].fdW, argv[current_index]);
                        slavesInfo[i].filesToProcess++;
                    } else{ // no hay mas files para procesar
                        close(slavesInfo[i].pipes[APP_TO_SLAVE].fdW);
                        close(slavesInfo[i].pipes[APP_TO_SLAVE].fdR);
                        close(slavesInfo[i].pipes[SLAVE_TO_APP].fdR);
                        // no es necesario cerrar slavesInfo[i].pipes[SLAVE_TO_APP].fdW porque se cierra automaticamente con la muerte del slave
                    }
                }
            }
        }
    }

    end_data_sending(shm_main_ptr, sem_main, total_files);

    close_semaphore(sem_main);
    close_shared_memory(shm_main_ptr);

    delete_semaphore(APP_VIEW_CONNECTION);
    delete_shared_memory(APP_VIEW_CONNECTION);


    return 0;
}

void end_data_sending(TSharedData* ptr, TSemaphore* sem, int index) {
    send_finishing_data(ptr, index);
    post_semaphore(sem);
}

void setup_slaves(TSlaveInfo* slavesInfo, int slotSize) {
    for (int i = 0; i < SLAVE_QTY; i++) {
        int pipe1[2];
        if (pipe(pipe1) == -1) ERROR_HANDLING(APP_SLAVE_PIPE_CREATION);

        slavesInfo[i].pipes[APP_TO_SLAVE].fdR = pipe1[R_END];
        slavesInfo[i].pipes[APP_TO_SLAVE].fdW = pipe1[W_END];

        int pipe2[2];
        if (pipe(pipe2) == -1) ERROR_HANDLING(SLAVE_APP_PIPE_CREATION);

        slavesInfo[i].pipes[SLAVE_TO_APP].fdR = pipe2[R_END];
        slavesInfo[i].pipes[SLAVE_TO_APP].fdW = pipe2[W_END];
    
        slavesInfo[i].filesToProcess = slotSize;

    }
}

int is_closed(int fd) {
    return fcntl(fd, F_GETFD) == -1 && errno == EBADF;
}

void send_file(int pipe_fd, char *arg) {
    if (write(pipe_fd, arg, strlen(arg)) == -1) ERROR_HANDLING(PIPE_WRITING);
    write(pipe_fd, "\n", 1);
    current_index++;
}