#include<stdio.h> 
#include<pthread.h> 
#include<unistd.h> 
#include<stdlib.h> 
#include<sys/shm.h>
#include<sys/stat.h>
#include<tgmath.h>

unsigned long long facto[4][5], arr[5][5];
int rC = 4, cC = 5, res, result;

unsigned long long fact(unsigned long long n){
    if(n==0 || n==1) return 1;
    else return n* fact(n-1);
}

void *factorial_(void *arg){
    for (int j = 0; j < 4; j++)  {
        for (int k = 0; k < 5; k++) {  
            printf("%llu\t\t\t", fact(arr[j][k]));
        }
        printf("\n");
    }
    pthread_exit(0);

}

int main() {
    key_t key = 12345;
    int (*matC)[5];

    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    matC = shmat(shmid, 0, 0);

    printf("Perkalian Matriks A dan Matriks B : \n");
    for (int i = 0; i < 4; i++) { 
        for (int j = 0; j < 5; j++)  {
            printf("%d ", matC[i][j]); 
        }
        printf("\n");
    } 

    for (int i = 0; i < 4; i++) { 
        for (int j = 0; j < 5; j++)  {
            arr[i][j] = matC[i][j]; 
        }
    } 

    pthread_t tid;
    pthread_attr_t attr; // set of thread attributes

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, factorial_, NULL);
    pthread_join(tid, NULL); 
}