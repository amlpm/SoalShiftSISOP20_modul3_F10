#include <unistd.h>
#include<pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <dirent.h> 
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

char fileName[100];

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

int isFileExists(char *path)
{
    printf("%s\n", path);
    char f[100];
    snprintf(f, sizeof(f), "/home/amelia/soal3/soal3/%s", path);
    DIR *directory = opendir("/home/amelia/soal3/soal3");
    struct dirent * direntp;
    int i = 0;
    while((direntp = readdir(directory)) != NULL){
        struct stat   buffer;   
        int exist = stat(f, &buffer);
        if(exist == 0) {
            printf("ada\n");
            return 1;
        }
        else {
            printf("Tidak ada\n");
            return 0;
        }
    }
}

void* isDirExists(void *arg) {
    struct stat st = {0};
    char dirName[100];
    snprintf(dirName, sizeof(dirName), "/home/amelia/amelia/%s", get_filename_ext(fileName));
    if (stat(dirName, &st) == -1)
    {
        mkdir(dirName, 0700);
    }
}

void* moveFile (void *arg) {
    FILE *fptr1, *fptr2; 
    char file[100], dest[100], c; 
  
    // Open one file for reading 
    snprintf(file, sizeof(file), "/home/amelia/soal3/soal3/%s", fileName);
    fptr1 = fopen(file, "r"); 
    if (fptr1 == NULL) 
    { 
        printf("Cannot open file %s \n", file); 
    }
  
    // Open another file for writing 
    snprintf(dest, sizeof(dest), "/home/amelia/amelia/%s/%s", get_filename_ext(fileName), fileName);
    fptr2 = fopen(dest, "w"); 

    while ((c = fgetc(fptr1)) != EOF) fputc(c, fptr2);
    
    remove(file);
    printf("Contents copied to %s\n", dest); 
}

int main (void) {
    FILE *fp; 
    char x[20];
    char a[20];
    char b[20];
    char c; 
    char *res;
    DIR *directory;
    struct dirent *de;
    pthread_t threads[100]; 

    printf("Masukkan Opsi : ");
    scanf("%s", x);
    strcpy(a, "-f");
    strcpy(b, "-d");

    directory = opendir("/home/amelia/soal3/soal3");
    if(directory){
        while ((de = readdir(directory)) != NULL){
            if(strcmp(x, a)==0) {
                printf("Masukkan nama file : ");
                scanf("%s", fileName);
                if(isFileExists(fileName)){
                    int* p;
                    int * p1; 
                    int x, y;

                    pthread_create(&threads[0], NULL, isDirExists, (void*)(p)); 
                    pthread_create(&threads[1], NULL, moveFile, (void*)(p)); 

                    pthread_join(threads[0], NULL);
                    pthread_join(threads[1], NULL);
                } 
            }
            else if (strcmp(x, "*") == 0) {
                int file_count = 0;

                directory = opendir("/home/amelia/soal3/soal3"); /* There should be error handling after this */
                while ((de = readdir(directory)) != NULL) {
                    if (de->d_type == DT_REG) { /* If the dentry is a regular file */
                        file_count++;
                    }
                }
                printf("%d\n", file_count);
                int arr[100];
                for (int i = 0; i < file_count; i++) {
                    fileName[i] = de->d_name; 
                }
                for (int i = 0; i < file_count; i++) { 
                    int* p; 
                    pthread_create(&threads[i], NULL, isDirExists, (void*)(p)); 
                } 
                for (int i = 0; i < file_count; i++)  pthread_join(threads[i], NULL);  
            }
            else if (strcmp(x, b)){

            }
        }
    }
}