#include<stdio.h> 
#include<pthread.h> 
#include<unistd.h> 
#include<stdlib.h> 
#include<sys/shm.h>
#include<sys/stat.h>

int matA[4][2], rA = 4, cA = 2; 
int matB[2][5], rB = 2, cB = 5;
int (*matC)[5];
int step_i = 0, sum = 0;
pthread_t threads[10], t1, t2; 
// int* p1;
// int* p2;
int* p; 

// void* addA(void * arg) {
//     printf("Masukkan Matriks A : \n");
//     for (int i = 0; i < 4; i++) { 
// 		for (int j = 0; j < 2; j++) {
// 	    	scanf("%d", &matA[i][j]); 
//         }
// 	}

// 	printf("\nMatriks A\n");
// 	for (int i = 0; i < 4; i++) { 
// 		for (int j = 0; j < 2; j++) {
// 	    	printf("%d ", matA[i][j]); 
//         }
//         printf("\n"); 
// 	}
//     printf("\n"); 

// }

// void* addB(void * arg) {
//     printf("Masukkan Matriks B : \n");
//     for (int i = 0; i < 2; i++) { 
// 		for (int j = 0; j < 5; j++) {
// 	    	scanf("%d", &matB[i][j]); 
//         }
// 	}

// 	printf("\nMatriks B\n");
// 	for (int i = 0; i < 2; i++) { 
// 		for (int j = 0; j < 5; j++) {
// 	    	printf("%d ", matB[i][j]); 
//         }
//         printf("\n"); 
// 	}
// }

void* multi(void* arg) 
{ 
    int core = step_i++; 
  
    // Each thread computes 1/4th of matrix multiplication 
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {  
            for (int k = 0; k < 2; k++) {  
                sum = sum + matA[i][k] * matB[k][j];
            }
            matC[i][j] = sum;
            sum = 0;
        }
    } 
} 

int main(void) 
{ 
    key_t key = 12345;
    int shmid = shmget(key, sizeof(int[5][5]), IPC_CREAT | 0666);
    matC = shmat(shmid, 0, 0);

    matA[0][0] = 1;
	matA[0][1] = 2;
	matA[1][0] = 3;
	matA[1][1] = 4;
	matA[2][0] = 5;
	matA[2][1] = 6;
	matA[3][0] = 7;
	matA[3][1] = 2;

	matB[0][0] = 3;
	matB[0][1] = 4;
	matB[0][2] = 5;
	matB[0][3] = 6;
	matB[0][4] = 1;
	matB[1][0] = 5;
	matB[1][1] = 4;
	matB[1][2] = 3;
	matB[1][3] = 2;
	matB[1][4] = 1;

	// pthread_create(&t1, NULL, addA, (void*)(p1)); 
    // pthread_join(t1, NULL);  
    // pthread_create(&t2, NULL, addB, (void*)(p2));
    // pthread_join(t2, NULL);     
  
    for (int i = 0; i < 4; i++) pthread_create(&threads[i], NULL, multi, (void*)(p)); 
  
    for (int i = 0; i < 4; i++)  pthread_join(threads[i], NULL);     

    printf("\nPerkalian Matriks A dan Matriks B : \n");
    for (int i = 0; i < 4; i++) { 
        for (int j = 0; j < 5; j++)  {
            printf("%d ", matC[i][j]); 
        }
        printf("\n");
    } 
} 