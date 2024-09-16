// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  // Biblioteca necesaria para shm_open
#include <sys/mman.h>  // Biblioteca necesaria para mmap y shm_open
#include <unistd.h>  // Biblioteca necesaria para ftruncate
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include "shared_memory_lib.h"
#include "semaphore_lib.h"
#include "error.h"
#include "errno.h"

int main(int argc, char const *argv[]) {

    char shared_memory_buffer[RESPONSE_SIZE];
    int bytes_read = 0;

    if (argc - 1 == 0) { // Conexión con main mediante el pipe
        if ((bytes_read = read(STDIN_FILENO, shared_memory_buffer, RESPONSE_SIZE - 1)) == -1)
            ERROR_HANDLING(STDIN_READING);
        
    } else if (argc-1 == 1)  { // Conexión como parametro de ejecución
        strncpy(shared_memory_buffer, argv[1], RESPONSE_SIZE);
        bytes_read = strlen(argv[1]);
    } else ERROR_HANDLING(NO_CONNECTION_PARAMETER); // Para conectar ambos procesos, se debe optar por alguna de las dos alternativas anteriores

    shared_memory_buffer[bytes_read] = '\0';

    TSharedData* shm_view_ptr = get_shared_memory(shared_memory_buffer);
    TSemaphore* sem_view = get_semaphore(shared_memory_buffer);

    TSharedData data_read;


    int index = 0;
    wait_semaphore(sem_view);
    data_read = shm_view_ptr[index];

    while (strcmp(data_read.response, ENDING_RESPONSE) != 0) {
        printf("VIEW[%d]: Slave PID: %d, MD5: %s, FILE: %s\n", index, shm_view_ptr[index].slave_pid, shm_view_ptr[index].response, shm_view_ptr[index].file_name);
        index++;
        wait_semaphore(sem_view);
        data_read = shm_view_ptr[index];
    }
    
    printf("\nVIEW FINISHED\n");

    close_semaphore(sem_view);
    close_shared_memory(shm_view_ptr);
    return 0;
}
