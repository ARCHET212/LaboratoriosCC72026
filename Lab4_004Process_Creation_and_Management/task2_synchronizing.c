#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Erro al crear el proceso\n");
        return 1;
    } else if (pid == 0) { //Processo Hijo
        
        printf("Child Process: PID = %d, Parent PID = %d\n", getpid(), getppid());
        
    } else { //Processo Padre
        waitpid(pid, NULL, 0); // Espera a que el proceso hijo termine
        printf("Parent Process: Child has finished execution\n");
    }
   
    return 0;
}