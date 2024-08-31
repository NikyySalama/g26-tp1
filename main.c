// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "globals.h"
#include <fcntl.h>  // Biblioteca necesaria para shm_open

#include <stdlib.h>
#include <sys/mman.h>  // Biblioteca necesaria para mmap y shm_open
#include <sys/stat.h>
#include <semaphore.h>

#include <time.h>

#define SHARED_MEMORY_PERMISSIONS           0666
#define SEMAPHORE_PERMISSIONS               0664

#define SHARED_MEMORY_SIZE                  4096

#define     SLAVE_QTY       5

int main(int argc, char *argv[]) {
    void* shm_main_ptr = start_shared_memory(SHARED_MEMORY_NAME);
    sem_t *sem = sem_open(SEMAPHORE_NAME, O_CREAT, SEMAPHORE_PERMISSIONS, 0);
    if (sem == SEM_FAILED) {
        perror("Error abriendo el semáforo desde main");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SLAVE_QTY; i++) {
        sleep(1);
        printf("Mandando a procesar el resultado del SLAVE [%d]...\n", i+1);
        sprintf(shm_main_ptr, "S[%d]RESULT", i+1); // Simulo agregar el resultado del i-esimo esclavo
        sem_post(sem);
    }
    
    end_shared_memory(sem);
    sem_close(sem);
    return 0;
}
