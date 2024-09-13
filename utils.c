#include "utils.h"



int setup_file_distribution(int slave_qty, int total_files, int* initial_files_qty){
    // Calcular la cantidad inicial de archivos a distribuir
    *initial_files_qty = (int)ceil(total_files * PERCENTAJE_INITIAL);

    // Si la cantidad inicial de archivos es menor que la cantidad de esclavos, asignar al menos un archivo por esclavo
    if (*initial_files_qty < slave_qty) {
        *initial_files_qty = slave_qty;
    }

    // Calcular la cantidad de archivos por esclavo, redondeando hacia arriba
    int files_per_slave = (int)ceil((double)(*initial_files_qty) / slave_qty);

    return files_per_slave;
}

void send_file(int pipe_fd, char *arg, int* current_index) {
    if (write(pipe_fd, arg, strlen(arg)) == -1) ERROR_HANDLING(PIPE_WRITING);
    write(pipe_fd, "\n", 1);
    (*current_index)++;
}

void setup_slaves(TSlaveInfo* slavesInfo, int slave_qty, int slotSize) {
    for (int i = 0; i < slave_qty; i++) {
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
void end_data_sending(TSharedData* ptr, TSemaphore* sem, int index) {
    send_finishing_data(ptr, index);
    post_semaphore(sem);
}