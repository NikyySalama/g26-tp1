#ifndef GLOBALS_H
#define GLOBALS_H

#define SLAVE_QTY               5

#define MD5_SIZE                32
#define MAX_FILENAME            256

#define SEMAPHORE_NAME          "/application_view_semaphore"
#define SEMAPHORE_PERMISSIONS   0664


void* start_shared_memory(char* name);
void end_shared_memory(void* ptr, char* name);
void delete_shared_memory(void* ptr, char* name);

typedef struct {
    int slaveID;
    char response[MD5_SIZE+1];
    char fileName[MAX_FILENAME];
} TSharedData;

#endif