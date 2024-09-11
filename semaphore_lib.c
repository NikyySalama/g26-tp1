// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "semaphore_lib.h"
#include <semaphore.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "error.h"


TSemaphore* create_semaphore(char* name){
    TSemaphore *sem = sem_open(name, O_CREAT | O_RDWR, 0666, 0);
    if (sem == SEM_FAILED) ERROR_HANDLING(SEMAPHORE_CREATING);
    return sem;
}

TSemaphore* get_semaphore(char* name){
    TSemaphore *sem = sem_open(name, O_RDONLY, 0);
    if (sem == SEM_FAILED) ERROR_HANDLING(SEMAPHORE_GETTING);
    return sem;
}

void wait_semaphore(TSemaphore* semaphore){
    if (sem_wait(semaphore) == -1) ERROR_HANDLING(SEMAPHORE_WAITING);
}
void post_semaphore(TSemaphore* semaphore){
    if (sem_post(semaphore) == -1) ERROR_HANDLING(SEMAPHORE_POSTING);
}
void delete_semaphore(char* name, TSemaphore* semaphore){
    if (sem_close(semaphore) == -1) ERROR_HANDLING(SEMAPHORE_CLOSING);
    if (sem_unlink(name) == -1) ERROR_HANDLING(SEMAPHORE_DESTROYING);
}