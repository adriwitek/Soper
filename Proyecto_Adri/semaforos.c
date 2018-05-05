/** @file semaforos.c
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


/**
 * @brief  Inicializar_Semaforo. Inicializa los semaforos indicados.
 * @param semid Identificador del semaforo.
 * @param array Valores iniciales.   OJO!!! El valor 0 del array indicara los semaforos que hay
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int Inicializar_Semaforo(int semid, unsigned short *array) {

    int i;

     union semun {
        int val;
        struct semid_ds *semstat;
        unsigned short *array;
    } arg;
     
    
    if(array == NULL){
        return ERROR;
    }
    
    arg.array = (unsigned short *) malloc(array[0]*sizeof(short));
    if(arg.array == NULL){
        return ERROR;
    }
    
    
     for(i = 0; i<array[0]; i++){
      arg.array[i] = ( unsigned short)array[i+1]; /*array[0] indica cuantos semaforos son,y por tanto,el numero de valores del array a asignar*/
     }   
     
     if(semctl(semid, array[0], SETALL, arg) == ERROR  ){
          free(arg.array);
         return ERROR;
  }
    free(arg.array);
    return OK;
}
 
/**
 * @brief  Borrar_Semaforo. Descripcion: Borra un semaforo.
 * @param semid Identificador del semaforo.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int Borrar_Semaforo(int semid) {
      if(semctl(semid, 0,IPC_RMID) == ERROR) {
        return ERROR;
    }
    
    return OK;
}
/**
 * @brief  Crear_Semaforo. Descripcion: Crea un semaforo con la clave y el tamaño especificado. Lo inicializa a 0.
 * @param key Clave precompartida del semaforo.
 * @param size Tamaño del semaforo.
 * @param semid Identificador del semaforo.
 * @return ERROR en caso de error, 0 si ha creado el semaforo, 1 si ya estaba creado.
*/
int Crear_Semaforo(key_t key, int size, int *semid) {

    if (size<=0 || semid == NULL) {
        return ERROR;
    }

    *semid = semget((int)key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    
   if((*semid == - 1) && (errno == EEXIST)){
        return ERROR;
   }
    return OK;
        
}
/**
 * @brief Down_Semaforo Baja el semaforo indicado
 * @param id Identificador del semaforo.
 * @param num_sem Semaforo dentro del array.
 * @param undo Flag de modo persistente pese a finalización abrupta.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int Down_Semaforo(int id, int num_sem, int undo) {
    struct sembuf sem_oper;
    
    sem_oper.sem_num = num_sem;
    sem_oper.sem_op =  -1;
    sem_oper.sem_flg = undo;
 
    if ( semop(id, &sem_oper, 1) == ERROR) {
        return ERROR;
    }
    return OK;
}
/**
 * @brief DownMultiple_Semaforo Descripcion: Baja todos los semaforos del array indicado por active.
 * @param id Identificador del semaforo.
 * @param size Numero de semaforos del array.
 * @param undo Flag de modo persistente pese a finalización abrupta.
 * @param active Semaforos involucrados.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int DownMultiple_Semaforo(int id, int size, int undo, int *active) {

     int i = 0;
     struct sembuf *sem_oper;
 
    if (NULL == active || size<=0) {
        return ERROR;
    }
 
   sem_oper = (struct sembuf *) malloc(sizeof(struct sembuf)*size);
    if(sem_oper == NULL){
        return ERROR;
    }
 
 
    for (i = 0; i < size; i++) {
        sem_oper[i].sem_op = sem_oper[i].sem_op -1;
        sem_oper[i].sem_flg = undo;
        sem_oper[i].sem_num = active[i];
    }
 
    if ( semop(id, sem_oper, size) == ERROR) {
        return ERROR;
    }
 
    return OK;
    free(sem_oper);


}
/**
 * @brief Up_Semaforo Descripcion: Sube el semaforo indicado
 * @param id Identificador del semaforo.
 * @param num_sem Semaforo dentro del array.
 * @param undo Flag de modo persistente pese a finalizacion abupta.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int Up_Semaforo(int id, int num_sem, int undo) {
    struct sembuf sem_oper;
    
    sem_oper.sem_num = num_sem;
    sem_oper.sem_op = 1;
    sem_oper.sem_flg = undo;
 
    if (semop(id, &sem_oper, 1) == ERROR) {
        return ERROR;
    }
    return OK;
}
/**
 * @brief  UpMultiple_Semaforo Descripcion: Sube todos los semaforos del array indicado por active.
 * @param id Identificador del semaforo.
 * @param size Numero de semaforos del array.
 * @param undo Flag de modo persistente pese a finalización abrupta.
 * @param active Semaforos involucrados.
 * @return OK si todo fue correcto, ERROR en caso de error.
*/
int UpMultiple_Semaforo(int id, int size, int undo, int *active) {
 
   int i = 0;
    struct sembuf *sem_oper;
 
    if (NULL == active || size<=0) {
        return ERROR;
    }
 
   sem_oper = (struct sembuf *) malloc(sizeof(struct sembuf)*size);
    if(sem_oper == NULL){
        return ERROR;
    }
 
 
    for (i = 0; i < size; i++) {
        sem_oper[i].sem_op = sem_oper[i].sem_op +1;
        sem_oper[i].sem_flg = undo;
        sem_oper[i].sem_num = active[i];
    }
 
    if ( semop(id, sem_oper, size) == ERROR) {
        return ERROR;
    }
    free(sem_oper);
    return OK;


}
