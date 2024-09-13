/**
 * @file shared_memory_lib.h
 * @brief Biblioteca para la gestión de memoria compartida.
 *
 * Este archivo contiene las definiciones y funciones necesarias para crear, obtener,
 * enviar datos de finalización, cerrar y eliminar memoria compartida, así como para
 * poblar datos desde una cadena.
 */

#include "globals.h"
#ifndef SHARED_MEMORY_LIB_H
#define SHARED_MEMORY_LIB_H

#define MAX_FILES                   256


#define SHARED_MEMORY_SIZE          (sizeof(TSharedData) * MAX_FILES)
#define SHARED_MEMORY_PERMISSIONS   0666

#define ENDING_PID                  -1
#define ENDING_RESPONSE             "END OF RESPONSE"
#define ENDING_FILENAME             "NO FILENAME"

/**
 * @brief Estructura que representa los datos compartidos.
 */
typedef struct {
    int slavePID;
    char response[MD5_SIZE+1];
    char fileName[MAX_FILENAME];
} TSharedData;


/**
 * @brief Datos de finalización constantes.
 */
extern const TSharedData ending_data;


/**
 * @brief Crea una memoria compartida.
 *
 * @param name Nombre de la memoria compartida.
 * @return Puntero a la memoria compartida creada.
 */
TSharedData* create_shared_memory(char* name);

/**
 * @brief Obtiene una memoria compartida existente.
 *
 * @param name Nombre de la memoria compartida.
 * @return Puntero a la memoria compartida obtenida.
 */
TSharedData* get_shared_memory(char* name);

/**
 * @brief Envía datos de finalización a la memoria compartida.
 *
 * @param ptr Puntero a la memoria compartida.
 * @param index Índice en la memoria compartida donde se enviarán el dato de finalización.
 */
void send_finishing_data(TSharedData* ptr, int index);

/**
 * @brief Cierra la memoria compartida.
 *
 * @param ptr Puntero a la memoria compartida.
 */
void close_shared_memory(void* ptr);

/**
 * @brief Elimina la memoria compartida.
 *
 * @param name Nombre de la memoria compartida.
 */
void delete_shared_memory(char* name);

/**
 * @brief Popula la shared memory desde una cadena.
 *
 * @param str Cadena de entrada.
 * @param delim Delimitador utilizado en la cadena.
 * @param shared_data Puntero a la estructura de datos compartidos a popular.
 */
void populate_data_from_string(const char *str, char* delim, TSharedData *shared_data);

#endif