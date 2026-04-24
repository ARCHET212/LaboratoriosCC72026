# Nombre del ejecutable final
TARGET = scheduling_lab

# Compilador a usar
CC = gcc

# Banderas de compilación (Warnings activados y estándar C11)
CFLAGS = -Wall -Wextra -std=c11

# Archivos fuente
SRC = Scheduling.c

# Regla por defecto: compila el programa
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo "Compilación completada. Para ejecutar: ./$(TARGET)"

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)
	@echo "Archivos temporales eliminados."

# Regla para ejecutar el programa después de compilar
run: all
	./$(TARGET)