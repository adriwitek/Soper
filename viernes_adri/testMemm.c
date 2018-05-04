#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

typedef struct
{
  int num;
  int denom;
} Fractionn;

typedef struct
{
  int num;
  int denom;
  Fractionn * frac;
} Fraction;
 
 
int main()
{
  key_t key;
  int shmid;
 
  key = 5678;
  shmid = shmget(key, 5*sizeof(Fraction), IPC_CREAT | 0666);
  Fraction * frac = shmat(shmid, NULL, 0);
 
  frac
  printf("test\n");
  printf("FracArray %d\n", frac[1].num);
  printf("FracArray %d\n", frac[1].frac[1].num);
  frac[1].num = 5;
  frac[1].frac[1].num = 5;
   
 
  return 0;
}