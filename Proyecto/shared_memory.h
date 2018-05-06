#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "semaforos.h"
#include  <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h> /* shm* */
#include <wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "semaforos.h"



typedef struct info{
   int shmid;
   int *  semaforo;
   void * contenido;
}informacion;


/*tamanio en N bytes(del contenido de la struct info)*/
/*el parametro key es necesario para poder asociar la memoria con otra que tenga la misma clave*/
/*!!!!Ojo, el puntero a char devuelto es conveniente hacerle un cast a informacion **/
/*El semaforo se inicializa dentro de la funcion,y se libera tb con destruir*/
char * crea_o_asocia_shm(int key,int * semaforo,int tamannio);
void destruye_shm(char * informacion);