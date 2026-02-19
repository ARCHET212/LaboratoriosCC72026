#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

    pid_t pid;
    int i;

    printf("Parent Process: PID = %d\n", getpid());

    for (i = 0; i < 3; i++) {
        pid = fork();
        
        if (pid < 0) {
            printf("Error al crear el proceso\n");
            return 1;
        } else if (pid == 0) { 
            printf("Proceso hijo %d: PID = %d, Parent PID = %d\n", i+1, getpid(), getppid());
            return 0; 
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (i = 0; i < 3; i++) {
        wait(NULL);
    }

    return 0;
}