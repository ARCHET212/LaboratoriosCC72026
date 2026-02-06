# Variables para el compilador y herramientas
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
QEMU = qemu-system-arm

# Opciones de compilación
CFLAGS = -c
LDFLAGS = -T memmap.ld

# Nombre del archivo de salida
TARGET = hello.elf

# Regla principal: compila todo
all: $(TARGET)

$(TARGET): startup.o hello.o
	$(LD) $(LDFLAGS) startup.o hello.o -o $(TARGET) 

startup.o: startup.s
	$(CC) $(CFLAGS) startup.s -o startup.o 

hello.o: hello.c
	$(CC) $(CFLAGS) hello.c -o hello.o 

# Regla para ejecutar en QEMU
run: $(TARGET)
	$(QEMU) -M versatilepb -nographic -kernel $(TARGET)

# Regla para limpiar los archivos generados
clean:
	rm -f *.o $(TARGET)