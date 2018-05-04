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
#include "caballo.h"
#include "semaforos.h"


typedef struct _Mensaje{ /*!< estructura mensaje*/
	long id;  /*!< id*/
	char contenido[3000];  /*!< contenido mensaje*/
}mensaje; /*!< mensaje*/


void inicializar_caballos(caballos caballos_creados){

	int i = 0;
	for(i=0;i<caballos_creados.num_caballos;i++){
		set_caballos_posicion(&caballos_creados,i,0);
		set_caballos_apostado(&caballos_creados,i,1.0);
		set_caballos_cotizacion(&caballos_creados,i,1.0);
		set_caballos_tirada(&caballos_creados,i,0);
		set_caballos_acumulado_tirada(&caballos_creados,i,0);
	}

}

/*FUNCIONES DE CABALLO*/
int get_caballo_id(caballo caballo_in){return caballo_in.id;}
int get_caballo_proceso_id(caballo caballo_in){return caballo_in.proceso_id;}
int get_caballo_posicion(caballo caballo_in){return caballo_in.posicion;}
int get_caballo_tirada(caballo caballo_in){return caballo_in.tirada;}
int get_caballo_acumulado_tirada(caballo caballo_in){return caballo_in.acumulado_tirada;}
double get_caballo_cotizacion(caballo caballo_in){return caballo_in.cotizacion;}
double get_caballo_apostado(caballo caballo_in){return caballo_in.apostado;}

void set_caballo_id(caballo *caballo_in, int i){caballo_in->id = i;}
void set_caballo_proceso_id(caballo *caballo_in, int i){caballo_in->proceso_id = i;}
void set_caballo_posicion(caballo *caballo_in, int i){caballo_in->posicion = i;}
void set_caballo_tirada(caballo *caballo_in, int i){caballo_in->tirada = i;}
void set_caballo_acumulado_tirada(caballo *caballo_in, int i){caballo_in->acumulado_tirada += i;}
void set_caballo_cotizacion(caballo *caballo_in, double i){caballo_in->cotizacion = i;}
void set_caballo_apostado(caballo *caballo_in, double i){caballo_in->apostado = i;}
/*FIN FUNCIONES DE CABALLO*/

/*FUNCIONES DE CABALLOS - LA ESTRUCTURA*/
int get_caballos_id(caballos caballos_creados, int id){return get_caballo_id(caballos_creados.caballos_registrados[id]);}
int get_caballos_proceso_id(caballos caballos_creados, int id){return get_caballo_proceso_id(caballos_creados.caballos_registrados[id]);}
int get_caballos_posicion(caballos caballos_creados, int id){return get_caballo_posicion(caballos_creados.caballos_registrados[id]);}
int get_caballos_tirada(caballos caballos_creados, int id){return get_caballo_tirada(caballos_creados.caballos_registrados[id]);}
int get_caballos_acumulado_tirada(caballos caballos_creados, int id){return get_caballo_acumulado_tirada(caballos_creados.caballos_registrados[id]);}
double get_caballos_cotizacion(caballos caballos_creados, int id){return get_caballo_cotizacion(caballos_creados.caballos_registrados[id]);}
double get_caballos_apostado(caballos caballos_creados, int id){return get_caballo_apostado(caballos_creados.caballos_registrados[id]);}
int get_caballos_total(caballos caballos_creados){return caballos_creados.total;}
int get_caballos_num_caballos(caballos caballos_creados){return caballos_creados.num_caballos;}

void set_caballos_id(caballos * caballos_creados, int id, int in){set_caballo_id(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_proceso_id(caballos * caballos_creados, int id, int in){set_caballo_proceso_id(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_posicion(caballos * caballos_creados, int id, int in){set_caballo_posicion(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_tirada(caballos * caballos_creados, int id, int in){set_caballo_tirada(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_acumulado_tirada(caballos * caballos_creados, int id, int in){set_caballo_acumulado_tirada(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_cotizacion(caballos * caballos_creados, int id, double in){set_caballo_cotizacion(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_apostado(caballos * caballos_creados, int id, double in){set_caballo_apostado(&caballos_creados->caballos_registrados[id], in);}
void set_caballos_total(caballos * caballos_creados, int in){caballos_creados->total=in;};
void set_caballos_num_caballos(caballos * caballos_creados, int in){caballos_creados->num_caballos=in;};
/*FIN FUNCIONES DE CABALLOS*/

int trabajo_caballos(int msqid_cola_msg, int msqid_caballos_imprimir, int * semaforon, int tuberias_padre_hijo[MAX_CABALLOS][2],
					 int id_local_caballo, int posicion_local_caballo, int imprimir_bonito, int num_caballos){

	char cadena_pipes[5]="";
	int i_aux = 0;
	mensaje msg;

	close(tuberias_padre_hijo[id_local_caballo][1]);
	read(tuberias_padre_hijo[id_local_caballo][0], cadena_pipes, sizeof(cadena_pipes));

	posicion_local_caballo = atoi(cadena_pipes);
	if(posicion_local_caballo >= 0){
		if(imprimir_bonito == 0){
			//printf("Caballo %d - Proceso principal me dice mi posicion: %d\n", id_local_caballo, posicion_local_caballo);
		} 
    	i_aux = determinar_tirada(posicion_local_caballo, num_caballos);

		memset(&msg.contenido[0],0,3000);
    	sprintf(msg.contenido,"%d-%d",id_local_caballo,i_aux);
		msg.id = id_local_caballo+1;
		msgsnd(msqid_cola_msg,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long), 0);

		if (imprimir_bonito == 1){
			gotoxy(40,50+id_local_caballo);
			//printf("Caballo %d pos:%d tira:%d\n", id_local_caballo, posicion_local_caballo,i_aux);
		}
		sleep(1);
		/*Semaforo numero 3 es de exclusion para la zona de syslog*/
		//Down_Semaforo(*semaforon, s_exclusion_syslog, SEM_UNDO);
		
		syslog( LOG_SYSLOG | LOG_INFO, "Caballo %d - hace una tirada %d", id_local_caballo, i_aux);
		
		//Up_Semaforo(*semaforon, s_exclusion_syslog, SEM_UNDO);
	}

	if(imprimir_bonito == 1){
		memset(&msg.contenido[0],0,3000);
		sprintf(msg.contenido,"%d",i_aux);
		msg.id = id_local_caballo+1;
		msgsnd(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long), IPC_NOWAIT);
		//Down_Semaforo(*semaforon, s_exclusion_syslog, SEM_UNDO);
		
		syslog( LOG_SYSLOG | LOG_INFO, "Caballo %d - envia su tirada(%d) por cola de mensajes", id_local_caballo, i_aux);
		
		//Up_Semaforo(*semaforon, s_exclusion_syslog, SEM_UNDO);
	}

	return i_aux;
}

int determinar_tirada(int posicion_in, int num_caballos){
	
	int ultimo = num_caballos-1;
	int primero = 0;
	int posicion = posicion_in;
	int retorno = 0;
	
	if(posicion == ultimo ){
		retorno = (aleat_num(1,6,posicion)+aleat_num(1,6,0));
	} else if (posicion == primero){
		retorno = aleat_num(1,7,posicion);
	} else {
		retorno = aleat_num(1,6,posicion);
	}

	return retorno;
}

/*FUNCION DE CARRERA QUE FUNCIONA CON EL MODULO DE IMPRESION BONITA*/

void recorrer_pista_i(int msqid_caballos_imprimir, int * semaforon, int longitud, int tuberias_padre_hijo[MAX_CABALLOS][2], 
					  int id_local_caballo, int posicion_local_caballo, int imprimir_bonito, int num_caballos){

	int posicion_X = ini_X, posicion_Y = 4+id_local_caballo*2;
	int j = 0;
	float tiempo = 1000000.0;
	int tirada_realizada = 1;
	mensaje msg;

	switch(id_local_caballo){
		case 0:
			for(j=0;j<=LONGITUD_CARRIL_0;j++){
				
				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);

				/*Semaforo numero 3 es de exclusion para la zona de syslog*/

				if(posicion_X<=119 && posicion_Y == 4){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X >  119 && posicion_X <= 131 && posicion_Y >=  4 && posicion_Y <= 16){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 132 &&                      posicion_Y>=  16 && posicion_Y <= 34 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >  119 && posicion_X <= 132 && posicion_Y >= 35 && posicion_Y <  46){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  20 && posicion_X <= 132 && posicion_Y == 46){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X >=   7 && posicion_X <=  20 && posicion_Y <= 46 && posicion_Y >= 34 ){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==   6 &&                      posicion_Y >= 18 && posicion_Y <= 34 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=   6 && posicion_X <=  20 && posicion_Y >=  4 && posicion_Y <= 17){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  20 && posicion_X <= 119 && posicion_Y ==  4){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		case 1:
			for(j=0;j<=LONGITUD_CARRIL_1;j++){

				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_1/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);
/*Semaforo numero 3 es de exclusion para la zona de syslog*/

				if(posicion_X<=118 && posicion_Y == 6){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X >  118 && posicion_X <= 128 && posicion_Y >=  6 && posicion_Y <= 16){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 129 &&                      posicion_Y >= 16 && posicion_Y <= 33 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >  118 && posicion_X <= 129 && posicion_Y >= 34 && posicion_Y <  44){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  21 && posicion_X <= 129 && posicion_Y == 44){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X >=   7 && posicion_X <=  21 && posicion_Y <= 46 && posicion_Y >= 34){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==   9 &&                      posicion_Y >= 18 && posicion_Y <= 34 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=   9 && posicion_X <=  21 && posicion_Y >=  6 && posicion_Y <= 17){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  21 && posicion_X <= 118 && posicion_Y ==  6){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		case 2:
			for(j=0;j<=LONGITUD_CARRIL_2;j++){
				
				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_2/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);

				if(posicion_X<=116 && posicion_Y == 8){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X >  116 && posicion_X <= 126 && posicion_Y >=  8 && posicion_Y <= 16){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 126 &&                      posicion_Y >= 16 && posicion_Y <= 33 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >  116 && posicion_X <= 126 && posicion_Y >= 34 && posicion_Y <  42){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  23 && posicion_X <= 126 && posicion_Y == 42){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X >=  13 && posicion_X <=  23 && posicion_Y <= 46 && posicion_Y >= 34 ){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  13 &&                      posicion_Y >= 18 && posicion_Y <= 33 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=  13 && posicion_X <=  23 && posicion_Y >=  4 && posicion_Y <= 17){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  23 && posicion_X <= 116 && posicion_Y ==  4){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		case 3:
			for(j=0;j<=LONGITUD_CARRIL_3;j++){
				
				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_3/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);


				if(posicion_X<=115 && posicion_Y == 10){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X >  115 && posicion_X <= 122 && posicion_Y >= 10 && posicion_Y <= 16){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 123 &&                      posicion_Y >= 17 && posicion_Y <= 32 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >  115 && posicion_X <= 123 && posicion_Y >= 32 && posicion_Y <  40){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  24 && posicion_X <= 123 && posicion_Y == 40){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X >=  16 && posicion_X <=  24 && posicion_Y <= 40 && posicion_Y >= 32){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  15 &&                      posicion_Y >= 19 && posicion_Y <= 32 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=  15 && posicion_X <=  24 && posicion_Y >= 10 && posicion_Y <= 19){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  24 && posicion_X <= 115 && posicion_Y == 10){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		case 4:
			for(j=0;j<=LONGITUD_CARRIL_4;j++){
				
				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_4/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);

				
				if(posicion_X<=113 && posicion_Y == 12){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X >  113 && posicion_X <= 119 && posicion_Y >= 12 && posicion_Y <= 17){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 120 &&                      posicion_Y >= 18 && posicion_Y <= 32 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >  113 && posicion_X <= 120 && posicion_Y >= 32 && posicion_Y <  38){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  26 && posicion_X <= 120 && posicion_Y == 38){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X >=  17 && posicion_X <=  26 && posicion_Y <= 38 && posicion_Y >= 32){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  18 &&                      posicion_Y >= 20 && posicion_Y <= 33 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=  18 && posicion_X <=  26 && posicion_Y >= 12 && posicion_Y <= 20){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  26 && posicion_X <= 113 && posicion_Y == 12){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		case 5:
			for(j=0;j<=LONGITUD_CARRIL_5;j++){
			
				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_5/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);


				if(posicion_X<=112 && posicion_Y == 14){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X >  112 && posicion_X <= 117 && posicion_Y >= 14 && posicion_Y <= 18){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 118 &&                      posicion_Y >= 19 && posicion_Y <= 31 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >  112 && posicion_X <= 118 && posicion_Y >= 31 && posicion_Y <  36){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  26 && posicion_X <= 118 && posicion_Y == 36){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X >=  18 && posicion_X <=  26 && posicion_Y <= 36 && posicion_Y >= 33){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  21 &&                      posicion_Y >= 20 && posicion_Y <= 32 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=  19 && posicion_X <=  26 && posicion_Y >= 14 && posicion_Y <= 19){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  26 && posicion_X <= 112 && posicion_Y == 14){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		case 6:
			for(j=0;j<=LONGITUD_CARRIL_6;j++){
				
				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_6/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);


				if(posicion_X<=110 && posicion_Y == 16){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X >  110 && posicion_X <= 114 && posicion_Y >= 16 && posicion_Y <= 19){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 115 &&                      posicion_Y >= 19 && posicion_Y <= 30 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >  110 && posicion_X <= 115 && posicion_Y >= 30 && posicion_Y <  34){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  28 && posicion_X <= 115 && posicion_Y == 34){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X >=  19 && posicion_X <=  28 && posicion_Y <= 34 && posicion_Y >= 32){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  24 &&                      posicion_Y >= 21 && posicion_Y <= 33){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=  19 && posicion_X <=  28 && posicion_Y >= 16 && posicion_Y <= 20){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  28 && posicion_X <= 110 && posicion_Y == 16){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		case 7:
			for(j=0;j<=LONGITUD_CARRIL_7;j++){

				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_7/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);


				if(posicion_X<=108 && posicion_Y == 18){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X >=  108 && posicion_X <= 111 && posicion_Y >= 18 && posicion_Y <= 19){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 111 &&                      posicion_Y >= 20 && posicion_Y <= 29 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >  108 && posicion_X <= 115 && posicion_Y >= 29 && posicion_Y <  32){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  30 && posicion_X <= 115 && posicion_Y == 32){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X >=  19 && posicion_X <=  30 && posicion_Y <= 32 && posicion_Y >= 31){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  27 &&                      posicion_Y >= 21 && posicion_Y <= 30 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=  19 && posicion_X <=  28 && posicion_Y >= 18 && posicion_Y <= 20){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  26 && posicion_X <= 108 && posicion_Y == 18){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		case 8:
			for(j=0;j<=LONGITUD_CARRIL_8;j++){

				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_8/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);


				if(posicion_X<=107 && posicion_Y == 20){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X >=  107 && posicion_X <= 109 && posicion_Y >= 20 && posicion_Y <= 19){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 108 &&                      posicion_Y >= 19 && posicion_Y <= 28 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >  107 && posicion_X <= 108 && posicion_Y >= 28 && posicion_Y <  30){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  33 && posicion_X <= 107 && posicion_Y == 30){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X >=  31 && posicion_X <=  33 && posicion_Y <= 30 && posicion_Y >= 28){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  30 &&                      posicion_Y >= 20 && posicion_Y <= 32 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=  19 && posicion_X <=  26 && posicion_Y >= 20 && posicion_Y <= 19){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  26 && posicion_X <= 107 && posicion_Y == 20){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		case 9:
			for(j=0;j<=LONGITUD_CARRIL_9;j++){

				memset(&msg.contenido[0],0,3000);
				msgrcv(msqid_caballos_imprimir,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),id_local_caballo+1, IPC_NOWAIT);
				tirada_realizada=atoi(msg.contenido);
				if(tirada_realizada != 0){
					
					syslog( LOG_SYSLOG | LOG_INFO, "Subdito de caballo %d - lee la tirada (%d) - espera %lf us",
					 id_local_caballo, tirada_realizada, tiempo);
					
					tiempo = (float)(1/((float)tirada_realizada*(float)((float)LONGITUD_CARRIL_9/(float)longitud)));
					tiempo *= ESPERA_CABALLO;
				}

				usleep(tiempo);


				if(posicion_X<=104 && posicion_Y == 22){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				} else if (posicion_X == 105 &&                      posicion_Y >= 22 && posicion_Y <= 27 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", id_local_caballo);
					posicion_Y++;
				} else if (posicion_X >=  35 && posicion_X <= 105 && posicion_Y == 28){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X--;
				} else if (posicion_X ==  34 &&                      posicion_Y >= 22 && posicion_Y <= 28 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", id_local_caballo);
					posicion_Y--;
				} else if (posicion_X >=  34 && posicion_X <= 105 && posicion_Y == 22){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", id_local_caballo);
					posicion_X++;
				}
			}
			break;
		default:
			break;
			
	}

}