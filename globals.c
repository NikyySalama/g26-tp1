#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  // Biblioteca necesaria para shm_open
#include <sys/mman.h>  // Biblioteca necesaria para mmap y shm_open
#include <unistd.h>  // Biblioteca necesaria para ftruncate
#include <sys/stat.h>
#include <semaphore.h>

#define SHARED_MEMORY_SIZE          sizeof(TSharedData) * 100 // Almacenamos los resultados
// ! Ver si se puede hacer almacenando de a un solo sharedData
// #define SHARED_MEMORY_SIZE          MD5_SIZE+1 // Solo almacenamos un único resultado por esclavo a la vez

#define SHARED_MEMORY_PERMISSIONS   0666

// TODO hacer una libreria de shared memory 

void* start_shared_memory(char* name){
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, SHARED_MEMORY_PERMISSIONS);
    if (shm_fd == -1) {
        // TODO coordinar que hacemos ante error
        perror("Error al abrir el bloque de memoria compartida main-view");
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

void end_shared_memory(void* ptr, char* name) {
    if (munmap(ptr, name) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(EXIT_FAILURE);
    }
}

void delete_shared_memory(void* ptr, char* name) {
    if (shm_unlink(name) == -1) {
        perror("Error al eliminar el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }
}