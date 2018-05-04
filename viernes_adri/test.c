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


void recorrer_pistas_i(int num_pista, int longitud);


int main(){

	int tamanio_carriles[10], i;
	int longitud;
	int flag = 0;
	float espera = 1000000.0;
	caballos * caballos_creados;
	int ubicacion[10];

	tamanio_carriles[0] = 286;
	tamanio_carriles[1] = 273;
	tamanio_carriles[2] = 258;
	tamanio_carriles[3] = 245;
	tamanio_carriles[4] = 230;
	tamanio_carriles[5] = 219;
	tamanio_carriles[6] = 203;
	tamanio_carriles[7] = 187;
	tamanio_carriles[8] = 172;
	tamanio_carriles[9] = 153;

	caballos_creados = malloc(sizeof(caballos));
	caballos_creados->caballos_registrados = malloc(sizeof(caballo)*10);
	set_caballos_num_caballos(caballos_creados,10);
	inicializar_caballos(*caballos_creados);

	system("clear");
		
	imprimir_hipodromo();

	longitud = 300;
	float espera_carril = 0.0;
	flag = 0;
	for(i=0;i<10 && flag ==0;i++){
		
		if(fork()==0){	
			srand(getpid());
			recorrer_pistas_i(i, longitud);
			flag = 1;
		}
	}

	gotoxy(1,49);
	while(wait(NULL)>0);

}

int tirada = 1;

void recorrer_pistas_i(int num_pista, int longitud){

	int posicion_X = ini_X, posicion_Y = 4+num_pista*2;
	int j = 0;
	//int tirada = 1;
	FILE * f= NULL;
	int acum = 0;
	int i = 0;
	float tiempo=0.0;
	float suma_tiempo = 0.0;
	switch(num_pista){
		case 0:
			f = fopen("tiradas.txt","w");
			for(j=0;j<=LONGITUD_CARRIL_0;j++){

				if(acum<longitud){
					tirada = aleat_num(1,6,9);
					tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
					tiempo *= 1000000;
					suma_tiempo+=tiempo;
					acum+=tirada;
					fprintf(f, "\t%d\t%d\t%lf\t%lf\n", tirada, acum, tiempo, suma_tiempo);
				}
				usleep(tiempo);

				if(posicion_X<=119 && posicion_Y == 4){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X >  119 && posicion_X <= 131 && posicion_Y >=  4 && posicion_Y <= 16){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 132 &&                      posicion_Y>=  16 && posicion_Y <= 34 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >  119 && posicion_X <= 132 && posicion_Y >= 35 && posicion_Y <  46){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  20 && posicion_X <= 132 && posicion_Y == 46){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X >=   7 && posicion_X <=  20 && posicion_Y <= 46 && posicion_Y >= 34 ){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==   6 &&                      posicion_Y >= 18 && posicion_Y <= 34 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=   6 && posicion_X <=  20 && posicion_Y >=  4 && posicion_Y <= 17){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  20 && posicion_X <= 119 && posicion_Y ==  4){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			fclose(f);
			break;
		case 1:
		i=0;
			for(j=0;j<=LONGITUD_CARRIL_1;j++){
								
				tirada = aleat_num(1,6,9);
				tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
				tiempo *= 1000000;
				usleep(tiempo);				
				if(posicion_X<=118 && posicion_Y == 6){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X >  118 && posicion_X <= 128 && posicion_Y >=  6 && posicion_Y <= 16){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 129 &&                      posicion_Y >= 16 && posicion_Y <= 33 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >  118 && posicion_X <= 129 && posicion_Y >= 34 && posicion_Y <  44){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  21 && posicion_X <= 129 && posicion_Y == 44){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X >=   7 && posicion_X <=  21 && posicion_Y <= 46 && posicion_Y >= 34){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==   9 &&                      posicion_Y >= 18 && posicion_Y <= 34 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=   9 && posicion_X <=  21 && posicion_Y >=  6 && posicion_Y <= 17){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  21 && posicion_X <= 118 && posicion_Y ==  6){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			break;
		case 2:
			for(j=0;j<=LONGITUD_CARRIL_2;j++){
				tirada = aleat_num(1,6,9);
				tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
				tiempo *= 1000000;
				usleep(tiempo);				
				if(posicion_X<=116 && posicion_Y == 8){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X >  116 && posicion_X <= 126 && posicion_Y >=  8 && posicion_Y <= 16){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 126 &&                      posicion_Y >= 16 && posicion_Y <= 33 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >  116 && posicion_X <= 126 && posicion_Y >= 34 && posicion_Y <  42){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  23 && posicion_X <= 126 && posicion_Y == 42){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X >=  13 && posicion_X <=  23 && posicion_Y <= 46 && posicion_Y >= 34 ){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  13 &&                      posicion_Y >= 18 && posicion_Y <= 33 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=  13 && posicion_X <=  23 && posicion_Y >=  4 && posicion_Y <= 17){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  23 && posicion_X <= 116 && posicion_Y ==  4){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			break;
		case 3:
			for(j=0;j<=LONGITUD_CARRIL_3;j++){
								
				tirada = aleat_num(1,6,9);
				tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
				tiempo *= 1000000;
				usleep(tiempo);								
				if(posicion_X<=115 && posicion_Y == 10){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X >  115 && posicion_X <= 122 && posicion_Y >= 10 && posicion_Y <= 16){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 123 &&                      posicion_Y >= 17 && posicion_Y <= 32 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >  115 && posicion_X <= 123 && posicion_Y >= 32 && posicion_Y <  40){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  24 && posicion_X <= 123 && posicion_Y == 40){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X >=  16 && posicion_X <=  24 && posicion_Y <= 40 && posicion_Y >= 32){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  15 &&                      posicion_Y >= 19 && posicion_Y <= 32 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=  15 && posicion_X <=  24 && posicion_Y >= 10 && posicion_Y <= 19){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  24 && posicion_X <= 115 && posicion_Y == 10){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			break;
		case 4:
			for(j=0;j<=LONGITUD_CARRIL_4;j++){
								
				tirada = aleat_num(1,6,9);
				tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
				tiempo *= 1000000;
				usleep(tiempo);				
				if(posicion_X<=113 && posicion_Y == 12){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X >  113 && posicion_X <= 119 && posicion_Y >= 12 && posicion_Y <= 17){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 120 &&                      posicion_Y >= 18 && posicion_Y <= 32 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >  113 && posicion_X <= 120 && posicion_Y >= 32 && posicion_Y <  38){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  26 && posicion_X <= 120 && posicion_Y == 38){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X >=  17 && posicion_X <=  26 && posicion_Y <= 38 && posicion_Y >= 32){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  18 &&                      posicion_Y >= 20 && posicion_Y <= 33 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=  18 && posicion_X <=  26 && posicion_Y >= 12 && posicion_Y <= 20){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  26 && posicion_X <= 113 && posicion_Y == 12){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			break;
		case 5:
			for(j=0;j<=LONGITUD_CARRIL_5;j++){
								
				tirada = aleat_num(1,6,9);
				tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
				tiempo *= 1000000;
				usleep(tiempo);				
				if(posicion_X<=112 && posicion_Y == 14){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X >  112 && posicion_X <= 117 && posicion_Y >= 14 && posicion_Y <= 18){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 118 &&                      posicion_Y >= 19 && posicion_Y <= 31 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >  112 && posicion_X <= 118 && posicion_Y >= 31 && posicion_Y <  36){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  26 && posicion_X <= 118 && posicion_Y == 36){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X >=  18 && posicion_X <=  26 && posicion_Y <= 36 && posicion_Y >= 33){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  21 &&                      posicion_Y >= 20 && posicion_Y <= 32 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=  19 && posicion_X <=  26 && posicion_Y >= 14 && posicion_Y <= 19){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  26 && posicion_X <= 112 && posicion_Y == 14){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			break;
		case 6:
			for(j=0;j<=LONGITUD_CARRIL_6;j++){
								
				tirada = aleat_num(1,6,9);
				tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
				tiempo *= 1000000;
				usleep(tiempo);				
				if(posicion_X<=110 && posicion_Y == 16){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X >  110 && posicion_X <= 114 && posicion_Y >= 16 && posicion_Y <= 19){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 115 &&                      posicion_Y >= 19 && posicion_Y <= 30 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >  110 && posicion_X <= 115 && posicion_Y >= 30 && posicion_Y <  34){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  28 && posicion_X <= 115 && posicion_Y == 34){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X >=  19 && posicion_X <=  28 && posicion_Y <= 34 && posicion_Y >= 32){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  24 &&                      posicion_Y >= 21 && posicion_Y <= 33){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=  19 && posicion_X <=  28 && posicion_Y >= 16 && posicion_Y <= 20){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  28 && posicion_X <= 110 && posicion_Y == 16){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			break;
		case 7:
			for(j=0;j<=LONGITUD_CARRIL_7;j++){
								
				tirada = aleat_num(1,6,9);
				tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
				tiempo *= 1000000;
				usleep(tiempo);				
				if(posicion_X<=108 && posicion_Y == 18){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X >=  108 && posicion_X <= 111 && posicion_Y >= 18 && posicion_Y <= 19){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 111 &&                      posicion_Y >= 20 && posicion_Y <= 29 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >  108 && posicion_X <= 115 && posicion_Y >= 29 && posicion_Y <  32){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  30 && posicion_X <= 115 && posicion_Y == 32){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X >=  19 && posicion_X <=  30 && posicion_Y <= 32 && posicion_Y >= 31){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  27 &&                      posicion_Y >= 21 && posicion_Y <= 30 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=  19 && posicion_X <=  28 && posicion_Y >= 18 && posicion_Y <= 20){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  26 && posicion_X <= 108 && posicion_Y == 18){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			break;
		case 8:
			for(j=0;j<=LONGITUD_CARRIL_8;j++){
								
				tirada = aleat_num(1,6,9);
				tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
				tiempo *= 1000000;
				usleep(tiempo);				
				if(posicion_X<=107 && posicion_Y == 20){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X >=  107 && posicion_X <= 109 && posicion_Y >= 20 && posicion_Y <= 19){ // primera bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y++;
				} else if (posicion_X == 108 &&                      posicion_Y >= 19 && posicion_Y <= 28 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >  107 && posicion_X <= 108 && posicion_Y >= 28 && posicion_Y <  30){ // segunda bajada inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y++;
				} else if (posicion_X >=  33 && posicion_X <= 107 && posicion_Y == 30){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X >=  31 && posicion_X <=  33 && posicion_Y <= 30 && posicion_Y >= 28){ // primera subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X--;
					posicion_Y--;
				} else if (posicion_X ==  30 &&                      posicion_Y >= 20 && posicion_Y <= 32 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=  19 && posicion_X <=  26 && posicion_Y >= 20 && posicion_Y <= 19){ // segunda subida inclinada
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_X++;
					posicion_Y--;
				} else if (posicion_X >=  26 && posicion_X <= 107 && posicion_Y == 20){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			break;
		case 9:
			for(j=0;j<=LONGITUD_CARRIL_9;j++){
								
				tirada = aleat_num(1,6,9);
				tiempo = (float)(1/((float)tirada*(float)((float)LONGITUD_CARRIL_0/(float)longitud)));
				tiempo *= 1000000;
				usleep(tiempo);				
				if(posicion_X<=104 && posicion_Y == 22){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				} else if (posicion_X == 105 &&                      posicion_Y >= 22 && posicion_Y <= 27 ){ // primera bajada vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y+1);
					printf("%d\n", num_pista);
					posicion_Y++;
				} else if (posicion_X >=  35 && posicion_X <= 105 && posicion_Y == 28){ // horizontal inferior
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X-1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X--;
				} else if (posicion_X ==  34 &&                      posicion_Y >= 22 && posicion_Y <= 28 ){ // primera subida vertical
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X,posicion_Y-1);
					printf("%d\n", num_pista);
					posicion_Y--;
				} else if (posicion_X >=  34 && posicion_X <= 105 && posicion_Y == 22){ // primera mitad recta
					gotoxy(posicion_X,posicion_Y);
					printf(" \n");
					gotoxy(posicion_X+1,posicion_Y);
					printf("%d\n", num_pista);
					posicion_X++;
				}
			}
			break;
		default:
			break;
			
	}

}

/*
void imprimir_caballos_clasificados(int longitud, int posicion_X, int posicion_Y, int num_pista){
	int j = 0;
	for(j=0;j<longitud;j++){
		
		if(70+j<=119){
			gotoxy(posicion_X,posicion_Y);
			printf(" \n");
			gotoxy(posicion_X+1,posicion_Y);
			printf("%d\n", num_pista);
			posicion_X++;
		} else if (70+j>119 && 70+j<=131){//119 a 132 
			gotoxy(posicion_X,posicion_Y);
			printf(" \n");
			gotoxy(posicion_X+1,posicion_Y+1);
			printf("%d\n", num_pista);
			posicion_X++;
			posicion_Y++;
		} else if (70+j>131 && posicion_Y <=34 ){ // 132 de fila 17 a 34
			gotoxy(posicion_X,posicion_Y);
			printf(" \n");
			gotoxy(posicion_X,posicion_Y+1);
			printf("%d\n", num_pista);
			posicion_Y++;
		}
	}
}
*/

/*
	printf("\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
*/
