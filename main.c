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

#define SHARED_MEMORY_PERMISSIONS   0666
#define SEMAPHORE_PERMISSIONS   0664

#define SHARED_MEMORY_SIZE          4096

#define     SLAVE_QTY       5

void viewTesting(void);
int main(int argc, char *argv[]) {
    viewTesting();
    sem_t *sem = sem_open(SEMAPHORE_NAME, O_CREAT, SEMAPHORE_PERMISSIONS, 1);
    if (sem == SEM_FAILED) {
        perror("Error abriendo el semáforo desde main");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < 100; i++) {
        printf(":p [%d]\n", i);
        
        // Dormir por 100 milisegundos
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 100 * 100000L; // 100 milisegundos en nanosegundos
        nanosleep(&ts, NULL);
    }
    sem_post(sem);

    printf("Lalalala");

    sem_close(sem);
    // int pid;
    // for(int i = 1; i <= SLAVE_QTY; i++){
    //     pid = fork();

    //     if(pid < 0) {
    //         perror("Error al crear el proceso");
    //         return 1;
    //     } else if (pid == 0) {
    //         // Este es el proceso hijo, se ejecuta el codigo del hijo
    //         char *args[] = {"./slave", argv[i], NULL };
    //         execve(args[0], args, NULL);
    //         perror("Error al ejecutar el proceso hijo"); // si execve falla
    //         exit(1); // Salir si execve falla
    //     }
    // }
    return 0;
}

void viewTesting(void) {
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, SHARED_MEMORY_PERMISSIONS);
    if (shm_fd == -1) {
        perror("Error al abrir el bloque de memoria compartida con view");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd,SHARED_MEMORY_SIZE) == -1) {
		perror("Error al asignarle un espacio de memoria");
		exit(EXIT_FAILURE);
	}

    void *ptr = mmap(0, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

    sprintf(ptr, "Vamos a dejar esto por parte de main\n");

    if (munmap(ptr, SHARED_MEMORY_SIZE) == -1) {
        perror("Error al desmapear la memoria compartida por parte del main");
        exit(EXIT_FAILURE);
    }
}