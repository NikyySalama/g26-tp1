// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "shared_memory_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "error.h"

TSharedData* create_shared_memory(char* name){
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, SHARED_MEMORY_PERMISSIONS);
    if (shm_fd == -1) ERROR_HANDLING(SHARED_MEMORY_OPENING);

    if (ftruncate(shm_fd,SHARED_MEMORY_SIZE) == -1) ERROR_HANDLING(SHARED_MEMORY_MAPPING);

    void *ptr = mmap(0, SHARED_MEMORY_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) ERROR_HANDLING(SHARED_MEMORY_MMAP);

    return (TSharedData*) ptr;
}

TSharedData* get_shared_memory(char* name) {
    int shm_fd = shm_open(name, O_RDONLY, 0);
    if (shm_fd == -1) ERROR_HANDLING(SHARED_MEMORY_GETTING);

    void *ptr = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

	if (ptr == MAP_FAILED) ERROR_HANDLING(SHARED_MEMORY_MMAP);

    return (TSharedData*) ptr;
}

void end_shared_memory(void* ptr) {
    if (munmap(ptr, SHARED_MEMORY_SIZE) == -1) ERROR_HANDLING(SHARED_MEMORY_UNMAPPING);
}

void delete_shared_memory(char* name) {
    if (shm_unlink(name) == -1) ERROR_HANDLING(SHARED_MEMORY_DELETING);
}