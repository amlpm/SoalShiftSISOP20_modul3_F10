#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() 
{ 
	int p1[2];

	pid_t p;
    int count=0; 
    DIR * directory = opendir("/home/amelia/amel/SoalShiftSISOP20_modul3_F10/soal4");
    struct dirent * de;
    struct stat mystat;

	p = fork(); 

	if (p < 0) 
	{ 
		fprintf(stderr, "fork Failed" ); 
		return 1; 
	} 

	// Parent process 
	else if (p > 0) 
	{
        dup2(p1[0],0);
	    while((de = readdir(directory)) != NULL){
            if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
            if(de->d_type == DT_REG || de->d_type == DT_DIR){
                count++;
            }
        }
        printf("%d\n", count);
	} 

	// child process 
	else
	{ 
		dup2(p1[1], 1);
        close(p1[0]); 
        char buf[512];
        directory = opendir("/home/amelia/amel/SoalShiftSISOP20_modul3_F10/soal4");
        while((de = readdir(directory)) != NULL)
        {
            sprintf(buf, "%s/%s", "/home/amelia/amel/SoalShiftSISOP20_modul3_F10/soal4", de->d_name);
            stat(buf, &mystat);
            printf("%zu\t %s\n",mystat.st_size, de->d_name);
        }
        closedir(directory);
 	} 
} 