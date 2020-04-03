
#include<stdio.h> 
#include<pthread.h> 
#include<unistd.h> 
#include<stdlib.h> 
#include<sys/shm.h>
#include<sys/stat.h>
#include <sys/ipc.h>

int arr[5][5];
int res, result;


void *plus(void *arg){
    printf("\nMatriks Penambahan : \n");

    for (int i = 0; i < 4; i++)  {
        for (int j = 0; j < 5; j++) {  
            int n = arr[i][j];
            for (int k = 1; k <= n; k ++) {
                res += k;
            }
            printf("%d ", res);
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
            printf("%d ", matC[i*cC+j]); 
            arr[i][j] = matC[i*cC+j];
        }
        printf("\n");
    } 

    pthread_t tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, plus, NULL);
    pthread_join(tid, NULL); 

    shmdt(matC);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}