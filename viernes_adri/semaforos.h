#ifndef semaforos_h 
#define semaforos_h 
/** @file semaforos.h
*   @brief semaforos
*
*   @date 17/04/2019
*   @author Daniel
*   @author Adrian
*   @bug No known bugs   
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define OK 1 /*!<Flag de OK es 1*/
#define ERROR -1 /*!<Flag de ERROR es -1*/


/***************************************************************
 * @brief  Inicializar_Semaforo. Inicializa los semaforos indicados.
 * @semid Identificador del semaforo.
 * @array Valores iniciales.   OJO!!! El valor 0 del array indicara los semaforos que hay
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int Inicializar_Semaforo(int semid, unsigned short *array);
 
/***************************************************************
 * @brief  Borrar_Semaforo. Descripcion: Borra un semaforo.
 * @semid Identificador del semaforo.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int Borrar_Semaforo(int semid);
/***************************************************************
 * @brief  Crear_Semaforo. Descripcion: Crea un semaforo con la clave y el tamaño especificado. Lo inicializa a 0.
 * @key Clave precompartida del semaforo.
 * @size Tamaño del semaforo.
 * @*semid Identificador del semaforo.
 int: ERROR en caso de error,
 0 si ha creado el semaforo,
 1 si ya estaba creado.
*/
int Crear_Semaforo(key_t key, int size, int *semid);
/**************************************************************
Nombre:Down_Semaforo Baja el semaforo indicado
 * @id Identificador del semaforo.
 * @num_sem Semaforo dentro del array.
 * @undo Flag de modo persistente pese a finalización abrupta.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int Down_Semaforo(int id, int num_sem, int undo);
/***************************************************************
 * @brief  DownMultiple_Semaforo Descripcion: Baja todos los semaforos del array indicado por active.
 * @semid Identificador del semaforo.
 * @size Numero de semaforos del array.
 * @undo Flag de modo persistente pese a finalización abrupta.
 * @active Semaforos involucrados.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int DownMultiple_Semaforo(int id, int size, int undo, int *active);
/**************************************************************
 * @brief Up_Semaforo Descripcion: Sube el semaforo indicado
 * @param semid Identificador del semaforo.
 * @param num_sem Semaforo dentro del array.
 * @param undo Flag de modo persistente pese a finalizacion abupta.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int Up_Semaforo(int id, int num_sem, int undo);
/***************************************************************
 * @brief  UpMultiple_Semaforo Descripcion: Sube todos los semaforos del array indicado por active.
 * @param semid Identificador del semaforo.
 * @param size Numero de semaforos del array.
 * @param undo Flag de modo persistente pese a finalización abrupta.
 * @param *active Semaforos involucrados.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int UpMultiple_Semaforo(int id, int size, int undo, int *active);


#endif