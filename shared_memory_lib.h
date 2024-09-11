#include "globals.h"
#ifndef SHARED_MEMORY_LIB_H
#define SHARED_MEMORY_LIB_H

#define SHARED_MEMORY_SIZE          (sizeof(TSharedData) * MAX_FILES)
#define SHARED_MEMORY_PERMISSIONS   0666

/*typedef struct {
    int slaveID;
    char response[MD5_SIZE+1];
    char fileName[MAX_FILENAME];
} TSharedData;*/

typedef char TSharedData[BUFFER_SIZE];

TSharedData* create_shared_memory(char* name);
TSharedData* get_shared_memory(char* name);
void end_shared_memory(void* ptr);
void delete_shared_memory(char* name);

#endif