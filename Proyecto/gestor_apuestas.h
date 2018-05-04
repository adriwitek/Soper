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
#include "caballo.h"
#include "semaforos.h"

#define KEY_G 139  /*!<Random key value*/

typedef struct _apuesta {
  int apostador_id;             /*meterrrrrrr*/
  int ventanilla_id;
	int caballo_id;
	double cotizacion_caballo;
	double cantidad_apostada;
	double posible_beneficio;
} apuesta;


typedef struct _apostador{
	int id; //Quien lo ha postado
	char nombre[15];
	double total_apostado;
	int n_apuestas_realizadas;
	apuesta apuestas_realizadas[30]; // 30 seg por apuesta,cada segundo 1,MAX 30 apuestas
	double beneficios_obtenidos;
	double dinero_ganado;
	double saldo;
} apostador;

typedef struct _gestor_apuestas{
  int n_ventanillas;
  pthread_t * ventanillas;
  int ga_msqid;
  int * sem_ventanillas;//Zonas criticas de memoria,semaforo
  short carrera_comenzada;/*Boolean*/
  
  int n_apostadores;
  apostador apostadores[110]; //tamaño de 100
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

/*typedef struct _comunicacion_con_gestor {
    short caballos_iniciados;   //Boolean
    short carrera_comenzada;    //Boolean
    struct _caballos * caballos_creados;
    
}comunicacion_con_gestor;*/
 
int crear_ventanillas(struct _gestor_apuestas * g_apuestas,caballos* e_cab, int n_ventanillas,int n_apostadores, int msqid);
int ventanillas_abre_ventas(struct _gestor_apuestas * g_apuestas);
int ventanillas_cierra_ventas(struct _gestor_apuestas * g_apuestas);
void * ventanilla_atiende_clientes(void *argv);
void inicializa_apuestas(struct _gestor_apuestas * g_apuestas,caballos* e_cab);
void actualizar_cotizaciones_caballos(struct _gestor_apuestas * g_apuestas,caballos* e_cab);
int* get_top_10_apostadores(struct _gestor_apuestas * g_apuestas);
void liberar_gestor_apuestas(struct _gestor_apuestas * g_apuestas);
double get_total_apostado(struct _gestor_apuestas * g_apuestas);
apostador * get_apostador_by_id(struct _gestor_apuestas * g_apuestas,int id);


/***SETTERS***//*
void set_apuesta_ventanilla_id(apuesta * ap, int in){ apuesta->ventanilla_id=in;}
void set_apuesta_caballo_id(apuesta * ap, int in){ apuesta->caballo_id=in;}
void set_apuesta_cotizacion_caballo(apuesta * ap, double in){ apuesta->cotizacion_caballo=in;}
void set_apuesta_cantidad_apostada(apuesta * ap, double in){ apuesta->cantidad_apostada=in;}
void set_apuesta_posible_beneficio(apuesta * ap, double in){ apuesta->posible_beneficio=in;}

void set_apostador_id(apostador * ap, int in){ apostador->id=in;}
void set_apostador_nombre(apostador * ap, char in[]){ strcpy(apostador->nombre,in);}
void set_apostador_total_apostado(apostador * ap, double in){ apostador->total_apostado=in;}
//set_apostador_n_apuestas_realizadas(apostador ap, in){ apostador->n_apuestas_realizadas=in;}
void set_apostador_apuestas_realizadas(apostador * ap, apuesta in){ apostador->apuestas_realizadas[n_apuestas_realizadas]=in; n_apuestas_realizadas ++;}
void set_apostador_beneficios_obtenidos(apostador * ap, double in){ apostador->beneficios_obtenidos=in;}
void set_apostador_dinero_ganado(apostador * ap,  double in){ apostador->dinero_ganado=in;}*/
/*void set_apostador_saldo(apostador * apostador,  double saldo);
void set_gestor_apuestas_apostador_saldo(gestor_apuestas * ga,int id,  double saldo);

void set_gestor_apuestas_n_ventanillas(gestor_apuestas * ga, int in);*/
//set_gestor_apuestas_ventanillas(gestor_apuestas * ga, )  pthread_t * ventanillas;*/

//void set_gestor_apuestas_msqid(gestor_apuestas * ga, int in);/*

/**//***FIN SETTERS***/


/***GETTERS***//*
int get_apuesta_ventanilla_id(apuesta ap){return ap->ventanilla_id; }
int get_apuesta_caballo_id(apuesta ap){return ap->caballo_id; }
double get_apuesta_cotizacion_caballo(apuesta ap){return ap->cotizacion_caballo; }
double get_apuesta_cantidad_apostada(apuesta ap){return ap->cantidad_apostada; }
double  get_apuesta_posible_beneficio(apuesta ap){return ap->posible_beneficio; }

int get_apostador_id(apostador ap){ return ap->id;}
char get_apostador_nombre(apostador ap){ return ap->nombre;}
double get_apostador_total_apostado(apostador ap){ return ap->total_apostado;}
int get_apostador_n_apuestas_realizadas(apostador ap){ return ap->n_apuestas_realizadas;}
apuesta get_apostador_apuestas_realizadas(apostador ap){ return ap->apuestas_realizadas;}
double get_apostador_beneficios_obtenidos(apostador ap){ return ap->beneficios_obtenidos;}
double get_apostador_dinero_ganado(apostador ap){ return ap->dinero_ganado;}*/
//double get_apostador_saldo(apostador ap);
//double get_gestor_apuestas_apostador_saldo(gestor_apuestas ga, int i);
/*
int get_gestor_apuestas_n_ventanillas(gestor_apuestas ga){return ga->n_ventanillas;}
//set_gestor_apuestas_ventanillas(gestor_apuestas  ga, )  
double get_gestor_apuestas_msqid(gestor_apuestas ga){return ga->msqid;}
//set_gestor_apuestas_sem_ventanillas(gestor_apuestas  ga, int in){}    sem_ventanillas;//Zonas criticas de memoria,semaforo
short get_gestor_apuestas_carrera_comenzada(gestor_apuestas ga){return ga->carrera_comenzada;}
//set_gestor_apuestas_n_apostadores(gestor_apuestas  ga, int in)*/
//apostador get_gestor_apuestas_apostadores(gestor_apuestas ga){return ga->apostadores};


/*
int get_gestor_apuestas_total_apostado(gestor_apuestas ga){return ga->total_apostado;}
//set_gestor_apuestas_apuestas()
//apuesta get_gestor_apuestas_apuestas_realizadas(gestor_apuestas ga){return ga->apuestas_realizadas};*/
/**/



void set_gestor_apuestas_n_ventanillas(gestor_apuestas * ga, int in);
void set_apostador_saldo(apostador * apostador,  double saldo);
void set_gestor_apuestas_apostador_saldo(gestor_apuestas * ga,int id,  double saldo);
double get_apostador_saldo(apostador ap);
double get_gestor_apuestas_apostador_saldo(gestor_apuestas ga, int i);
//set_gestor_apuestas_sem_ventanillas(gestor_apuestas * ga, int in){}   * sem_ventanillas;//Zonas criticas de memoria,semaforo
void set_gestor_apuestas_carrera_comenzada(gestor_apuestas * ga, short in);
//set_gestor_apuestas_n_apostadores(gestor_apuestas * ga, int in)  int n_apostadores;
//void set_gestor_apuestas_(gestor_apuestas * ga, apostador ap);
void set_gestor_apuestas_total_apostado(gestor_apuestas * ga, int in);
//set_gestor_apuestas_apuestas()  int n_apuestas;
void set_gestor_apuestas_apuestas_realizadas(gestor_apuestas * ga, apuesta ap);


#endif