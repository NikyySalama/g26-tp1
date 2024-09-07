#include "shared_memory_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <semaphore.h>

#define SHARED_MEMORY_SIZE          (sizeof(TSharedData) * 100) // Almacenamos los resultados

#define SHARED_MEMORY_PERMISSIONS   0666

TSharedData* create_shared_memory(char* name){
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

    void *ptr = mmap(0, SHARED_MEMORY_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

    return (TSharedData*) ptr;
}

TSharedData* get_shared_memory(char* name) {
    int shm_fd = shm_open(name, O_RDONLY, 0);
    if (shm_fd == -1) {
        perror("Error al abrir el bloque de memoria compartida desde view");
        exit(EXIT_FAILURE);
    }

    void *ptr = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		perror("Error mapeando la memoria compartida");
        close(shm_fd);
		exit(EXIT_FAILURE);
	}

    // close(shm_fd);

    return (TSharedData*) ptr;
}

void end_shared_memory(void* ptr) {
    if (munmap(ptr, SHARED_MEMORY_SIZE) == -1) {
        perror("Error al desmapear la memoria compartida");
        exit(EXIT_FAILURE);
    }
}

void delete_shared_memory(char* name) {
    if (shm_unlink(name) == -1) {
        perror("Error al eliminar el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }
}