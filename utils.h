#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "shared_memory_lib.h"
#include "semaphore_lib.h"
#include "pipe_lib.h"

#include "error.h"

#ifndef UTILS_H
#define UTILS_H

typedef struct {
    TPipe pipes[2];
    int files_to_process;
} TSlaveInfo;

#define     MAIN_TO_SLAVE            0
#define     SLAVE_TO_APP            1
#define     PERCENTAJE_INITIAL      0.1

/**
 * @brief Configura la distribución de archivos entre los esclavos.
 *
 * @param slave_qty Cantidad de esclavos.
 * @param total_files Cantidad total de archivos a procesar.
 * @param initial_files_qty Puntero a la cantidad inicial de archivos por esclavo.
 * @return 0 si la configuración fue exitosa, -1 en caso de error.
 */
int setup_file_distribution(int slave_qty, int total_files, int* initial_files_qty);

/**
 * @brief Envía un archivo a través de un pipe.
 *
 * @param pipe_fd Descriptor de archivo del pipe.
 * @param arg Argumento que representa el archivo a enviar.
 * @param current_index Puntero al índice actual del archivo.
 */
void send_file(int pipe_fd, char *arg, int* current_index);

/**
 * @brief Espera y finaliza a los procesos hijos.
 *
 * @param slave_qty La cantidad de esclavos.
 */
void end_slavery(int slave_qty);

/**
 * @brief Finaliza el envío de datos a través de la memoria compartida.
 *
 * @param ptr Puntero a la memoria compartida.
 * @param sem Puntero al semáforo.
 * @param index Índice en la memoria compartida donde se enviarán los datos de finalización.
 */
void end_data_sending(TSharedData* ptr, TSemaphore* sem, int index);

/**
 * @brief Verifica si un descriptor de archivo está cerrado.
 *
 * @param fd Descriptor de archivo a verificar.
 * @return 1 si el descriptor de archivo está cerrado, 0 en caso contrario.
 */
int is_closed(int fd);

/**
 * @brief Configura la información de los esclavos.
 *
 * @param slaves_info Puntero a la estructura de información de los esclavos.
 * @param slave_qty Cantidad de esclavos.
 * @param slot_size Tamaño del slot para cada esclavo.
 */
void setup_slaves(TSlaveInfo* slaves_info, int slave_qty, int slot_size);


#endif