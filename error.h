#ifndef MESSAGES_H
#define MESSAGES_H
#include "errno.h"
// File error messages
#define FILE_OPENING                            "No se pudo abrir el archivo\n"
#define NO_FILES_ENTERED                        "No se ingresaron archivos a procesar\n"

// Pipe error messages
#define PIPE_READING                            "No se pudo leer del pipe indicado\n"
#define PIPE_WRITING                            "No se pudo escribir en el pipe indicado\n"
#define APP_SLAVE_PIPE_CREATION                 "No se pudo crear el pipe app->slave\n"
#define SLAVE_APP_PIPE_CREATION                 "No se pudo crear el pipe slave->app\n"
#define SLAVE_MD5_PIPE_CREATION                 "No se pudo crear el pipe slave->MD5\n"

// Process error messages
#define PROCESS_CREATING                        "No se pudo crear el proceso\n"
#define CHILD_PROCESS_EXECUTING                 "No se pudo ejecutar el proceso hijo\n"

// File descriptors error messages
#define FDS_SELECT                              "No se pudo realizar select() de los FileDescriptors\n"
#define STDIN_READING                           "No se pudo leer desde la entrada estándar\n"

// Semaphore error messages
#define SEMAPHORE_CREATING                      "No se pudo crear el semáforo\n"
#define SEMAPHORE_GETTING                       "No se pudo obtener el semáforo\n"
#define SEMAPHORE_WAITING                       "No se pudo hacer wait() del semáforo\n"
#define SEMAPHORE_POSTING                       "No se pudo hacer post() del semáforo\n"
#define SEMAPHORE_CLOSING                       "No se pudo cerrar el semáforo\n"
#define SEMAPHORE_DESTROYING                    "No se pudo destruir el semáforo\n"

// Shared memory error messages
#define SHARED_MEMORY_OPENING                   "No se pudo abrir el bloque de memoria compartida\n"
#define SHARED_MEMORY_MAPPING                   "No se pudo asignar un espacio para el bloque de memoria compartida\n"
#define SHARED_MEMORY_MMAP                      "No se pudo realizar mmap() sobre el bloque de memoria compartida\n"
#define SHARED_MEMORY_GETTING                   "No se pudo obtener el bloque de memoria compartida\n"
#define SHARED_MEMORY_UNMAPPING                 "No se pudo desmapear el bloque de memoria compartida\n"
#define SHARED_MEMORY_DELETING                  "No se pudo eliminar el bloque de memoria compartida\n"

// Macro
#define ERROR_HANDLING(error_details)                           \
    do {                                                        \
        fprintf(stderr, "ERROR: %s:\n", error_details);         \
        exit(EXIT_FAILURE);                                     \
    } while (0)
#endif