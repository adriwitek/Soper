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
#include "semaforos.h"
#include "utilidades.h"
#include "caballo.h"
#include "shared_memory.h"
#include "gestor_apuestas.h"


#define MAX_APOSTADORES 100
#define MAX_BUFF 2048
#define KEY 123

#define SH_KEY 1234
#define SH_KEY_GESTOR	15893	/*Random values*/
#define SH_KEY_MONITOR	45782

#define TAM_SH	200				/*Tamanio en bytes de la memoria cmpartida creada*/


typedef struct _Mensaje{ /*!< estructura mensaje*/
	long id;  /*!< id*/
	char contenido[3000];  /*!< contenido mensaje*/
}mensaje; /*!< mensaje*/

typedef struct _Pid{ /*!< estructura mensaje*/
	int num;  /*!< id*/
	int pids[100];
}pid_system; /*!< mensaje*/

struct _msg_apostador{ /*!< estructura info*/
	long id;
	char nombre[20]; /*!< nombre*/
	int numero_caballo;
	double apuesta;
}msg_apostador;

void imprimir_cuenta_atras(int imprimir_bonito);

void imprimir_caballos(caballos caballos_creados);
void imprimir_podio_caballos(caballos caballos_creados, int imprimir_bonito);

void imprimir_apostadores_fin(caballos caballos_creados);

void notificar_posicion_caballos_hp(int tuberias_hijo_padre[MAX_CABALLOS][2], caballos * caballos_creados);
void notificar_posicion_caballos_fin_hp(int tuberias_hijo_padre[MAX_CABALLOS][2], caballos * caballos_creados);
void actualizar_posiciones_caballos(caballos * caballos_creados);

void manejador_SIGINT();
void manejador_SIGUSR1();


int main (int argc, char ** argv){
	
	int i = 0, j = 0, pid = 0, flag = 0, i_contador = 0;

	mensaje msg;
	
	int posicion_local_caballo = 0;
	int id_local_caballo = 0;

	int tirada_realizada = 0;

	int numero_total_procesos_sinc = 0;

	//struct _caballos * caballos_creados;

	int tuberias_padre_hijo[MAX_CABALLOS][2];
	int tuberias_hijo_padre[MAX_CABALLOS][2];
	char cadena_pipes[20]="";

	sigset_t set_caballos, oset_caballos;

/*************************************************************************************************************/
	int n_ventanillas;
	int n_apostadores;
	
	struct _gestor_apuestas * g_apuestas;
	
	informacion * sh_gestor;  /*shared memory zone*/
	informacion * sh_monitor;
	int *sem_sh_gestor;		/*Semaforos para la meoria compartida*/
	int *sem_sh_monitor;	
	
	struct _comunicacion_con_gestor comunicacion_con_gestor;


	int establo_creado = 0;
	int carrera_comenzada = 0;
	struct _comunicacion_con_gestor *contenido;
	/////////////////////////////////////////////////////////////////////////////////////
	estructura_memoria_compartida * mem_compartida;

	int * semaforo;	
	unsigned short array_comun[2] = {1, 1}; /*1 semaforo,inicializados a 1*/
	int id = 0;
	/////////////////////////////////////////////////////////////////////////////////////


/*************************************************************************************************************/

	int * semaforon;
	unsigned short * array_cli = NULL;

	int shared_memory_p_to_a = -1;
	int shared_memory_p_to_m = -1;

	key_t clave_caballos; 
	int msqid_caballos; 
	mensaje msg_caballos;

	key_t clave_cola_msg; 
	int msqid_cola_msg; 
	mensaje msg_cola_msg;
	
	key_t clave_caballos_imprimir; 
	int msqid_caballos_imprimir; 
	mensaje msg_caballos_imprimir;

	char cadena_aux[MAX_BUFF]="";
	char cadena_aux2[MAX_BUFF]="";
	char *cadena_pt_aux=NULL;
	char *cadena_pt_aux2=NULL;	
	int i_aux = 0, i_aux2 = 0;
	int imprimir_activado = 0;
	int contador_caballos_fin = 0;

	int imprimir_bonito = 0;

	int flag_fin = 0;
	/* 
	 * Forma de llamar al programa
	 *
	 * ./main nCaballos longitudCarrera nApostadores nVentanillasApostar dineroMax
	 *
	 *
	 * nCaballos -> 1 a 10
	 * nApostadores -> 1-100
	 * 
	 *
	 */

	/* 
	 *
	 *  Ejecutar en tamaño de terminal
	 *	132 x 43 
	 *
	 */

	system("clear");
	srand(getpid());
	if (argc < 6){
		printf ("Error al invocar el programa, tienes que llamarlo:\n");
		printf("\t./main nCaballos longitudCarrera nApostadores nVentanillasApostar dineroMax \n");
		printf("\t nCaballos:\tnumero de caballos que correran, de 1 a 10\n");
		printf("\t longitudCarrera:\tlongitud de carrera para los caballos\n");
		printf("\t nApostadores:\tnumero de apostadores, de 1 a 100\n");
		printf("\t nVentanillasApostar:\tnumero de ventanillas para gestionar las apuestas\n");
		printf("\t dineroMax:\tcantidad de dinero por cada apostante\n");
		return -1;
	} else {
		if(atoi(argv[1]) > MAX_CABALLOS){
			printf("Error, introduce menos de 10 caballos\n");
			return -1;
		} else if(atoi(argv[3]) > MAX_APOSTADORES) {
			printf("Error, introduce menos de 100 apostadores\n");
			return -1;
		}

		printf("Quieres activar el imprimir bonito? (0-No 1-Si)\n");
		scanf("%d",&imprimir_bonito);
		//imprimir_bonito = 0;
		if(imprimir_bonito == 1){
			printf("Imprimir bonito activado\n");
			sleep(2);//clear
			imprimir_plantilla();
			fflush(stdout);
			
		}
		
		numero_total_procesos_sinc = 0;
		numero_total_procesos_sinc = atoi(argv[1]) + 1;
		n_ventanillas = atoi(argv[4]);
		n_apostadores = atoi(argv[3]);

		gotoxy(2,2);
		printf("Se han introducido un total de:\n");
		printf("\t\t%d caballos para correr\n", atoi(argv[1]));
		printf("\t\t%d longitud que recorrer\n", atoi(argv[2]));
		printf("\t\t%d apostadores\n", atoi(argv[3]));
		printf("\t\t%d ventanillas para apostar\n", atoi(argv[4]));
		printf("\t\t%d dinero maximo a apostar\n", atoi(argv[5]));
	}

	sleep(2);
	
	/*FASE DE INICIACION DE PROGRAMA*/


	/*************************************************************************************/

	/*
		Declaracion de inicializacion de memoria compartida, esta cotiene todas las estructuras del programa
		Contiene ademas el semaforo de exclusion para poder acceder a sus datos, 
			cuando se refiera al semaforo en un down y un up, ojo con los contenios y/o direcciones
	*/

	semaforo = (int*) malloc(sizeof (int)); 
    if (semaforo == NULL) {printf("Linea %d - Error al reservar memoria\n", __LINE__);}
    if (-1 == Crear_Semaforo(IPC_PRIVATE, 1, semaforo)) {printf("Linea %d - Error al crear el semaforo\n", __LINE__);return -1;}
    if (Inicializar_Semaforo(*semaforo, array_comun) == -1) {printf("\n Linea %d - Error al inicializar el semaforo\n", __LINE__);return -1;} 

    if((id=shmget(SH_KEY,sizeof(struct _estructura_memoria_compartida),IPC_CREAT|IPC_EXCL|0660))==-1){
		if((id=shmget(SH_KEY,sizeof(struct _estructura_memoria_compartida),0))==-1){
			printf("Error al abrir el segmento\n");
		}
		mem_compartida = shmat (id, (char *)0, 0);
		if (mem_compartida == NULL) {
			return -1; 
		}
	}

	mem_compartida = shmat (id, (char *)0, 0);
	if (mem_compartida == NULL) {
		fprintf (stderr, "Error reserve shared memory \n");
		return -1; 
	}

	mem_compartida->semaforo_id = * semaforo;


	set_caballos_num_caballos(&mem_compartida->caballos_creados,atoi(argv[1]));
	inicializar_caballos(mem_compartida->caballos_creados);

	/*************************************************************************************/

    /****Array de semaforos 1***/
    semaforon = (int*) malloc(sizeof (int)); /* ID de la lista de los semáforos cajeros */
    if (semaforon == NULL) {
        printf("Linea %d - Error al reservar memoria\n", __LINE__);
    }
    array_cli = (unsigned short *) malloc(sizeof(e_sem_comunes) * sizeof (unsigned short));
    if (NULL == array_cli) {
        printf("Linea %d - Error al reservar memoria\n", __LINE__);
        return -1;
    }

    array_cli[0]=7;
    array_cli[1]=0;
    array_cli[2]=0;
    array_cli[3]=0;
    array_cli[4]=0;
    array_cli[5]=1;
    array_cli[6]=0;
    array_cli[7]=0;

    if (-1 == Crear_Semaforo(IPC_PRIVATE, array_cli[0], semaforon)) {
        printf("Linea %d - Error al crear el semaforo\n", __LINE__);
        return -1;
    }
    if (Inicializar_Semaforo(*semaforon, array_cli) == -1) {
        printf("\n Linea %d - Error al inicializar semaforo\n", __LINE__);
        return -1;
    }

    free(array_cli);

    /*************************************************************************************/

   /*	sh_gestor = (informacion *) crea_o_asocia_shm(SH_KEY_GESTOR,sem_sh_gestor,TAM_SH);
    if(sh_gestor == NULL){
        printf("\n Fallo con la memoria compartida del proceso gestor");
    }
    
    sh_monitor = (informacion *) crea_o_asocia_shm(SH_KEY_MONITOR,sem_sh_monitor,TAM_SH);
    if(sh_monitor == NULL){
        printf("\n Fallo con la memoria compartida del proceso monitor");
    }
*/
	/*Cola de mensajes*/
  	clave_cola_msg = ftok ("/bin/ls", KEY+1); 
	if (clave_cola_msg == (key_t) -1) { 
		perror("Error al obtener clave para cola mensajes\n"); 
		exit(EXIT_FAILURE); 
	} 

	msqid_cola_msg = msgget (clave_cola_msg, 0600 | IPC_CREAT); 
	if (msqid_cola_msg == -1) { 
		perror("Error al obtener identificador para cola mensajes"); 
		return(0); 
	}



	/***********************FIN FASE DE INICIACION***********************/
	i=0;
	do {
		if(pid=fork()){
			syslog( LOG_SYSLOG | LOG_INFO, "Creado hijo %d", i);
			i++;
		} else {
			srand(getpid());
			flag = 1;
		}

	} while (i < 3 && flag == 0);

	if(i==0){
		/*PROCESO MONITOR*/

		/**************************************************************************************************/
		if((id=shmget(SH_KEY,sizeof(struct _estructura_memoria_compartida),IPC_CREAT|IPC_EXCL|0660))==-1){
			if((id=shmget(SH_KEY,sizeof(struct _estructura_memoria_compartida),0))==-1){
				printf("Error al abrir el segmento\n");
			}
			mem_compartida = shmat (id, (char *)0, 0);
			if (mem_compartida == NULL) {
				return -1; 
			}
		}

		mem_compartida = shmat (id, (char *)0, 0);
		if (mem_compartida == NULL) {
			fprintf (stderr, "Error reserve shared memory \n");
			return -1; 
		}
		/**************************************************************************************************/

		/* ANTES DE LA CARRERA */
		if(imprimir_bonito == 1){
			gotoxy(98,3);
		}
		printf("\tSoy el proceso monitor\n");
		if(imprimir_bonito == 1){
			gotoxy(98,4);
		}
		printf("\tSegundos para empezar la carrera: \n");
		imprimir_cuenta_atras(imprimir_bonito);
		if(imprimir_bonito == 1){
			system("clear");
			gotoxy(1,1);
			imprimir_plantilla();
		}

		Up_Semaforo(*semaforon, s_sincro_1, SEM_UNDO);
		Down_Semaforo(*semaforon, s_sincro_2, SEM_UNDO);

		if(imprimir_bonito == 1){
			gotoxy(67,7);
		}

		//printf("\tEstados de las apuestas:\n");

		/****************FIN FASE ANTES DE CARRERA******************/

		/*DURANTE LA CARRERA*/

		i_contador = 0;
		while(flag_fin==0){
			

			if(imprimir_bonito == 0){
				printf("---------------------------------------------------------RONDA %d---------------------------------------------\n", i_contador);
			}

			/*
				Leer todo el rato de memoria compartida, a ver si el proceso principal le ha dicho q es el final o no
				en caso de ser el final, paramos todos los procesos
			*/
			for(i=0;i<get_caballos_num_caballos(mem_compartida->caballos_creados);i++){				
				if(imprimir_bonito==0){
					printf("Caballo %d \t tirada %d \t posicion %d \t acumulado %d\n", 
						get_caballos_id(mem_compartida->caballos_creados,i), 	   get_caballos_tirada(mem_compartida->caballos_creados,i), 
						get_caballos_posicion(mem_compartida->caballos_creados,i), get_caballos_acumulado_tirada(mem_compartida->caballos_creados,i));
				} else {
					//imprimir_podio_caballos(*caballos_creados, imprimir_bonito);			
					//gotoxy(70,30+j);
					//printf("Caballo %d - acumulado %d\n", j, get_caballos_acumulado_tirada(mem_compartida->caballos_creados,j));
				}
				if( get_caballos_acumulado_tirada(mem_compartida->caballos_creados,j) >= atoi(argv[2])){
					if(imprimir_bonito == 0){
					//	printf ("HAY UN GANADOR - Caballo %d\n", i_aux);
					}
					contador_caballos_fin ++;
					if(contador_caballos_fin == get_caballos_num_caballos(mem_compartida->caballos_creados)){
						flag_fin = 1;
					}
				}
			}

			i_contador ++;

			/*SINCRONIZACION CON EL PROCESO PRINCIPAL, CADA TIRADA DE LOS CABALLOS*/
			Down_Semaforo(*semaforon, s_sincro_2, SEM_UNDO);
			Up_Semaforo(*semaforon, s_sincro_1, SEM_UNDO);
		}
		/****************FIN FASE DURANTE LA CARRERA******************/

		//	for(i=0;i<mem_compartida)

		/*FIN CARRERA*/

		/*
			Imprimir Listado de apuestas realizadas:
				- El apostador , ventanilla que gestiona la apuesta, el caballo, la cotizacion del caballo antes de la apuesta y la cantidad apostada
				(estas apuestas en orden, segun se efectuaron)
			Imprimir la posicion de los caballos, segun terminaron
				imprimir_podio_caballos(mem_compartida->caballos_creados,imprimir_bonito);

			Imprimir resultado de las apuestas:
				- Nombre del apostador, cantidad apostada, beneficios obtenidos y dinero restante
				for(i=0;i<)
		*/

		/****************FIN CARRERA******************/

		exit(EXIT_SUCCESS);
	} else if (i==1){	
		/*****************************************************************************************PROCESO GESTOR DE APUESTAS*******************************************************************************/

		/**************************************************************************************************/
		if((id=shmget(SH_KEY,sizeof(struct _estructura_memoria_compartida),IPC_CREAT|IPC_EXCL|0660))==-1){
			if((id=shmget(SH_KEY,sizeof(struct _estructura_memoria_compartida),0))==-1){
				printf("Error al abrir el segmento\n");
			}
			mem_compartida = shmat (id, (char *)0, 0);
			if (mem_compartida == NULL) {
				return -1; 
			}
		}

		mem_compartida = shmat (id, (char *)0, 0);
		if (mem_compartida == NULL) {
			fprintf (stderr, "Error reserve shared memory \n");
			return -1; 
		}
		/**************************************************************************************************/

		if(imprimir_bonito == 1){
			gotoxy(2,24);
		}

		printf("\tSoy el proceso gestor de apuestas\n");

		Down_Semaforo(*semaforon, s_sincro_principal_gestor_2, SEM_UNDO);
		Up_Semaforo(*semaforon, s_sincro_principal_gestor_1, SEM_UNDO);
		printf("Sincronizado gestor con principal\n");
		
		//contenido->caballos_creados = caballos_creados;


		/*LEER DE LA MEMORIA COMPARTIDA SI YA HA INICIADO EL PUNTERO*/
		/*if(-1 ==  crear_ventanillas( g_apuestas, caballos_creados,  n_ventanillas, n_apostadores, msqid_cola_msg)){
			printf("\n ERROR AL CREAR VENTANILLAS");
		}
		inicializa_apuestas(g_apuestas,caballos_creados);
		*/
		set_gestor_apuestas_msqid(&mem_compartida->g_apuestas,msqid_cola_msg);

		ventanillas_abre_ventas( g_apuestas);/*Lanza los hilos*/

		/*BUCLE PRINCIPAL DEL PROCESO*/
		do{
			Down_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);/*Notificamos del inicio de la carrera al gesotr de apuestas*/
			carrera_comenzada =contenido->carrera_comenzada;
			Up_Semaforo( *sh_gestor->semaforo,0, SEM_UNDO);
			/*Los hilos estan atiendo*/
		}while(carrera_comenzada != 1);

		//YA HA EMPEZADO LA CARRERA
		ventanillas_cierra_ventas(g_apuestas);

		exit(EXIT_SUCCESS);

	} else if (i==2){
		/*PROCESO APOSTADOR*/
		//printf("\tSoy el proceso apostador\n");
		//trabajo_proceso_apostador(imprimir_bonito);
		/*while(1){
			msgsnd(msqid_cola_msg,);
		}*/
		for(i=0,j=2;i<5;i++,j++){
			memset(&msg.contenido[0],0,3000);
			//ID ID_CABALLO CANTIDAD
			//aleat_num(1,get_(saldo),0);
	    	sprintf(msg.contenido,"%d-%d-300",i,j);
			msg.id = 12;
			msgsnd(msqid_cola_msg,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long), 0);
		}
		exit(EXIT_SUCCESS);
	}

	sigemptyset(&set_caballos);  

	/*CREACION DE LOS CABALLOS*/
	if(imprimir_bonito == 1){
		gotoxy(31,1);
	}

	for(i=0;i<atoi(argv[1]);){
		if(pipe(tuberias_padre_hijo[i])==-1) {
			printf("Linea %d - Error creando la tuberia\n", __LINE__);
			exit(EXIT_FAILURE);
		}

		if(imprimir_bonito == 1){
			clave_caballos_imprimir = ftok ("/bin/ls", KEY); 
			if (clave_caballos_imprimir == (key_t) -1) { 
				perror("Error al obtener clave para cola mensajes\n"); 
				exit(EXIT_FAILURE); 
			} 

			msqid_caballos_imprimir = msgget (clave_caballos_imprimir, 0600 | IPC_CREAT); 
			if (msqid_caballos_imprimir == -1) { 
				perror("Error al obtener identificador para cola mensajes"); 
				return(0); 
			}
		}

		if(pid = fork()){
			syslog( LOG_SYSLOG | LOG_INFO, "Proceso principal crea el caballo numero %d", i);
			set_caballos_id(&mem_compartida->caballos_creados,i,i);
			set_caballos_proceso_id(&mem_compartida->caballos_creados,i,pid);
			i++;
		} else {
			srand(getpid());
			signal(SIGUSR1, manejador_SIGUSR1);
			id_local_caballo = i;
			set_caballos_proceso_id(&mem_compartida->caballos_creados,i,getpid());
			
			if(imprimir_bonito == 1){
				fflush(stdout);
				gotoxy(70,id_local_caballo+30);
			}
			printf("\t\tCaballo %d listo - %d\n", i, getpid());
			
			pause();			
			do{
				if(imprimir_bonito==0){
					tirada_realizada = trabajo_caballos(msqid_cola_msg, msqid_caballos_imprimir, semaforon, tuberias_padre_hijo, id_local_caballo, posicion_local_caballo, imprimir_bonito, atoi(argv[1]));
				} else {
					tirada_realizada = trabajo_caballos(msqid_cola_msg, msqid_caballos_imprimir, semaforon, tuberias_padre_hijo, id_local_caballo, posicion_local_caballo, imprimir_bonito, atoi(argv[1]));
					if(imprimir_activado==0){
						if(fork()==0){
							recorrer_pista_i(msqid_caballos_imprimir, semaforon, atoi(argv[2]), tuberias_padre_hijo, id_local_caballo, 
											 posicion_local_caballo, imprimir_bonito, atoi(argv[1]));
							exit(EXIT_SUCCESS);
						}
						imprimir_activado = 1;
					}
				}
			} while (posicion_local_caballo != -1);

			printf("Soy el caballo %d y me retiro a los establos\n",id_local_caballo);
			wait(NULL);
			exit(EXIT_SUCCESS);
		}
	}

	/*TERMINA LA CREACION DE LOS CABALLOS Y EL GESTOR DE APUESTAS TIENE YA LA ESTRUCTURA DE CABALLOS*/
	Down_Semaforo(*semaforon, s_sincro_principal_gestor_1, SEM_UNDO);
	Up_Semaforo(*semaforon, s_sincro_principal_gestor_2, SEM_UNDO);

	//Mandar por memoria la estructura

	signal(SIGINT, manejador_SIGINT); /* Rutina por defecto */

	/* Bloqueamos la señal*/
	sigemptyset(&set_caballos);  
	sigaddset(&set_caballos,SIGUSR1);
	if( -1 == sigprocmask(SIG_BLOCK, &set_caballos,&oset_caballos)){ /*Bloquea la recepción de las señales */
		printf("Linea %d - Error al hacer procmask", __LINE__);
	}
	/***********************************************************************************************/

	/*SINCRONIZACION CON EL PROCESO MONITOR, UNA VEZ TERMINADA LA CUENTA ATRAS*/
	Down_Semaforo(*semaforon, s_sincro_1, SEM_UNDO);
	Up_Semaforo(*semaforon, s_sincro_2, SEM_UNDO);

	if(imprimir_bonito == 1){
		system("clear");
		imprimir_hipodromo();
	}

	i_contador = 0;
	if(NULL==(cadena_pt_aux = malloc(sizeof(char))))return -1;
	for(j=0;j<get_caballos_num_caballos(mem_compartida->caballos_creados);j++){
		kill(get_caballos_proceso_id(mem_compartida->caballos_creados,j),SIGUSR1);
	}

	do{
		notificar_posicion_caballos_hp(tuberias_padre_hijo, &mem_compartida->caballos_creados);

		for(j=0;j<get_caballos_num_caballos(mem_compartida->caballos_creados);){
			memset(&msg.contenido[0],0,3000);
			msgrcv(msqid_cola_msg,(struct msgbuf *) &msg, sizeof(mensaje)-sizeof(long),0, 0);

	        /* El mensaje contiene primero el id del caballo y luego su tirada*/
			//strcpy(cadena_pipes,"1-4");
			cadena_pt_aux=strtok(msg.contenido,"-");
			i_aux = atoi(cadena_pt_aux);
			cadena_pt_aux=strtok(NULL,"-");
			i_aux2 = atoi(cadena_pt_aux);

			set_caballos_tirada(&mem_compartida->caballos_creados,i_aux,i_aux2);
			set_caballos_acumulado_tirada(&mem_compartida->caballos_creados,i_aux,i_aux2);
			j++;

		}

		actualizar_posiciones_caballos(&mem_compartida->caballos_creados);
		i_contador ++;
		sleep(1);
		

		/*SINCRONIZACION CON EL PROCESO MONITOR, UNA VEZ TERMINADA HECHA CADA TIRADA*/
		Up_Semaforo(*semaforon, s_sincro_2, SEM_UNDO);
		Down_Semaforo(*semaforon, s_sincro_1, SEM_UNDO);

	}while(flag_fin == 0);

	free(cadena_pt_aux);
	cadena_pt_aux = NULL;


	notificar_posicion_caballos_fin_hp(tuberias_padre_hijo, &mem_compartida->caballos_creados);
	//decir por memoria compartida que ya esta terminado
	
	system("clear");
	imprimir_podio_caballos(mem_compartida->caballos_creados, imprimir_bonito);

	//Hablar con proceso monitor para decirle que ha termiando la carrera

	while(wait(NULL)>0);	

	return 1;

}


void imprimir_cuenta_atras(int imprimir_bonito){
	
	int i = 0;

	for(i=5;i>=0;i--){
		if(imprimir_bonito == 1){
			gotoxy(108,5);
			printf("%d\n", i);
		} else {
			printf("Cuenta atras: %d\n", i);
		}
		sleep(1);	
	}
	
}


void imprimir_podio_caballos(caballos caballos_creados, int imprimir_bonito){

	int i = 0, j = 0;
	for(i=0;i<get_caballos_num_caballos(caballos_creados);i++){
		for(j=0;j<get_caballos_num_caballos(caballos_creados);j++){
			if(get_caballos_posicion(caballos_creados,j)==i){
				if(imprimir_bonito == 1){
					gotoxy(50, 36+i);
				}
				printf("Caballo %d - En el puesto %d - Tirada %d - Acum %d\n",
				get_caballos_id(caballos_creados,j), get_caballos_posicion(caballos_creados,j),
				get_caballos_tirada(caballos_creados,j), get_caballos_acumulado_tirada(caballos_creados,j));
			}
		}
	}

}


void notificar_posicion_caballos_hp(int tuberias_padre_hijo[MAX_CABALLOS][2], caballos *caballos_creados){
	int i = 0;
	char cadena_pipes[5]="";

	
	for(i=0;i<get_caballos_num_caballos(*caballos_creados);i++){
		strcpy(cadena_pipes,"");
		sprintf(cadena_pipes,"%d", get_caballos_posicion(*caballos_creados,i));
		syslog( LOG_SYSLOG | LOG_INFO, "Notifica a caballo: %s ", cadena_pipes);

        close(tuberias_padre_hijo[i][0]);
        write(tuberias_padre_hijo[i][1], cadena_pipes, strlen(cadena_pipes));
	}

}


void notificar_posicion_caballos_fin_hp(int tuberias_padre_hijo[MAX_CABALLOS][2], caballos *caballos_creados){
	int i = 0;
	char cadena_pipes[5]="";

	
	for(i=0;i<get_caballos_num_caballos(*caballos_creados);i++){
		strcpy(cadena_pipes,"");
		sprintf(cadena_pipes,"-1");
        close(tuberias_padre_hijo[i][0]);
        write(tuberias_padre_hijo[i][1], cadena_pipes, strlen(cadena_pipes));
	}
}

/**
*	@brief Manejador de señal SIGUSR1
*/
void manejador_SIGINT (int sig){
	
	printf("SIGINT recibido\n");
	kill(SIGINT,0);
	while(wait(NULL)>0);
}


/**
*	@brief Manejador de señal SIGUSR1
*/
void manejador_SIGUSR1(int sig){
	
	//printf("SIGUSR1 recibido, por pid %d\n", getpid());
	signal(sig, manejador_SIGUSR1); /* Restaura rutina por defecto */
}

void actualizar_posiciones_caballos(caballos * caballos_creados){

	int i=0;
	int *array;
	int flag_posicion = 0, contador = 0, posicion_int = 0, max = 0;

	array = malloc(sizeof(int)* get_caballos_num_caballos(*caballos_creados));
	if(array != NULL){
		posicion_int = get_caballos_num_caballos(*caballos_creados);
		for(i=0;i<get_caballos_num_caballos(*caballos_creados);i++){
			*(array+i)= get_caballos_acumulado_tirada(*caballos_creados,i);
		}
		get_caballos_num_caballos(*caballos_creados);
		for(contador=0;contador<get_caballos_num_caballos(*caballos_creados);contador++){
			for(i=0;i<get_caballos_num_caballos(*caballos_creados);i++){
				if(*(array+i)>max) {
					max = array[i];
					flag_posicion = i;
				}
			}
			*(array+flag_posicion) = 0;
			set_caballos_posicion(caballos_creados,flag_posicion,get_caballos_num_caballos(*caballos_creados)-posicion_int);
			posicion_int --;
			max = 0;
		}	
	} else {
		printf("Error en actualizar_posiciones_caballos\n");
	}

}
