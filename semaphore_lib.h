#include <semaphore.h>

#ifndef SEMAPHORE_LIB_H
#define SEMAPHORE_LIB_H

#define SEM_NAME        "/app_view_semaphore"

typedef sem_t TSemaphore;

TSemaphore* create_semaphore(char* name);
TSemaphore* get_semaphore(char* name);
void wait_semaphore(TSemaphore* semaphore);
void post_semaphore(TSemaphore* semaphore);
void delete_semaphore(char* name, TSemaphore* semaphore);

#endif