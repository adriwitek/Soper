#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include "utilidades.h"
#include "imprimir.h"
#include "caballo.h"
#include "semaforos.h"

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

#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define ini_X 70


int main(){

	int tamanio_carriles[10], i;
	int tamanio_carriles_fin[10];
	int longitud;
	int flag = 0;
	float espera = 1000000.0;
	caballos * caballos_creados;
	int ubicacion[10];

	tamanio_carriles[0] = 30;
	tamanio_carriles[1] = 25;
	tamanio_carriles[2] = 32;
	tamanio_carriles[3] = 10;
	tamanio_carriles[4] = 5;
	tamanio_carriles[5] = 4;
	tamanio_carriles[6] = 2;
	tamanio_carriles[7] = -5;
	tamanio_carriles[8] = 25;
	tamanio_carriles[9] = 2;

	int max=0;
	int i_aux,i_aux2;
int j = 0;
	for(i=0;i<10;i++){
		if(tamanio_carriles[i]>max){
			max = tamanio_carriles[i];
				
			tamanio_carriles_fin[j] = max;
			j++;
		}
	}

}
