#include <string.h>
#include "gestor_apuestas.h"
#define SH_KEY 1234

struct parametros{
    int id;/*Identificador de la ventanilla*/
};


int crear_ventanillas(struct _gestor_apuestas * g_apuestas,caballos* e_cab, int msqid){

    int i;
    pthread_t h[MAX_APOSTADORES+1];
    struct parametros q;



    q.id = 0;
    for(i=0;i<get_gestor_apuestas_n_ventanillas(*g_apuestas);){
        if(pthread_create(&h[i], NULL, ventanilla_atiende_clientes ,(void *)&q)!=0){
            printf("Modulo Apuestas: Linea %d - Error al crear el hilo\n", __LINE__);
            return -1;
        } else{
            printf("Lanzado el hilo %d\n", i);
            i++;
            //usleep(50000);
        }
    }


    for(i=0;i<get_gestor_apuestas_n_ventanillas(*g_apuestas);i++){
        pthread_join(h[i],NULL);
    }
    return 1; /*Ok*/
}


void * ventanilla_atiende_clientes(void *argv){ /*Para los hilos solo*/
  
    int id = 0;
    int id_local = 0;
    struct parametros * q;
    mensaje_ventanilla msg;
    mensaje msg_general;

    int id_apostador;
    int flag_fin = 0;

    apuesta ap;

    estructura_memoria_compartida * mem_compartida;


    q = (struct parametros *) argv;
    id_local = q->id;
    q->id++;
    
    syslog( LOG_SYSLOG | LOG_INFO, "Hilo %d iniciado", id_local);
    //printf("Hilo %d iniciado\n", id_local);

    /**************************************************************************************************/
    if((id=shmget(SH_KEY,sizeof(struct _estructura_memoria_compartida),IPC_CREAT|IPC_EXCL|0660))==-1){
        if((id=shmget(SH_KEY,sizeof(struct _estructura_memoria_compartida),0))==-1){
            printf("Error al abrir el segmento\n");
        }
        mem_compartida = shmat (id, (char *)0, 0);
        if (mem_compartida == NULL) {
            pthread_exit(NULL); 
        }
    }

    mem_compartida = shmat (id, (char *)0, 0);
    if (mem_compartida == NULL) {
        fprintf (stderr, "Error reserve shared memory \n");
        pthread_exit(NULL);
    }
    /**************************************************************************************************/

    //printf("Leemos apuestas de\t%d\n", mem_compartida->msqid_apuestas);
    //printf("FINITO en cola %d\n", mem_compartida->msqid);

    memset(&msg_general.contenido[0],0,3000);
    while( flag_fin == 0 ){/*Atiende mientras no halla comenzado*/
        msgrcv ( mem_compartida->msqid_apuestas, (struct msgbuf *) &msg, sizeof(mensaje_ventanilla) - sizeof(long),   0, 0);
        if(strcmp(msg.nombre_apostador,"FIN")==0){
            flag_fin = 1;
        } else {
            id_apostador = atoi(strtok(msg.nombre_apostador,"Apostador-"));
           

            Down_Semaforo(mem_compartida->semaforo_id, 1, SEM_UNDO);

            set_apuesta_apostador_id(&ap, id_apostador);
            set_apuesta_ventanilla_id(&ap, id_local);
            set_apuesta_caballo_id(&ap, msg.id_caballo);
            set_apuesta_cotizacion_caballo(&ap, get_caballos_cotizacion(mem_compartida->caballos_creados,msg.id_caballo));
            set_apuesta_cantidad_apostada(&ap, msg.dinero_apuesta);
            set_apuesta_posible_beneficio(&ap, msg.dinero_apuesta*get_caballos_cotizacion(mem_compartida->caballos_creados,msg.id_caballo));

            //imprimir_apuesta(ap);
            set_gestor_apuestas__apostador_apuesta(&mem_compartida->g_apuestas,id_apostador,ap);
            set_gestor_apuestas_apuestas_realizadas(&mem_compartida->g_apuestas,ap);

            printf("Apuesta %d - Apostador %d - Ventanilla %d - Caballo %d - Dinero %lf - Cotizacion %lf\n",
                get_gestor_apuestas_n_apuestas(mem_compartida->g_apuestas)-1
                , get_gestor_apuestas_apostador_id(mem_compartida->g_apuestas,get_gestor_apuestas_n_apuestas(mem_compartida->g_apuestas)-1), 
                get_gestor_apuestas__i__ventanilla_id(mem_compartida->g_apuestas,get_gestor_apuestas_n_apuestas(mem_compartida->g_apuestas)-1)
                , get_gestor_apuestas__i__caballo_id(mem_compartida->g_apuestas,get_gestor_apuestas_n_apuestas(mem_compartida->g_apuestas)-1), 
                get_gestor_apuestas__i__cantidad_apostada(mem_compartida->g_apuestas,get_gestor_apuestas_n_apuestas(mem_compartida->g_apuestas)-1)
                , get_gestor_apuestas__i__cotizacion_caballo(mem_compartida->g_apuestas,get_gestor_apuestas_n_apuestas(mem_compartida->g_apuestas)-1));

            actualizar_cotizaciones_caballos( &mem_compartida->g_apuestas, &mem_compartida->caballos_creados);
            Up_Semaforo(mem_compartida->semaforo_id, 1, SEM_UNDO);
        }
        

    } 
    printf("Salgo de atender apuestas - Ventanilla %d\n", id_local);
    syslog( LOG_SYSLOG | LOG_INFO, "Salgo de atender apuestas - Ventanilla %d\n", id_local);
    pthread_exit(NULL);
}


void actualizar_cotizaciones_caballos(struct _gestor_apuestas * g_apuestas,caballos* e_cab){/*Llamada desde los hilos*/

	int i =0;
	
	for(i=0;i<  get_caballos_num_caballos(*e_cab);i++){
        set_caballos_cotizacion(e_cab,i, get_total_apostado(g_apuestas)  / get_caballos_apostado(*e_cab,  i)   );
	}
}


double get_total_apostado(struct _gestor_apuestas * g_apuestas){
    return g_apuestas->total_apostado;
}


/***SETTERS***/
void set_gestor_apuestas_ga_msqid(gestor_apuestas * ga, int in){ga->ga_msqid=in;}
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
apuesta get_apostador_apuestas_realizadas(apostador ap, int i){return ap.apuestas_realizadas[i];}
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
void set_apostador_apuestas_realizadas(apostador * ap, apuesta in){
    set_apuesta_apostador_id(&ap->apuestas_realizadas[ap->n_apuestas_realizadas], in.apostador_id);
    set_apuesta_ventanilla_id(&ap->apuestas_realizadas[ap->n_apuestas_realizadas], in.ventanilla_id);
    set_apuesta_caballo_id(&ap->apuestas_realizadas[ap->n_apuestas_realizadas], in.caballo_id);
    set_apuesta_cotizacion_caballo(&ap->apuestas_realizadas[ap->n_apuestas_realizadas], in.cotizacion_caballo);
    set_apuesta_cantidad_apostada(&ap->apuestas_realizadas[ap->n_apuestas_realizadas], in.cantidad_apostada);
    set_apuesta_posible_beneficio(&ap->apuestas_realizadas[ap->n_apuestas_realizadas], in.posible_beneficio);
    
    // imprimir_apuesta(ap->apuestas_realizadas[ap->n_apuestas_realizadas]);
    //ap->saldo-=in.cantidad_apostada;
    ap->total_apostado+=in.cantidad_apostada;
    ap->n_apuestas_realizadas++;
}
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

void set_gestor_apuestas_n_apostadores(gestor_apuestas * ga, int in){
  ga->n_apostadores = in;
}


void set_gestor_apuestas_n_ventanillas(gestor_apuestas * ga, int in){
  ga->n_ventanillas = in;
}

void inicializar_apostadores(gestor_apuestas * g_apuestas, double saldo){

  char cadena[30]="";
  int i = 0;
  for(i=0;i<get_gestor_apuestas_n_apostadores(*g_apuestas);i++){
    memset(&cadena[0],0,30);
    sprintf(cadena,"Apostador-%d",i);
    set_apostador_id(&g_apuestas->apostadores[i], i);
    set_apostador_nombre(&g_apuestas->apostadores[i], cadena);
    set_apostador_total_apostado(&g_apuestas->apostadores[i], 0);
    set_apostador_n_apuestas_realizadas(&g_apuestas->apostadores[i], 0);
    set_apostador_beneficios_obtenidos(&g_apuestas->apostadores[i], 0);
    set_apostador_dinero_ganado(&g_apuestas->apostadores[i], 0);
    set_apostador_saldo(&g_apuestas->apostadores[i], saldo);
  }

}

int get_gestor_apuestas_n_ventanillas(gestor_apuestas g_apuestas){
  return g_apuestas.n_ventanillas;
}
void set_gestor_apuestas_general_msqid(gestor_apuestas * g_apuestas, int in){
    g_apuestas->general_msqid = in;
}

int get_gestor_apuestas_ga_msqid(gestor_apuestas ga){
    return ga.ga_msqid;
}
int get_gestor_apuestas_general_msqid(gestor_apuestas ga){
    return ga.general_msqid;
}

void imprimir_apuesta(apuesta ap){
    printf("apostador_id -> %d\n", ap.apostador_id);
    printf("ventanilla_id -> %d\n", ap.ventanilla_id);
    printf("caballo_id -> %d\n", ap.caballo_id);
    printf("cotizacion_caballo -> %lf\n", ap.cotizacion_caballo);
    printf("cantidad_apostada -> %lf\n", ap.cantidad_apostada);
    printf("posible_beneficio -> %lf\n", ap.posible_beneficio);
}

void set_gestor_apuestas__apostador_apuesta(gestor_apuestas * g_apuestas, int id, apuesta ap){
    set_apostador_apuestas_realizadas(&g_apuestas->apostadores[id],ap);
}
void set_gestor_apuestas_apuestas_realizadas(gestor_apuestas * g_apuestas, apuesta in){
    set_apuesta_apostador_id(&g_apuestas->apuestas_realizadas[g_apuestas->n_apuestas], in.apostador_id);
    set_apuesta_ventanilla_id(&g_apuestas->apuestas_realizadas[g_apuestas->n_apuestas], in.ventanilla_id);
    set_apuesta_caballo_id(&g_apuestas->apuestas_realizadas[g_apuestas->n_apuestas], in.caballo_id);
    set_apuesta_cotizacion_caballo(&g_apuestas->apuestas_realizadas[g_apuestas->n_apuestas], in.cotizacion_caballo);
    set_apuesta_cantidad_apostada(&g_apuestas->apuestas_realizadas[g_apuestas->n_apuestas], in.cantidad_apostada);
    set_apuesta_posible_beneficio(&g_apuestas->apuestas_realizadas[g_apuestas->n_apuestas], in.posible_beneficio);
   

    g_apuestas->total_apostado += in.cantidad_apostada;
    g_apuestas->n_apuestas++;
}

int get_gestor_apuestas__i__apostador_id(gestor_apuestas g_apuestas, int i){
    return get_apuesta_apostador_id(g_apuestas.apuestas_realizadas[i]);
}
int get_gestor_apuestas__i__ventanilla_id(gestor_apuestas g_apuestas, int i){
    return get_apuesta_ventanilla_id(g_apuestas.apuestas_realizadas[i]);
}
int get_gestor_apuestas__i__caballo_id(gestor_apuestas g_apuestas, int i){
    return get_apuesta_caballo_id(g_apuestas.apuestas_realizadas[i]);
}
double get_gestor_apuestas__i__cantidad_apostada(gestor_apuestas g_apuestas, int i){
    return get_apuesta_cantidad_apostada(g_apuestas.apuestas_realizadas[i]);
}
double get_gestor_apuestas__i__cotizacion_caballo(gestor_apuestas g_apuestas, int i){
    return get_apuesta_cotizacion_caballo(g_apuestas.apuestas_realizadas[i]);
}

void imprime_gestor_apuestas_apostador_apuestas(gestor_apuestas g_apuestas, int i){
    imprime_apuestas_apostador(g_apuestas.apostadores[i]);
}
void imprime_apuestas_apostador(apostador ap){
    int i = 0;
    for(i=0;i<get_apostador_n_apuestas_realizadas(ap);i++){
        printf("---------------------------------\n");
        imprimir_apuesta(ap.apuestas_realizadas[i]);
        printf("---------------------------------\n");
    }
}

void set_ganancias_apostadores(gestor_apuestas * g_apuestas, int id_ganador){
    int i = 0;
    for(i=0;i<get_gestor_apuestas_n_apostadores(*g_apuestas);i++){
        set_apostador_ganancias(&g_apuestas->apostadores[i],id_ganador);
    }
}

void set_apostador_ganancias(apostador * ap, int id_ganador){
    int i = 0;
    double beneficio = 0;
    for(i=0;i<get_apostador_n_apuestas_realizadas(*ap);i++){
        if(get_apuesta_caballo_id(ap->apuestas_realizadas[i])==id_ganador){
            printf("HAY GANANCIAS \n");
            beneficio = get_apuesta_cantidad_apostada(ap->apuestas_realizadas[i])*get_apuesta_cotizacion_caballo(ap->apuestas_realizadas[i]);
            set_apuesta_dinero_ganado(&ap->apuestas_realizadas[i],beneficio);
        }
    }
}
void set_gestor_apuestas_apostado(gestor_apuestas * g_apuestas, double in){
    g_apuestas->total_apostado = in;
}

void set_apuesta_dinero_ganado(apostador * ap, double in){
    ap->dinero_ganado+=in;
}