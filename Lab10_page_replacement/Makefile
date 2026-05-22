# Nombre del compilador
CC = gcc

# Banderas de compilación (activan advertencias útiles)
CFLAGS = -Wall -Wextra -std=c99

# Nombre del archivo fuente y del ejecutable final
TARGET = page_replacement
SRC = page_replacement.c

# Regla por defecto: compila el programa
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# NUEVA REGLA: Compila (si es necesario) y ejecuta el programa inmediatamente
run: $(TARGET)
	./$(TARGET)

# Regla para limpiar el ejecutable generado
clean:
	rm -f $(TARGET)