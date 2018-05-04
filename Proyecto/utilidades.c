/** @file utilidades.c
 *  @brief utilidades
 *
 *  @date 17/04/2018
 *  @author Daniel
 *  @author Adrian
 *  @bug No known bugs.
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>

#include "semaforos.h"
/**
* @brief genera un numero aleatorio entre dos margenes
*
* @param inf margen inferior
* @param sup margen superior
* @param id id que cambia la seed para rand
* @return el numero generado o -1 en caso de -1or
*/
int aleat_num(int inf, int sup, int id){
  int num_ale;
  if(inf > sup){
  	printf("Aleat_NUM: inferior tiene que se mas pequeño que superior\n");
  	return -1;
  }
  //srand(id);
  num_ale = ((rand()/(RAND_MAX + 1.) ) * (sup - inf + 1)) + inf;

  return num_ale;
  
}


/**
 * @brief reemplaza un caracter indicado por otro en una cadena
 *
 * @param str cadena a cambiar
 * @param find caracter a buscar
 * @param replace caracter por el que cambiar fin
 * @return cadena cambiada
 *
*/
char * replace_char(char* str, char find, char replace){

    char *current_pos = strchr(str,find);
    while (current_pos){
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}

void registrar_syslog(int * semaforon, int id_semaforo, char * cadena){
    Down_Semaforo(*semaforon, 3, SEM_UNDO);
    openlog( NULL, LOG_PID, LOG_SYSLOG);
    syslog( LOG_SYSLOG | LOG_INFO, "%s", cadena);
    closelog();
    Up_Semaforo(*semaforon, 3, SEM_UNDO);
}