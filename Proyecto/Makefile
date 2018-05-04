FLAG = -std=gnu99 -Wall -g	

all: semaforos.o utilidades.o caballo.o imprimir.o main 

main: main.c imprimir.o semaforos.o utilidades.o caballo.o gestor_apuestas.o shared_memory.o
	gcc -o main main.c caballo.o imprimir.o semaforos.o utilidades.o gestor_apuestas.o shared_memory.o -lpthread


testMem1: testMem1.c imprimir.o semaforos.o utilidades.o caballo.o gestor_apuestas.o shared_memory.o imprimir.o
	gcc -o testMem1 testMem1.c caballo.o imprimir.o semaforos.o utilidades.o gestor_apuestas.o shared_memory.o imprimir.o -lpthread
testMem2: testMem2.c imprimir.o semaforos.o utilidades.o caballo.o gestor_apuestas.o shared_memory.o
	gcc -o testMem2 testMem2.c caballo.o imprimir.o semaforos.o utilidades.o gestor_apuestas.o shared_memory.o -lpthread

imprimir.o: 
	gcc -c imprimir.c imprimir.h

semaforos.o: 
	gcc -c semaforos.c semaforos.h $(FLAG)

caballo.o: 
	gcc -c caballo.c caballo.h $(FLAG)

gestor_apuestas.o: gestor_apuestas.c gestor_apuestas.h 
	gcc -c gestor_apuestas.c gestor_apuestas.h $(FLAG)
	
shared_memory.o: shared_memory.h shared_memory.c caballo.o gestor_apuestas.o
	gcc -c shared_memory.c shared_memory.h caballo.o gestor_apuestas.o $(FLAG)



utilidades.o: 
	gcc -c utilidades.c utilidades.h $(FLAG)	


test: test.c utilidades.o caballo.o imprimir.o semaforos.o
	gcc -o test test.c utilidades.o caballo.o imprimir.o semaforos.o

clean:
	rm -f main test *h.gch *.o
