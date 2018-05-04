#include "shared_memory.h"	

char * crea_o_asocia_shm(int key,int * semaforo, int tamannio){
		
		unsigned short array_comun[2] = {1, 1}; /*1 semaforo,inicializados a 1*/
		struct info * informacion;
		int id = 0;
		
		semaforo = (int*) malloc(sizeof (int)); 
    if (semaforo == NULL || tamannio < 0) {
        printf("Linea %d - Error al reservar memoria\n", __LINE__);
    }
    
    if (-1 == Crear_Semaforo(IPC_PRIVATE, 1, semaforo)) {/*1 semaforo*/
        printf("Linea %d - Error al crear el semaforo\n", __LINE__);
        return NULL;
    }
    if (Inicializar_Semaforo(*semaforo, array_comun) == -1) {
        printf("\n Linea %d - Error al inicializar el semaforo\n", __LINE__);
        return NULL;
    }/* else {*/
     /*   printf("Semaforo inicializado correctamente\n");
    }*/

  
  

    if((id=shmget(key,tamannio + (sizeof(int) + sizeof(int *)),IPC_CREAT|IPC_EXCL|0660))==-1){
			/*printf("El segmento de memoria compartida ya existe\n");
			printf(" Abriendo como cliente\n");*/
			if((id=shmget(key,tamannio + (sizeof(int) + sizeof(int *) ),0))==-1){
				printf("Error al abrir el segmento\n");
			}
			sleep(2);
			informacion = shmat (id, (char *)0, 0);
			if (informacion == NULL) {
				return NULL; 
			}else{
				return (char *) informacion;
			}
			
	
			
			
	/*	} else {*/
		/*	printf("Nuevo segmento creado\n");*/
	}
	
		informacion = shmat (id, (char *)0, 0);
		if (informacion == NULL) {
			fprintf (stderr, "Error reserve shared memory \n");
			return NULL; 
		}
		informacion->shmid = id;
		informacion->semaforo = semaforo ;
		informacion->contenido = (char *)  (informacion + sizeof(int) +sizeof(int*)) ;/*situamos el puntero*/
		return (char*)informacion;
	}
	
	
void destruye_shm(char * informacion){
		int id;
		Borrar_Semaforo(*( (struct info *)informacion)->semaforo)  ;
		id = ( (struct info *)informacion)->shmid;
		shmdt ((char *)informacion);
	  shmctl (id, IPC_RMID, (struct shmid_ds *)NULL);
	}
	