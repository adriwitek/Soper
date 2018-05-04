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

int main(){

	estructura_memoria_compartida * mem_compartida;
	int * semaforo;	


	unsigned short array_comun[2] = {1, 0}; /*1 semaforo,inicializados a 1*/
	int id = 0;


	/////////////////////////////////////////

		
	semaforo = (int*) malloc(sizeof (int)); 
    if (semaforo == NULL) {printf("Linea %d - Error al reservar memoria\n", __LINE__);}
    if (-1 == Crear_Semaforo(IPC_PRIVATE, 1, semaforo)) {printf("Linea %d - Error al crear el semaforo\n", __LINE__);return -1;}
    if (Inicializar_Semaforo(*semaforo, array_comun) == -1) {printf("\n Linea %d - Error al inicializar el semaforo\n", __LINE__);return -1;}

  
  
    if((id=shmget(123,sizeof(struct _estructura_memoria_compartida),IPC_CREAT|IPC_EXCL|0660))==-1){
		if((id=shmget(123,sizeof(struct _estructura_memoria_compartida),0))==-1){
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


	printf("2 Id %d\n", get_caballos_id(mem_compartida->caballos_creados,1));
	printf("2 Num %d\n", get_caballos_num_caballos(mem_compartida->caballos_creados));
	printf("%d doasdasd\n", mem_compartida->semaforo_id);
	//printf("%d dsa \n", *mem_compartida->semaforo);

	Up_Semaforo(mem_compartida->semaforo_id,0,SEM_UNDO);


	//shmctl(id,IPC_SET,(struct shmid_ds *)caballos_creados);	


	///////////////////////////



	//shmctl(mem_id,IPC_SET,caballos_creados);

}