# Nombre del archivo de salida
TARGET = zombie_bridge

# Compilador
CC = gcc

# Banderas de compilación (-lpthread es necesaria para hilos POSIX)
CFLAGS = -Wall -Wextra -lpthread

# Regla principal: compila el programa
all: $(TARGET)

$(TARGET): zombie_bridge.c
	$(CC) zombie_bridge.c -o $(TARGET) $(CFLAGS)

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)

# Regla para ejecutar el programa después de compilar
run: all
	./$(TARGET)