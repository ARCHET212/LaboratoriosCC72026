# 1. Definición de variables
CC = gcc
CFLAGS = -Wall -Wextra -g


# 2. Regla principal: compila el ejecutable
all: task1_create_new_process task2_synchronizing task3_IPC task4_multiple_child_process task5_shared_memory
 
 
 
# 3. Regla para ejecutar el programa directamente
task1: task1_create_new_process.c
		$(CC) $(CFLAGS) -o task1 task1_create_new_process.c
runt1: task1
		./task1


task2: task2_synchronizing.c
		$(CC) $(CFLAGS) -o task2 task2_synchronizing.c
runt2: task2
		./task2


task3: task3_IPC.c
		$(CC) $(CFLAGS) -o task3 task3_IPC.c
runt3: task3
		./task3

task4: task4_multiple_child_process.c
		$(CC) $(CFLAGS) -o task4 task4_multiple_child_process.c
runt4: task4
		./task4

task5: task5_shared_memory.c
		$(CC) $(CFLAGS) -o task5 task5_shared_memory.c
runt5: task5
		./task5
# 4. Regla para limpiar archivos generados
clean:
	rm -f task1 task2 task3 task4 task5
