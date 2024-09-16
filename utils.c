// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "utils.h"

int setup_file_distribution(int slave_qty, int total_files, int* initial_files_qty){
    *initial_files_qty = (int)ceil(total_files * PERCENTAJE_INITIAL);

    // Si la cantidad inicial de archivos es menor que la cantidad de esclavos, asignamos al menos un archivo por esclavo
    if (*initial_files_qty < slave_qty) {
        *initial_files_qty = slave_qty;
    }

    int files_per_slave = (int)ceil((double)(*initial_files_qty) / slave_qty);

    return files_per_slave;
}

void send_file(int pipe_fd, char *arg, int* current_index) {
    if (write(pipe_fd, arg, strlen(arg)) == -1) ERROR_HANDLING(PIPE_WRITING);
    write(pipe_fd, "\n", 1);
    (*current_index)++;
}

void setup_slaves(TSlaveInfo* slaves_info, int slave_qty, int slot_size) {
    for (int i = 0; i < slave_qty; i++) {
        int pipe1[2];
        if (pipe(pipe1) == -1) ERROR_HANDLING(APP_SLAVE_PIPE_CREATION);

        slaves_info[i].pipes[MAIN_TO_SLAVE].fd_R = pipe1[R_END];
        slaves_info[i].pipes[MAIN_TO_SLAVE].fd_W = pipe1[W_END];

        int pipe2[2];
        if (pipe(pipe2) == -1) ERROR_HANDLING(SLAVE_APP_PIPE_CREATION);

        slaves_info[i].pipes[SLAVE_TO_APP].fd_R = pipe2[R_END];
        slaves_info[i].pipes[SLAVE_TO_APP].fd_W = pipe2[W_END];
    
        slaves_info[i].files_to_process = slot_size;

    }
}

void end_slavery(int slave_qty) {
    for (int i = 0; i < slave_qty; i++) {
        if(wait(NULL) == -1)
            ERROR_HANDLING(SLAVE_WAITING);
    }
}

int is_closed(int fd) {
    return fcntl(fd, F_GETFD) == -1 && errno == EBADF;
}
void end_data_sending(TSharedData* ptr, TSemaphore* sem, int index) {
    send_finishing_data(ptr, index);
    post_semaphore(sem);
}