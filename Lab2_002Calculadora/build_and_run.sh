#!/bin/bash

# Detener el script si ocurre un error
set -e

# 1. Limpieza de archivos previos
echo "Cleaning up previous build files..."
rm -f *.o *.elf *.bin

# 2. Ensamblado del arranque (Nivel OS - Hardware)
echo "Assembling root.s..."
arm-none-eabi-as -o root.o os/root.s

# 3. Compilación de las capas
# Usamos -I para incluir las rutas de los archivos .h
echo "Compiling OS Level (os.c)..."
arm-none-eabi-gcc -c -Ios -Ilib -o os.o os/os.c

echo "Compiling Library Level (stdio.c and string.c)..."
arm-none-eabi-gcc -c -Ios -Ilib -o stdio.o lib/stdio.c
arm-none-eabi-gcc -c -Ios -Ilib -o string.o lib/string.c

echo "Compiling User Level (main.c)..."
arm-none-eabi-gcc -c -Ios -Ilib -o main.o user/main.c

# 4. Enlazado (Linker)
echo "Linking object files with libgcc..."
# Nota: linker.ld ahora está dentro de la carpeta os/
arm-none-eabi-gcc -nostartfiles -T os/linker.ld -o calculadora.elf root.o main.o os.o stdio.o string.o -lgcc

# 5. Generación del binario final 
echo "Converting ELF to binary..."
arm-none-eabi-objcopy -O binary calculadora.elf calculadora.bin

# 6. Ejecución en el emulador QEMU
echo "Running QEMU..."
qemu-system-arm -M versatilepb -nographic -kernel calculadora.elf