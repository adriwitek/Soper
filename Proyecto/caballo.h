#ifndef caballo_h 
#define caballo_h 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <wait.h>

/* Para usar fstat */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <syslog.h>

#include "imprimir.h"
#include "utilidades.h"


#define ESPERA_CABALLO 1000000
#define MAX_CABALLOS 10
#define LONGITUD_CARRIL_0 286
#define LONGITUD_CARRIL_1 273
#define LONGITUD_CARRIL_2 258
#define LONGITUD_CARRIL_3 245
#define LONGITUD_CARRIL_4 230
#define LONGITUD_CARRIL_5 219
#define LONGITUD_CARRIL_6 203
#define LONGITUD_CARRIL_7 187
#define LONGITUD_CARRIL_8 172
#define LONGITUD_CARRIL_9 153

#define ini_X 70

enum _e_sem_comunes {
    s_sincro_1 = 1, s_sincro_2, s_sincro_principal_gestor_1 , s_sincro_principal_gestor_2, s_exclusion_syslog, s_sincro_monitorPrin_1 , s_sincro_monitorPrin_2
}e_sem_comunes;


typedef struct _podio{
	int * id;
	int * posicion;
} podio;

typedef struct _caballo{
	int id;
	int proceso_id;
	int posicion;
	int tirada;
	int acumulado_tirada;
	float cotizacion;
	float apostado;
} caballo;

typedef struct _caballos{
	int total;
	int num_caballos;
	caballo caballos_registrados[MAX_CABALLOS];
	//podio * podio_caballos;
} caballos;

void inicializar_caballos(caballos caballos_creados);


/*FUNCIONES CABALLO*/
int get_caballo_id(caballo caballo_in);
int get_caballo_proceso_id(caballo caballo_in);
int get_caballo_posicion(caballo caballo_in);
int get_caballo_tirada(caballo caballo_in);
int get_caballo_acumulado_tirada(caballo caballo_in);
double get_caballo_cotizacion(caballo caballo_in);
double get_caballo_apostado(caballo caballo_in);

void set_caballo_id(caballo * caballo_in, int i);
void set_caballo_proceso_id(caballo * caballo_in, int i);
void set_caballo_posicion(caballo * caballo_in, int i);
void set_caballo_tirada(caballo * caballo_in, int i);
void set_caballo_acumulado_tirada(caballo * caballo_in, int i);
void set_caballo_cotizacion(caballo * caballo_in, double i);
void set_caballo_apostado(caballo * caballo_in, double i);
/*FIN FUNCIONES CABALLO*/


/*FUNCIONES DE CABALLOS - LA ESTRUCTURA*/
int get_caballos_id(caballos caballos_creados, int id);
int get_caballos_proceso_id(caballos caballos_creados, int id);
int get_caballos_posicion(caballos caballos_creados, int id);
int get_caballos_tirada(caballos caballos_creados, int id);
int get_caballos_acumulado_tirada(caballos caballos_creados, int id);
double get_caballos_cotizacion(caballos caballos_creados, int id);
double get_caballos_apostado(caballos caballos_creados, int id);
int get_caballos_total(caballos caballos_creados);
int get_caballos_num_caballos(caballos caballos_creados);

void set_caballos_id(caballos * caballos_creados, int id, int in);
void set_caballos_proceso_id(caballos * caballos_creados, int id, int in);
void set_caballos_posicion(caballos * caballos_creados, int id, int in);
void set_caballos_tirada(caballos * caballos_creados, int id, int in);
void set_caballos_acumulado_tirada(caballos * caballos_creados, int id, int in);
void set_caballos_cotizacion(caballos * caballos_creados, int id, double in);
void set_caballos_apostado(caballos * caballos_creados, int id, double in);
void set_caballos_total(caballos * caballos_creados, int in);
void set_caballos_num_caballos(caballos * caballos_creados, int in);
/*FIN FUNCIONES DE CABALLOS*/

int trabajo_caballos(int msqid_cola_msg, int msqid_caballos_imprimir, int * semaforon, int tuberias_padre_hijo[MAX_CABALLOS][2], int id_local_caballo, int posicion_local_caballo, int imprimir_bonito, int num_caballos);

int determinar_tirada(int posicion_in, int num_caballos);

/*FUNCION DE IMPRIMRIR CABALLOS CON IMPRESION BONITA*/
void recorrer_pista_i(int msqid_caballos_imprimir, int * semaforon, int longitud, int tuberias_padre_hijo[MAX_CABALLOS][2],  
					  int id_local_caballo, int posicion_local_caballo, int imprimir_bonito, int num_caballos);


#endif