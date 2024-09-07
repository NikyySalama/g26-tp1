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

#include "globals.h"
#include "shared_memory_lib.h"
#include "semaphore_lib.h"

#define BUFFER_SIZE                 100

int main(int argc, char const *argv[]) {

    char shared_memory_buffer[BUFFER_SIZE];
    int bytesRead = 0;
    if ((bytesRead = read(STDIN_FILENO, shared_memory_buffer, BUFFER_SIZE - 1)) == -1) {
        perror("Error leyendo de entrada estandar");
        exit(EXIT_FAILURE);
    }
    shared_memory_buffer[bytesRead] = '\0';

    TSharedData* shm_view_ptr = get_shared_memory(shared_memory_buffer);
    TSemaphore* sem_view = get_semaphore(SEM_NAME);

    int s = 0;
    while (s < 100) {
        wait_semaphore(sem_view);
        printf("VIEW: Slave ID: %d, MD5: %s, FILE: %s\n", shm_view_ptr[s].slaveID, shm_view_ptr[s].response, shm_view_ptr[s].fileName);
        post_semaphore(sem_view);
        s++;
    }

    printf("Conectandose a %s, de %d bytes\n", shared_memory_buffer, bytesRead);
    printf("\nVIEW FINISHED\n");

    return 0;
}
