#ifndef imprimir_h 
#define imprimir_h 

#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

void imprimir_plantilla();
void imprimir_hipodromo();

#endif