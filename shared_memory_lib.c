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
#include <string.h>
#include "error.h"

const TSharedData ending_data = {
    .slave_pid = ENDING_PID,
    .response = ENDING_RESPONSE,
    .file_name = ENDING_FILENAME
};

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

void send_finishing_data(TSharedData* ptr, int index) {
    ptr[index] = ending_data;
}

void close_shared_memory(void* ptr) {

    if (munmap(ptr, SHARED_MEMORY_SIZE) == -1) ERROR_HANDLING(SHARED_MEMORY_UNMAPPING);
}

void delete_shared_memory(char* name) {
    if (shm_unlink(name) == -1) ERROR_HANDLING(SHARED_MEMORY_DELETING);
}

void populate_data_from_string(const char *str, char* delim, TSharedData *shared_data) {
    char *str_copy = strdup(str);
    if (!str_copy)
        ERROR_HANDLING(ERROR_DUPLICATING_STRING);
        
    char *token = strtok(str_copy, delim);
    if (token == NULL) {
        free(str_copy);
        ERROR_HANDLING(ERROR_PARSING_SLAVE_PID);
    }

    shared_data->slave_pid = atoi(token);

    token = strtok(NULL, delim);
    if (token == NULL) {
        free(str_copy);
        ERROR_HANDLING(ERROR_PARSING_FILENAME);
    }
    strncpy(shared_data->file_name, token, MAX_FILEPATH);
    shared_data->file_name[MAX_FILEPATH - 1] = '\0';

    token = strtok(NULL, delim);
    if (token == NULL) {
        free(str_copy);
        ERROR_HANDLING(ERROR_PARSING_RESPONSE);
    }
    strncpy(shared_data->response, token, MD5_SIZE);
    shared_data->response[MD5_SIZE] = '\0';

    free(str_copy);
}
