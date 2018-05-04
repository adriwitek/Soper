#include <string.h>
#include "gestor_apuestas.h"

struct parametros{
    struct _gestor_apuestas * g_apuestas;
    caballos* e_cab;
    int id;/*Identificador de la ventanilla*/
};

int crear_ventanillas(struct _gestor_apuestas * g_apuestas,caballos* e_cab, int n_ventanillas,int n_apostadores, int msqid){

  int i;
  unsigned short array_comun[2] = {1, 0}; /*1 semaforos,inicializados a 0*/
  struct parametros p; /*Para las ventanillas(hilos)*/
  apostador *ap;
  if(g_apuestas == NULL){
    return -1;
  }
  g_apuestas->carrera_comenzada=0;
  g_apuestas->n_ventanillas = n_ventanillas;
  g_apuestas->ventanillas = (pthread_t*)malloc(n_ventanillas*sizeof(pthread_t));
  if(g_apuestas->ventanillas == NULL){
     printf("Modulo Apuestas: Linea %d - Error al reservar memoria\n", __LINE__);
  }
  
  p.g_apuestas = g_apuestas;
  p.e_cab = e_cab;
  for(i=0;i<n_ventanillas;i++){
     p.id = i;
    if( pthread_create(&g_apuestas->ventanillas[i],NULL, ventanilla_atiende_clientes ,(void*)&p )  ){
       printf("Modulo Apuestas: Linea %d - Error al crear el hilo\n", __LINE__);
       free(g_apuestas->ventanillas);
      return -1;
    }
  }
  
   g_apuestas->total_apostado = 0;
   g_apuestas->n_apuestas = 0;
   g_apuestas->n_apostadores = n_apostadores;
//   g_apuestas->apostadores = (apostador*)malloc(n_apostadores*sizeof(apostador));
   if(g_apuestas->apostadores == NULL){
     printf("Modulo Apuestas: Linea %d - Error al reservar memoria\n", __LINE__);
  }
  for(i=0;i<n_apostadores;i++){/*Inicializamos los apostadores*/
    ap = get_apostador_by_id( g_apuestas, i);
    ap->id=i;
    sprintf( ap->nombre,"Apostador-%d",i);
    ap->total_apostado=0;
    ap->n_apuestas_realizadas = 0 ;
    ap->beneficios_obtenidos = 0;
    ap->dinero_ganado = 0;
  }
  

	g_apuestas->ga_msqid = msqid;
  
   /*Creamos el semaforo ventanillas,para acerptar apuestas*/
    g_apuestas->sem_ventanillas = (int*) malloc(sizeof (int)); 
    if (g_apuestas->sem_ventanillas == NULL) {
        printf("Modulo apuestas : Linea %d - Error al reservar memoria\n", __LINE__);
    }
   

    if (-1 == Crear_Semaforo(IPC_PRIVATE, 1, g_apuestas->sem_ventanillas)) {
        printf("Modulo apuestas :Linea %d - Error al crear el semaforo\n", __LINE__);
        return -1;
    }
    if (Inicializar_Semaforo(*g_apuestas->sem_ventanillas, array_comun) == -1) {
        printf("\n Modulo apuestas :Linea %d - Error al inicializar el semaforo\n", __LINE__);
        return -1;
    } else {
        printf("Modulo apuestas :Semaforo inicializado correctamente\n");
    }
  
  return 1; /*Ok*/
}



int ventanillas_abre_ventas(struct _gestor_apuestas * g_apuestas){
  int i;
  if(g_apuestas ==NULL){
    return -1;  
  }
  
  for(i=0;i<g_apuestas->n_ventanillas;i++){
    pthread_join(g_apuestas->ventanillas[i],NULL);
  }
  Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	/*Ponemos a 1 el semaforo*/
  return 1;
}


int ventanillas_cierra_ventas(struct _gestor_apuestas * g_apuestas){
    if(g_apuestas ==NULL){
    return -1;  
  }
   Down_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
        g_apuestas->carrera_comenzada=1;
    Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);
    return 1;
}




void * ventanilla_atiende_clientes(void *argv){ /*Para los hilos solo*/
  
  struct parametros *q;
  apostador *ap;
  q = (struct parametros*)argv;
  struct _gestor_apuestas * g_apuestas  =  q->g_apuestas;
   if(g_apuestas == NULL){
    exit(EXIT_FAILURE) ;
  }
  caballos* e_cab = q->e_cab;
  int id = q->id;/*Identificador de la ventanilla*/
  apuesta * apuesta;
  mensaje_ventanilla msg;
  int id_apostador;
  
  
  while( g_apuestas->carrera_comenzada != 1){/*Atiende mientras no halla comenzado*/
   
    msgrcv ( g_apuestas->ga_msqid, (struct msgbuf *) &msg, sizeof(mensaje_ventanilla) - sizeof(long),   12, 0); 
    printf("dijoadoasojjsaojsa\n");
    id_apostador = atoi(strtok(msg.nombre_apostador,"Apostador-"));
    ap =  get_apostador_by_id(g_apuestas, id_apostador);
    Down_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
          g_apuestas->total_apostado += msg.dinero_apuesta;
          ap->total_apostado +=   msg.dinero_apuesta;
        
          apuesta = (struct _apuesta*)malloc( 1* sizeof(apuesta)  );
          if(apuesta == NULL){
               printf("Modulo Apuestas: Linea %d - Error al reservar memoria\n", __LINE__);
               exit(EXIT_FAILURE);
          }
          apuesta->caballo_id= msg.id_caballo;
          apuesta->cantidad_apostada = msg.dinero_apuesta;
          apuesta->cotizacion_caballo = get_caballos_cotizacion(*e_cab, get_caballos_id(*e_cab, msg.id_caballo));
//          apuesta->posible_benficio = msg.dinero_apuesta * apuesta->cotizacion_caballo;
          apuesta->ventanilla_id = id;
          
          
//          g_apuestas->apuestas_realizadas[ g_apuestas->n_apuestas] = apuesta; /*Anniadimos a la lista general de apuestas*/
          g_apuestas->n_apuestas++;
          
          
//          ap->apuestas_realizadas[ap->n_apuestas_realizadas] = apuesta;
          ap->n_apuestas_realizadas++;
             
          actualizar_cotizaciones_caballos( g_apuestas,e_cab);
    Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
  } 
  
  pthread_exit(NULL);
}




void inicializa_apuestas(struct _gestor_apuestas * g_apuestas,caballos * e_cab){
	if(e_cab == NULL){
		return;
	}
	int i;
    apostador * ap;
	
	for(i=0;i<  get_caballos_total(*e_cab);i++){/*Inicializar apuestas ccabllos*/
		set_caballos_apostado(e_cab, get_caballos_id(*e_cab, i), 1.0);
		set_caballos_cotizacion(e_cab,get_caballos_id(*e_cab, i),0.0);
	}
	
	for(i=0;i<g_apuestas->n_apostadores  ;i++){/*Inicializar apostadores*/
    	ap = get_apostador_by_id( g_apuestas,i);
	    ap->total_apostado = 0.0;
	}
	return;
}


void actualizar_cotizaciones_caballos(struct _gestor_apuestas * g_apuestas,caballos* e_cab){/*Llamada desde los hilos*/
	if(e_cab == NULL){
		return;
	}
	int i =0;
	
	for(i=0;i<  get_caballos_total(*e_cab);i++){
		set_caballos_apostado(e_cab, get_caballos_id(*e_cab, i), 1.0);
		set_caballos_cotizacion(e_cab,get_caballos_id(*e_cab, i), get_total_apostado(g_apuestas)  / get_caballos_apostado(*e_cab,  get_caballos_id(*e_cab, i))   );
	}
	return;
}




/*int* get_top_10_apostadores(struct _gestor_apuestas * g_apuestas){}*/

double get_total_apostado(struct _gestor_apuestas * g_apuestas){
    double apostado;
    if(g_apuestas == NULL){
    return -1;
  }
    Down_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
     apostado = g_apuestas->total_apostado;
    Up_Semaforo(*g_apuestas->sem_ventanillas, 0, SEM_UNDO);	
  return apostado;
}

void liberar_gestor_apuestas(struct _gestor_apuestas * g_apuestas){
  if(g_apuestas == NULL){
    return;
  }
   Borrar_Semaforo(*g_apuestas->sem_ventanillas);
   msgctl (g_apuestas->ga_msqid, IPC_RMID, (struct msqid_ds *)NULL);/*Borramos cola de mensajes*/
   free(g_apuestas->ventanillas);
 ///////////////////**************BUCLE APUESTAS DE CADA APOSTADOR********/ 
   free(g_apuestas->apostadores);
   free(g_apuestas->apuestas_realizadas);
   return;
}

apostador * get_apostador_by_id(struct _gestor_apuestas * g_apuestas,int id){
    if(!g_apuestas){
        return NULL;
    }
    return  &g_apuestas->apostadores[id];
}




/***SETTERS***/
void set_gestor_apuestas_msqid(gestor_apuestas * ga, int in){ga->ga_msqid=in;}
/***FIN SETTERS***/


/***GETTERS***/

int get_gestor_apuestas_n_apuestas(gestor_apuestas ga){return ga.n_apuestas;}
int get_gestor_apuestas_n_apostadores(gestor_apuestas ga){return ga.n_apostadores;}
int get_gestor_apuestas_apostador_id(gestor_apuestas ga, int i) { return get_apuesta_apostador_id(ga.apuestas_realizadas[i]);}
int get_gestor_apuestas_ventanilla_id(gestor_apuestas ga, int i) { return get_apuesta_ventanilla_id(ga.apuestas_realizadas[i]);}
int get_gestor_apuestas_caballo_id(gestor_apuestas ga, int i) { return get_apuesta_caballo_id(ga.apuestas_realizadas[i]);}
double get_gestor_apuestas_cotizacion_caballo(gestor_apuestas ga, int i) { return get_apuesta_cotizacion_caballo(ga.apuestas_realizadas[i]);}
double get_gestor_apuestas_cantidad_apostada(gestor_apuestas ga, int i) { return get_apuesta_cantidad_apostada(ga.apuestas_realizadas[i]);}

int get_gestor_apuestas_apostador_n_apuestas_realizadas(gestor_apuestas ga, int i){
  return get_apostador_n_apuestas_realizadas(ga.apostadores[i]);
}

/*GETTERS*/

int get_apuesta_apostador_id(apuesta ap){return ap.apostador_id;}
int get_apuesta_ventanilla_id(apuesta ap){return ap.ventanilla_id;}
int get_apuesta_caballo_id(apuesta ap){return ap.caballo_id;}
double get_apuesta_cotizacion_caballo(apuesta ap){return ap.cotizacion_caballo;}
double get_apuesta_cantidad_apostada(apuesta ap){return ap.cantidad_apostada;}
double get_apuesta_posible_beneficio(apuesta ap){return ap.posible_beneficio;}

int get_apostador_id(apostador ap){return ap.id;}
char * get_apostador_nombre(apostador ap){char * returno = strdup(ap.nombre);return returno;}
double get_apostador_total_apostado(apostador ap){return ap.total_apostado;}
int get_apostador_n_apuestas_realizadas(apostador ap){return ap.n_apuestas_realizadas;}
//apuesta get_apostador_apuestas_realizadas(apostador ap, int i){return ap.apuestas_realizadas[i];}
double get_apostador_beneficios_obtenidos(apostador ap){return ap.beneficios_obtenidos;}
double get_apostador_dinero_ganado(apostador ap){return ap.dinero_ganado;}
double get_apostador_saldo(apostador ap){return ap.saldo;}


double get_gestor_apuestas_apostador_saldo(gestor_apuestas ga, int i){
  return get_apostador_saldo(ga.apostadores[i]);
}

char * get_gestor_apuestas_apostador_nombre(gestor_apuestas ga, int i){
  return get_apostador_nombre(ga.apostadores[i]);
}

/*SETTERS*/

void set_apuesta_apostador_id(apuesta * ap, int in){ap->apostador_id=in;}
void set_apuesta_ventanilla_id(apuesta * ap, int in){ap->ventanilla_id=in;}
void set_apuesta_caballo_id(apuesta * ap, int in){ap->caballo_id=in;}
void set_apuesta_cotizacion_caballo(apuesta * ap, double in){ap->cotizacion_caballo=in;}
void set_apuesta_cantidad_apostada(apuesta * ap, double in){ap->cantidad_apostada=in;}
void set_apuesta_posible_beneficio(apuesta * ap, double in){ap->posible_beneficio=in;}

void set_apostador_id(apostador * ap, int in){ap->id=in;}
void set_apostador_nombre(apostador * ap, char *in){ strcpy(ap->nombre,in);}
void set_apostador_total_apostado(apostador * ap, double in){ap->total_apostado=in;}
void set_apostador_n_apuestas_realizadas(apostador * ap, int in){ap->n_apuestas_realizadas=in;}
//void set_apostador_apuestas_realizadas(apostador * ap, apuesta in){ap->apuestas_realizadas=in;}
void set_apostador_beneficios_obtenidos(apostador * ap, double in){ap->beneficios_obtenidos=in;}
void set_apostador_dinero_ganado(apostador * ap, double in){ap->dinero_ganado=in;}
void set_apostador_saldo(apostador * ap, double in){ap->saldo=in;}


/***FIN GETTERS***/



double get_gestor_apuestas_apostador_apuestas_realizadas_cantidad_apostada(gestor_apuestas g_apuestas,int i, int j){
  return get_apostador_apuestas_realizadas_cantidad_apostada(g_apuestas.apostadores[i],j);
}
double get_apostador_apuestas_realizadas_cantidad_apostada(apostador ap, int i){
  //return get_apuesta_cantidad_apostada();
  return 0;
}