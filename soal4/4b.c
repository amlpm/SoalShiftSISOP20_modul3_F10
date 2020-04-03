
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
    for (int i = 0; i < 4; i++)  {
        for (int j = 0; j < 5; j++) {  
            int n = arr[i][j];
            for (int k = 1; k <= n; k ++) {
                res += k;
            }
            arr[i][j] = res;
        }
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
            printf("%d ", matC[i*cC+j]); 
            arr[i][j] = matC[i*cC+j];
        }
        printf("\n");
    } 

    pthread_t threads[30]; 

    for (int i = 0; i < 20; i++) { 
        int* p; 
        pthread_create(&threads[i], NULL, plus, (void*)(p)); 
    } 
  
    for (int i = 0; i < 20; i++)  
        pthread_join(threads[i], NULL);     

    printf("\nHasil penambahan matriks : \n");
    for (int i = 0; i < rC; i++) { 
        for (int j = 0; j < cC; j++)  {
            printf("%d ",arr[i][j]); 
        }
        printf("\n");
    } 

    shmdt(matC);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}