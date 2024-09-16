// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/select.h>
#include "shared_memory_lib.h"
#include "semaphore_lib.h"
#include "utils.h"
#include "error.h"


#define     BASE_SLAVE_QTY          10
#define     S_WAIT_FOR_VIEW         5
#define     APP_VIEW_CONNECTION     "/app_view_connection"


int main(int argc, char *argv[]) {
    int current_index = 1; // Indice del archivo a procesar
    int total_files = argc - 1;

    int slave_qty = BASE_SLAVE_QTY;

    if (total_files < BASE_SLAVE_QTY)  // No vamos a generar esclavos de mas
        slave_qty = total_files;

    TSlaveInfo slaves_info[slave_qty];

    if(total_files == 0)
        ERROR_HANDLING(NO_FILES_ENTERED);

    int initial_files_qty;

    int files_per_slave = setup_file_distribution(slave_qty, total_files, &initial_files_qty);

    setup_slaves(slaves_info, slave_qty, files_per_slave);

    TSharedData* shm_main_ptr = create_shared_memory(APP_VIEW_CONNECTION);
    TSemaphore* sem_main = create_semaphore(APP_VIEW_CONNECTION);

    printf(APP_VIEW_CONNECTION);
    fflush(stdout);
    sleep(S_WAIT_FOR_VIEW); // Esperamos a que haya un proceso vista para conectar la salida

    for(int i = 0; i < slave_qty; i++){
        int pid = fork();

        if(pid < 0) ERROR_HANDLING(PROCESS_CREATING);

        else if (pid == 0) { // Este es el proceso hijo

            for (int j = 0; j < slave_qty; j++) {
                if (j != i) { 
                    close(slaves_info[j].pipes[MAIN_TO_SLAVE].fd_R);
                    close(slaves_info[j].pipes[MAIN_TO_SLAVE].fd_W);
                    close(slaves_info[j].pipes[SLAVE_TO_APP].fd_R);
                    close(slaves_info[j].pipes[SLAVE_TO_APP].fd_W);
                }
            }
        
            close(slaves_info[i].pipes[MAIN_TO_SLAVE].fd_W);
            close(slaves_info[i].pipes[SLAVE_TO_APP].fd_R);

            dup2(slaves_info[i].pipes[MAIN_TO_SLAVE].fd_R, STDIN_FILENO); // Lo que escriba la app se redirige a STDIN para que el eslcavo lo levante de ahi            
            close(slaves_info[i].pipes[MAIN_TO_SLAVE].fd_R);

            dup2(slaves_info[i].pipes[SLAVE_TO_APP].fd_W, STDOUT_FILENO); // El proceso slave escribe en STDOUT, y queremos que esto se pipee al fd_W correspondiente
            close(slaves_info[i].pipes[SLAVE_TO_APP].fd_W);

            char *args[] = {"./slave", NULL};
            execve(args[0], args, NULL);

            // Ante un fallo de execve
            ERROR_HANDLING(CHILD_PROCESS_EXECUTING);

        } else { // Proceso padre
            close(slaves_info[i].pipes[MAIN_TO_SLAVE].fd_R);
            close(slaves_info[i].pipes[SLAVE_TO_APP].fd_W);
        }
    }
    // Envio de datos
    for (int i = 0; i < slave_qty; i++) {
        for(int f = 0; f < files_per_slave; f++) {
            send_file(slaves_info[i].pipes[MAIN_TO_SLAVE].fd_W, argv[current_index], &current_index);
        }
    }

    int remaining_files = total_files;

    while (remaining_files > 0) {
        fd_set fdSet;
        FD_ZERO(&fdSet);

        for(int i = 0; i < slave_qty; i++){
            if (!is_closed(slaves_info[i].pipes[SLAVE_TO_APP].fd_R)) FD_SET(slaves_info[i].pipes[SLAVE_TO_APP].fd_R, &fdSet); // Agregamos este file descriptor para que se lo tenga en cuenta a la hora de escuchar cambios
        }

        if (select(slaves_info[slave_qty-1].pipes[SLAVE_TO_APP].fd_R+1, &fdSet, NULL, NULL, NULL) < 0) ERROR_HANDLING(FDS_SELECT);

        for (int i = 0; i < slave_qty; i++) {
            int fd_slave = slaves_info[i].pipes[SLAVE_TO_APP].fd_R;
            if (FD_ISSET(fd_slave, &fdSet)) {

                ssize_t bytes_read;
                if (slaves_info[i].files_to_process > 0) {
                    char buffer[RESPONSE_SIZE * initial_files_qty];

                    bytes_read = read(fd_slave, buffer, (RESPONSE_SIZE * initial_files_qty));
    
                    if (bytes_read == -1) ERROR_HANDLING(PIPE_READING);
                    else if (bytes_read == 0) break;

                    buffer[bytes_read] = '\0';
                    
                    char* save_ptr;
                    char *slave_response = strtok_r(buffer, SEPARATOR, &save_ptr);
                    while (slave_response != NULL) {
                        int shm_index = total_files - remaining_files;

                        FILE *file = fopen("results.txt", "a");
                        
                        if (file == NULL) ERROR_HANDLING(FILE_OPENING);

                        fprintf(file, "%s\n", slave_response);

                        fclose(file);

                        // Procesamos la línea (respuesta de un slave)
                        
                        populate_data_from_string(slave_response, DELIMITER, &(shm_main_ptr[shm_index]));

                        post_semaphore(sem_main);
                        remaining_files--;
                        slaves_info[i].files_to_process--;

                        slave_response = strtok_r(NULL, SEPARATOR, &save_ptr);
                    }   
                }
                
                if(slaves_info[i].files_to_process == 0) { // el slave ya no tiene archivos a procesar
                    if(current_index <= total_files){
                        send_file(slaves_info[i].pipes[MAIN_TO_SLAVE].fd_W, argv[current_index], &current_index);
                        slaves_info[i].files_to_process++;
                    } else{ // no hay mas files para procesar
                        close(slaves_info[i].pipes[MAIN_TO_SLAVE].fd_W);
                        close(slaves_info[i].pipes[MAIN_TO_SLAVE].fd_R);
                        close(slaves_info[i].pipes[SLAVE_TO_APP].fd_R);
                        // no es necesario cerrar slaves_info[i].pipes[SLAVE_TO_APP].fd_W porque se cierra automaticamente con la muerte del slave
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