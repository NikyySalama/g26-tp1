#ifndef GLOBALS_H
#define GLOBALS_H

#define SEMAPHORE_NAME   "/application_view_semaphore"
#define SHARED_MEMORY_NAME "/application_view_shared_memory"

void* start_shared_memory(void);
void end_shared_memory(void* ptr);
void delete_shared_memory(void* ptr);

#endif