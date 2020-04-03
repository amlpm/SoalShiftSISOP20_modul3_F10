#include<stdio.h> 
#include<pthread.h> 
#include<unistd.h> 
#include<stdlib.h> 
#include<sys/shm.h>
#include<sys/stat.h>
#include <sys/ipc.h>

int matA[4][2], rA = 4, cA = 2; 
int matB[2][5], rB = 2, cB = 5;
int *matC;
int result[4][5];
int step_i = 0, sum = 0;

void* multi(void* arg) 
{ 
    int core = step_i++; 
    for (int i = core * 4 / 4; i < (core + 1) * 4 / 4; i++)  {
        for (int j = 0; j < 5; j++)  {
            for (int k = 0; k < 2; k++) {  
                result[i][j] += matA[i][k] * matB[k][j]; 
            }
        }
    }
} 

int main(void) 
{ 
    int rC = 4, cC = 5;

    key_t key = 1234;
    int shmid = shmget(key, sizeof(int)*rC*cC, IPC_CREAT | 0666);
    matC = (int *)shmat(shmid, NULL, 0);

    matA[0][0] = 1;
	matA[0][1] = 2;
	matA[1][0] = 8;
	matA[1][1] = 1;
	matA[2][0] = 2;
	matA[2][1] = 1;
	matA[3][0] = 3;
	matA[3][1] = 1;

	matB[0][0] = 2;
	matB[0][1] = 1;
	matB[0][2] = 3;
	matB[0][3] = 4;
	matB[0][4] = 1;
	matB[1][0] = 2;
	matB[1][1] = 3;
	matB[1][2] = 2;
	matB[1][3] = 1;
	matB[1][4] = 9;
    
    pthread_t threads[4]; 
  
    for (int i = 0; i < 4; i++) { 
        int* p; 
        pthread_create(&threads[i], NULL, multi, (void*)(p)); 
    } 
  
    for (int i = 0; i < 4; i++)  
        pthread_join(threads[i], NULL);     
  
    printf("Perkalian Matriks A dan Matriks B : \n");
    for (int i = 0; i < rC; i++) { 
        for (int j = 0; j < cC; j++)  {
            matC[i*cC+j] = result[i][j];
            printf("%d\t", matC[i*cC+j]);
        }
        printf("\n");
    } 
    sleep(10);

    shmdt(matC);
    shmctl(shmid, IPC_RMID, NULL);
} 