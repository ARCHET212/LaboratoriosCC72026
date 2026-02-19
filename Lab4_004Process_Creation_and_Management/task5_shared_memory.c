#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <string.h>

int main() {

    int id_memoria = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);

    char *area_compartida = (char *)shmat(id_memoria, NULL, 0);

    if (fork() == 0) {
        
        sleep(1);
        printf("Child Process: Read \"%s\"\n", area_compartida); 
        shmdt(area_compartida); // Desconectar el área compartida en el proceso hijo
    } else {
        printf("Parent Process: Writing \"Shared Memory Example\"\n");
        strcpy(area_compartida, "Shared Memory Example"); // Escribir en el área compartida
        wait(NULL); // Esperar a que el proceso hijo termine
        shmdt(area_compartida); // Desconectar el área compartida en el proceso padre
        shmctl(id_memoria, IPC_RMID, NULL); // Eliminar el área compartida del sistema
    }

    return 0;
}