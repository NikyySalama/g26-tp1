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

#include "globals.h"

#define BUFFER_SIZE                 100

int main(int argc, char const *argv[]) {
    // TODO hacer que funcione si están conectados en terminales distintas
    char shared_memory_buffer[BUFFER_SIZE];
    if (fgets(shared_memory_buffer, BUFFER_SIZE, stdin) == NULL) {
        perror("Error al leer desde la entrada estandar");
        return 1;
    }

    void* shm_view_ptr = start_shared_memory(shared_memory_buffer);
    // ? El semaforo es global?
    sem_t *sem_view = sem_open(SEMAPHORE_NAME, O_CREAT, SEMAPHORE_PERMISSIONS, 1);
    if (sem_view == SEM_FAILED) {
        perror("Error abriendo el semáforo desde view");
        exit(EXIT_FAILURE);
    }

    int s = 0;
    while (s < 100) {
        sem_wait(sem_view);
        TSharedData* slaveResult = (TSharedData*) shm_view_ptr;
        printf("VIEW: Slave ID: %d, MD5: %s, FILE: %s\n", slaveResult[s].slaveID, slaveResult[s].response, slaveResult[s].fileName);
        sem_post(sem_view);
        s++;
    }

    printf("\nVIEW FINISHED\n");

    return 0;
}
