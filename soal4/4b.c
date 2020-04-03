
#include<stdio.h> 
#include<pthread.h> 
#include<unistd.h> 
#include<stdlib.h> 
#include<sys/shm.h>
#include<sys/stat.h>
#include <sys/ipc.h>

int arr[5][5];
int res, result;

void *plus(void *arg) {
    printf("\nHasil Penambahan Matriks : \n");
    for (int i = 0; i < 4; i++)  {
        for (int j = 0; j < 5; j++) {  
            int n = arr[i][j];
            for (int k = 1; k <= n; k ++) {
                res += k;
            }
            arr[i][j] = res;
            printf("%d\t", res);
            res = 0;
        }
        printf("\n");
    }
    pthread_exit(0);
}

int main() {
    int rC = 4, cC = 5;
    int *matC;

    key_t key = 1234;
    int shmid = shmget(key, sizeof(int)*rC*cC, IPC_CREAT | 0666);
    matC = (int *)shmat(shmid, NULL, 0);

    printf("Perkalian Matriks A dan Matriks B : \n");
    for (int i = 0; i < rC; i++) { 
        for (int j = 0; j < cC; j++)  {
            printf("%d\t", matC[i*cC+j]); 
            arr[i][j] = matC[i*cC+j];
        }
        printf("\n");
    } 

    pthread_t threads[2]; 
    int* p;

    pthread_create(&threads[0], NULL, plus, (void*)(p));
    pthread_join(threads[0], NULL);

    shmdt(matC);
    shmctl(shmid, IPC_RMID, NULL);
}