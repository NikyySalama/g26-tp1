// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  // Biblioteca necesaria para shm_open
#include <sys/mman.h>  // Biblioteca necesaria para mmap y shm_open
#include <unistd.h>  // Biblioteca necesaria para ftruncate
#include <sys/stat.h>
#include <semaphore.h>
#include "globals.h"

#define SEMAPHORE_PERMISSIONS       0664

int main(int argc, char const *argv[]) {
    // Modificar el recibo del SHMNAME
    void* shm_view_ptr = start_shared_memory(SHARED_MEMORY_NAME);
    sem_t *sem = sem_open(SEMAPHORE_NAME, O_CREAT, SEMAPHORE_PERMISSIONS, 1);
    if (sem == SEM_FAILED) {
        perror("Error abriendo el semáforo desde view");
        exit(EXIT_FAILURE);
    }
    int hits = 0;   

    while (hits <= 5) {
        sem_wait(sem);
        char* slaveResult = (char*) shm_view_ptr;
        printf("VIEW: %s\n", slaveResult);
        hits++;
    }

    printf("\nVIEW FINISHED\n");

    end_shared_memory(shm_view_ptr);
    delete_shared_memory(shm_view_ptr);

    sem_close(sem);
    sem_destroy(sem);
    return 0;
}
