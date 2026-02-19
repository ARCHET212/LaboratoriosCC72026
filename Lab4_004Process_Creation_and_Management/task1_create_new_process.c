
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Erro al crear el proceso\n");
        return 1;
    } else if (pid == 0) { //Processo Hijo
        
        printf("Proceso hijo: PID = %d, Parent PID = %d\n", getpid(), getppid());
    } else { //Processo Padre
        sleep(1); // Espera para asegurar que el proceso hijo imprima primero
        printf("Proceso Padre: PID = %d\n", getpid());
    }
   
    return 0;
}