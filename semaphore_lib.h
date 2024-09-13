#include <semaphore.h>

#ifndef SEMAPHORE_LIB_H
#define SEMAPHORE_LIB_H

#define SEM_NAME        "/app_view_semaphore"

/**
 * @brief Tipo de semáforo.
 */
typedef sem_t TSemaphore;


/**
 * @brief Crea un semáforo.
 *
 * @param name Nombre del semáforo.
 * @return Puntero al semáforo creado.
 */
TSemaphore* create_semaphore(char* name);

/**
 * @brief Obtiene un semáforo existente.
 *
 * @param name Nombre del semáforo.
 * @return Puntero al semáforo obtenido.
 */
TSemaphore* get_semaphore(char* name);

/**
 * @brief Espera en un semáforo.
 *
 * @param semaphore Puntero al semáforo.
 */
void wait_semaphore(TSemaphore* semaphore);

/**
 * @brief Postea un semáforo.
 *
 * @param semaphore Puntero al semáforo.
 */
void post_semaphore(TSemaphore* semaphore);

/**
 * @brief Cierra un semáforo.
 *
 * @param semaphore Puntero al semáforo.
 */
void close_semaphore(TSemaphore* semaphore);

/**
 * @brief Elimina un semáforo.
 *
 * @param name Nombre del semáforo.
 */
void delete_semaphore(char* name);
#endif