/**
 * @file error.h
 * @brief Manejo de errores.
 *
 * Este archivo contiene los mensajes y la macro necesaria manejar
 * correctamente los estados de error dentro del trabajo práctico
 */

#ifndef MESSAGES_H
#define MESSAGES_H
#include "errno.h"

// Mensajes de error en archivos
#define FILE_OPENING                            "No se pudo abrir el archivo\n"
#define NO_FILES_ENTERED                        "No se ingresaron archivos a procesar\n"

// Mensajes de error de Pipes
#define PIPE_READING                            "No se pudo leer del pipe indicado\n"
#define PIPE_WRITING                            "No se pudo escribir en el pipe indicado\n"
#define APP_SLAVE_PIPE_CREATION                 "No se pudo crear el pipe app->slave\n"
#define SLAVE_APP_PIPE_CREATION                 "No se pudo crear el pipe slave->app\n"
#define SLAVE_MD5_PIPE_CREATION                 "No se pudo crear el pipe slave->MD5\n"

// Mensajes de error de procesos
#define PROCESS_CREATING                        "No se pudo crear el proceso\n"
#define CHILD_PROCESS_EXECUTING                 "No se pudo ejecutar el proceso hijo\n"

// Mensajes de error de File Descriptors
#define FDS_SELECT                              "No se pudo realizar select() de los FileDescriptors\n"
#define STDIN_READING                           "No se pudo leer desde la entrada estándar\n"

// Mensajes de error de semáforos
#define SEMAPHORE_CREATING                      "No se pudo crear el semáforo\n"
#define SEMAPHORE_GETTING                       "No se pudo obtener el semáforo\n"
#define SEMAPHORE_WAITING                       "No se pudo hacer wait() del semáforo\n"
#define SEMAPHORE_POSTING                       "No se pudo hacer post() del semáforo\n"
#define SEMAPHORE_CLOSING                       "No se pudo cerrar el semáforo\n"
#define SEMAPHORE_DESTROYING                    "No se pudo destruir el semáforo\n"

// Mensajes de error de memoria compartida
#define SHARED_MEMORY_OPENING                   "No se pudo abrir el bloque de memoria compartida\n"
#define SHARED_MEMORY_MAPPING                   "No se pudo asignar un espacio para el bloque de memoria compartida\n"
#define SHARED_MEMORY_MMAP                      "No se pudo realizar mmap() sobre el bloque de memoria compartida\n"
#define SHARED_MEMORY_GETTING                   "No se pudo obtener el bloque de memoria compartida\n"
#define SHARED_MEMORY_UNMAPPING                 "No se pudo desmapear el bloque de memoria compartida\n"
#define SHARED_MEMORY_DELETING                  "No se pudo eliminar el bloque de memoria compartida\n"

// Mensajes de error de tokenización
#define ERROR_DUPLICATING_STRING                "No se pudo duplicar la cadena\n"
#define ERROR_PARSING_SLAVE_PID                 "No se pudo parsear el slave_pid\n"
#define ERROR_PARSING_FILENAME                  "No se pudo parsear el nombre del archivo\n"
#define ERROR_PARSING_RESPONSE                  "No se pudo parsear la respuesta (MD5)\n"


// Macro
#define ERROR_HANDLING(error_details)                           \
    do {                                                        \
        fprintf(stderr, "ERROR: %s:\n", error_details);         \
        exit(EXIT_FAILURE);                                     \
    } while (0)
#endif