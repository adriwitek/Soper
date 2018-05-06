#ifndef gestor_apuestas_h 
#define gestor_apuestas_h 

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/msg.h>
#include <string.h>
#include "semaforos.h"
#include "caballo.h"

#define KEY_G 139  /*!<Random key value*/
#define MAX_APOSTADORES 100


typedef struct _Mensaje{ /*!< estructura mensaje*/
  long id;  /*!< id*/
  char contenido[3000];  /*!< contenido mensaje*/
}mensaje; /*!< mensaje*/


typedef struct _apuesta {
  int apostador_id;
  int ventanilla_id;
	int caballo_id;
	double cotizacion_caballo;
	double cantidad_apostada;
	double posible_beneficio;
} apuesta;


typedef struct _apostador{
	int id;
	char nombre[15];
	double total_apostado;
	int n_apuestas_realizadas;
	apuesta apuestas_realizadas[35]; 
	double beneficios_obtenidos;
	double dinero_ganado;
  double saldo;
} apostador;

typedef struct _gestor_apuestas{
  int n_ventanillas;
  int ventanillas_id[MAX_APOSTADORES+10];
  int ga_msqid; // id para mandar apuestas
  int sem_ventanillas;//Zonas criticas de memoria,semaforo
  short carrera_comenzada;/*Boolean*/
  
  int general_msqid; // id de cola general

  int n_apostadores;
  apostador apostadores[MAX_APOSTADORES+10]; //tamaño de 100
  
  int total_apostado;
  int n_apuestas;
  apuesta apuestas_realizadas[3500]; //Total donde se almacen todas las apuestas tamaño de 3000 = MAX APOSTADORES * 30 seg
}gestor_apuestas;


typedef struct _Mensaje_Ventanilla{ /*!< estructura mensaje*/
	long id;  /*!< id*/ /*Tipo de mensaje*/
	char nombre_apostador[3000];  /*!< nombre del apostador*/
  int id_caballo; 
  double dinero_apuesta;
}mensaje_ventanilla; /*!< mensaje*/

typedef struct _estructura_memoria_compartida{
  int shmid;
  int semaforo_id; //exclusion
  int sem_ventanillas;// sincro apostadores y ventanillas
  int msqid; // id de cola general
  int msqid_apuestas; // id de cola apuestas
  int carrera_comenzada;
  caballos caballos_creados;
  gestor_apuestas g_apuestas;
}estructura_memoria_compartida;
 
void * ventanilla_atiende_clientes(void *argv);

int crear_ventanillas(struct _gestor_apuestas * g_apuestas,caballos* e_cab, int msqid);
int ventanillas_abre_ventas(struct _gestor_apuestas * g_apuestas);
int ventanillas_cierra_ventas(struct _gestor_apuestas * g_apuestas);
void actualizar_cotizaciones_caballos(struct _gestor_apuestas * g_apuestas,caballos* e_cab);
int* get_top_10_apostadores(struct _gestor_apuestas * g_apuestas);
void liberar_gestor_apuestas(struct _gestor_apuestas * g_apuestas);
double get_total_apostado(struct _gestor_apuestas * g_apuestas);
apostador * get_apostador_by_id(struct _gestor_apuestas * g_apuestas,int id);





/***SETTERS***/
void set_gestor_apuestas_ga_msqid(gestor_apuestas * ga, int in);
/***FIN SETTERS***/


/***GETTERS***/


int get_gestor_apuestas_n_apuestas(gestor_apuestas ga);
int get_gestor_apuestas_n_apostadores(gestor_apuestas ga);
int get_gestor_apuestas_apostador_id(gestor_apuestas ga, int i);
int get_gestor_apuestas_ventanilla_id(gestor_apuestas ga, int i);
int get_gestor_apuestas_caballo_id(gestor_apuestas ga, int i);
double get_gestor_apuestas_cotizacion_caballo(gestor_apuestas ga, int i);
double get_gestor_apuestas_cantidad_apostada(gestor_apuestas ga, int i);
int get_gestor_apuestas_apostador_n_apuestas_realizadas(gestor_apuestas ga, int i);


/*GETTERS*/

int get_apuesta_apostador_id(apuesta ap);
int get_apuesta_ventanilla_id(apuesta ap);
int get_apuesta_caballo_id(apuesta ap);
double get_apuesta_cotizacion_caballo(apuesta ap);
double get_apuesta_cantidad_apostada(apuesta ap);
double get_apuesta_posible_beneficio(apuesta ap);

int get_apostador_id(apostador ap);
char * get_apostador_nombre(apostador ap);
double get_apostador_total_apostado(apostador ap);
int get_apostador_n_apuestas_realizadas(apostador ap);
apuesta get_apostador_apuestas_realizadas(apostador ap, int i);
double get_apostador_beneficios_obtenidos(apostador ap);
double get_apostador_dinero_ganado(apostador ap);
double get_apostador_saldo(apostador ap);


char * get_gestor_apuestas_apostador_nombre(gestor_apuestas ga, int i);

/*SETTERS*/

void set_apuesta_apostador_id(apuesta * ap, int in);
void set_apuesta_ventanilla_id(apuesta * ap, int in);
void set_apuesta_caballo_id(apuesta * ap, int in);
void set_apuesta_cotizacion_caballo(apuesta * ap, double in);
void set_apuesta_cantidad_apostada(apuesta * ap, double in);
void set_apuesta_posible_beneficio(apuesta * ap, double in);

void set_apostador_id(apostador * ap, int in);
void set_apostador_nombre(apostador * ap, char * in);
void set_apostador_total_apostado(apostador * ap, double in);
void set_apostador_n_apuestas_realizadas(apostador * ap, int in);
void set_apostador_apuestas_realizadas(apostador * ap, apuesta in);
void set_apostador_beneficios_obtenidos(apostador * ap, double in);
void set_apostador_dinero_ganado(apostador * ap, double in);
void set_apostador_saldo(apostador * ap, double in);


double get_gestor_apuestas_apostador_apuestas_realizadas_cantidad_apostada(gestor_apuestas g_apuestas,int i, int j);
double get_apostador_apuestas_realizadas_cantidad_apostada(apostador ap, int i);
double get_gestor_apuestas_apostador_saldo(gestor_apuestas ga, int i);
void set_gestor_apuestas_n_apostadores(gestor_apuestas * ga, int in);
void set_gestor_apuestas_n_ventanillas(gestor_apuestas * ga, int in);


void inicializar_apostadores(gestor_apuestas * g_apuestas, double saldo);
int get_gestor_apuestas_n_ventanillas(gestor_apuestas g_apuestas);
void set_gestor_apuestas_general_msqid(gestor_apuestas * g_apuestas, int in);


int get_gestor_apuestas_ga_msqid(gestor_apuestas ga);
int get_gestor_apuestas_general_msqid(gestor_apuestas ga);

void imprimir_apuesta(apuesta ap);

void set_gestor_apuestas__apostador_apuesta(gestor_apuestas * g_apuestas, int id, apuesta ap);
void set_gestor_apuestas_apuestas_realizadas(gestor_apuestas * g_apuestas, apuesta in);

int get_gestor_apuestas__i__apostador_id(gestor_apuestas g_apuestas, int i);
int get_gestor_apuestas__i__ventanilla_id(gestor_apuestas g_apuestas, int i);
int get_gestor_apuestas__i__caballo_id(gestor_apuestas g_apuestas, int i);
double get_gestor_apuestas__i__cantidad_apostada(gestor_apuestas g_apuestas, int i);
double get_gestor_apuestas__i__cotizacion_caballo(gestor_apuestas g_apuestas, int i);

void imprime_gestor_apuestas_apostador_apuestas(gestor_apuestas g_apuestas, int i);
void imprime_apuestas_apostador(apostador ap);

void set_ganancias_apostadores(gestor_apuestas * g_apuestas, int id_ganador);
void set_apostador_ganancias(apostador * ap, int id_ganador);
void set_gestor_apuestas_apostado(gestor_apuestas * g_apuestas, double in);
#endif