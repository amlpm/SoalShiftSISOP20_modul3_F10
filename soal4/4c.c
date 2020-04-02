#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include<sys/wait.h> 
#include <sys/types.h>
#include<string.h> 

int main () {
    char *parameter[]={"ls",NULL};

    int p[2], count = 0, i;
    pipe(p);
    pid_t p1;

    p1 = fork();

    if (p1 < 0) 
	{ 
		fprintf(stderr, "fork gagal" ); 
		return 1; 
	}

    else if (p1 > 0){ // parent
        dup2(p[0],0);

        char buffer[500];
        read(STDIN_FILENO, buffer, 500);

        for(i = 0; buffer[i] != '\0';i++){
            if (buffer[i] == '\n') count++;
        }
        printf("%d\n", count);
		
    }

    else { // child 
        dup2(p[1], 1);
        close(p[0]);
        execvp("ls", parameter);
        perror("execvp ls gagal");
    }
    close(p[1]);
}