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

const char * ext;
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
    DIR *directory = opendir("/home/amelia/soal3/soal3"); //specify the directory name
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
    snprintf(dirName, sizeof(dirName), "./%s", get_filename_ext(fileName));
    if (stat(ext, &st) == -1)
    {
        mkdir(ext, 0700);
    }

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
    if (fptr2 == NULL) 
    { 
        printf("Cannot read file %s \n", dest); 
    } 

    while ((c = fgetc(fptr1)) != EOF) fputc(c, fptr2);
    
    remove(file);
    printf("\nContents copied to %s", dest); 

    fclose(fptr2); 
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
                    pthread_t threads[2]; 
                    int* p;
                    int * p1; 
                    int x, y;

                    pthread_create(&threads[0], NULL, isDirExists, (void*)(p)); 
                    pthread_join(threads[0], NULL);
                } 
            }
        }
    }
}
