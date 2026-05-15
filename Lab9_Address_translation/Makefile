
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = mmu
SRC = addres_translation.c
INPUT = direcciones.txt
VPAGES = 20
SEED = 12346


all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)


run: $(TARGET)
	@if [ -f $(INPUT) ]; then \
		./$(TARGET) $(VPAGES) $(INPUT) $(SEED); \
	else \
		echo "Error: El archivo '$(INPUT)' no existe en esta carpeta."; \
	fi


clean:
	rm -f $(TARGET)


help:
	@echo "Comandos disponibles:"
	@echo "  make        - Compila el programa"
	@echo "  make run    - Compila y ejecuta con $(VPAGES) paginas y $(INPUT)"
	@echo "  make clean  - Borra el ejecutable"