// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  // Biblioteca necesaria para shm_open
#include <sys/mman.h>  // Biblioteca necesaria para mmap y shm_open
#include <unistd.h>  // Biblioteca necesaria para ftruncate
#include <sys/stat.h>
#include "globals.h"

#define SHARED_MEMORY_PERMISSIONS   0666
#define SHARED_MEMORY_SIZE          4096

void* startSharedMemory(void);
void endSharedMemory(void* ptr);

int main(int argc, char const *argv[])
{
    void* shm_view_ptr = startSharedMemory();
    printf("Todo correctamente inicializado\n");
    // sprintf(shm_view_ptr, "Hola a todos!\n");
    printf("Lo que hay en el bloque compartido es: %s", (char*) shm_view_ptr);
    endSharedMemory(shm_view_ptr);
    return 0;
}

void* startSharedMemory(void) {
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, SHARED_MEMORY_PERMISSIONS);
    if (shm_fd == -1) {
        // TODO coordinar que hacemos ante error
        perror("Error al abrir el bloque de memoria compartida con main");
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

    return ptr;
}

void endSharedMemory (void* ptr) {
    if (munmap(ptr, SHARED_MEMORY_SIZE) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // TODO coordinar qué proceso hace esto
    if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
        perror("Error al eliminar el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }
}
