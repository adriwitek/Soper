#ifndef utilidades_h 
#define utilidades_h 

/** @file utilidades.h
 *  @brief utilidades
 *
 *  @date 17/04/2018
 *  @author Daniel
 *  @author Adrian
 *  @bug No known bugs.
 */

/**
* @brief genera un numero aleatorio entre dos margenes
*
* @param inf margen inferior
* @param sup margen superior
* @param id id que cambia la seed para rand
* @return el numero generado o -1 en caso de error
*/
int aleat_num(int inf, int sup, int id);

/**
 * @brief reemplaza un caracter indicado por otro en una cadena
 *
 * @param str cadena a cambiar
 * @param find caracter a buscar
 * @param replace caracter por el que cambiar fin
 * @return cadena cambiada
 *
*/
char * replace_char(char* str, char find, char replace);

void registrar_syslog(int * semaforon, int id_semaforo, char * cadena);

#endif