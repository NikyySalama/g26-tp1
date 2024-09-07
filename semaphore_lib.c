#include "semaphore_lib.h"
#include <semaphore.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

TSemaphore* create_semaphore(char* name){
    TSemaphore *sem = sem_open(name, O_CREAT | O_RDWR, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Error creando el semáforo");
        exit(EXIT_FAILURE);
    }
    return sem;
}

TSemaphore* get_semaphore(char* name){
    TSemaphore *sem = sem_open(name, O_RDONLY, 0);
    if (sem == SEM_FAILED) {
        perror("Error obteniendo el semáforo");
        exit(EXIT_FAILURE);
    }
    return sem;
}

void wait_semaphore(TSemaphore* semaphore){
    if (sem_wait(semaphore) == -1) {
        perror("Error haciendo wait del semaforo");
        exit(EXIT_FAILURE);
    }
}
void post_semaphore(TSemaphore* semaphore){
    if (sem_post(semaphore) == -1) {
        perror("Error haciendo post del semaforo");
        exit(EXIT_FAILURE);
    }
}
void delete_semaphore(char* name, TSemaphore* semaphore){
    if (sem_close(semaphore) == -1) {
        perror("Error cerrando el semáforo");
    }
    if (sem_unlink(name) == -1) {
        perror("Error destruyendo el semáforo");
    }
}