#include "globals.h"
#ifndef SHARED_MEMORY_LIB_H
#define SHARED_MEMORY_LIB_H

#define SHARED_MEMORY_SIZE          (sizeof(TSharedData) * MAX_FILES)
#define SHARED_MEMORY_PERMISSIONS   0666

#define ENDING_PID                  -1
#define ENDING_RESPONSE             "END OF RESPONSE"
#define ENDING_FILENAME             "NO FILENAME"


typedef struct {
    int slavePID;
    char response[MD5_SIZE+1];
    char fileName[MAX_FILENAME];
} TSharedData;

extern const TSharedData ending_data;

TSharedData* create_shared_memory(char* name);
TSharedData* get_shared_memory(char* name);
void send_finishing_data(TSharedData* ptr, int index);
void close_shared_memory(void* ptr);
void delete_shared_memory(char* name);
void populate_data_from_string(const char *str, char* delim, TSharedData *shared_data);

#endif