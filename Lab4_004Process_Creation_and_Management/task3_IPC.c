#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

    int tuberia[2]; // tuberia[0] para lectura, tuberia[1] para escritura
    pid_t pid;
    char msm[]= "Hello from the parent!";
    char buffer[100];

    pipe(tuberia); // Crear la tubería
    pid = fork(); // Crear un nuevo proceso
    
    if (pid < 0) {
        printf("Error al crear el proceso\n");
        return 1;
    } else if (pid == 0) { // Proceso Hijo
        close(tuberia[1]); // Cerrar el extremo de escritura en el hijo
        read(tuberia[0], buffer, sizeof(buffer)); // Leer el mensaje del padre
        printf("Child Process: Received  \"%s\"\n", buffer);
        close(tuberia[0]); // Cerrar el extremo de lectura en el hijo
    } else { // Proceso Padre
        close(tuberia[0]); // Cerrar el extremo de lectura en el padre
        printf("Parent Process: Writing \"%s\"\n", msm);
        write(tuberia[1], msm, sizeof(msm)); // Escribir el mensaje al hijo
        close(tuberia[1]); // Cerrar el extremo de escritura en el padre
        wait(NULL); // Esperar a que el proceso hijo termine    
    }
    return 0;
}