#include <stdarg.h>
#include "stdio.h"
#include "os.h"

void PRINT(const char *format, ...) {
    va_list lista;           // Aquí guardaremos los argumentos extra
    va_start(lista, format); // Empezamos a leer después de la cadena 'format'

    //Revisar el texto letra por letra
    for (int i = 0; format[i] != '\0'; i++) {
        
        
        if (format[i] == '%') {
            i++;
            
            if (format[i] == 'd') {
                // Es un número entero
                int num = va_arg(lista, int);
                char texto[16];
                uart_itoa(num, texto);
                uart_puts(texto);
            } 
            else if (format[i] == 'f') {
                // Es un número con decimales
                double decimal = va_arg(lista, double);
                char texto[32];
                uart_ftoa((float)decimal, texto, 2);
                uart_puts(texto);
            }
            else if (format[i] == 's') {
                // Es una cadena de texto (string)
                char *str = va_arg(lista, char *);
                uart_puts(str);
            }
            else if (format[i] == 'c') {
                // Es un solo carácter
                char car = (char)va_arg(lista, int);
                uart_putc(car);
            }
        } 
        else {
           //imprimimos la letra tal cual
            uart_putc(format[i]);
        }
    }

    va_end(lista); // Limpiar la lista al terminar
}

void READ(const char *format, ...) {
    va_list lista;
    va_start(lista, format);

    char mi_buffer[64]; 
    
    // Recorremos el formato letra por letra
    for (int i = 0; format[i] != '\0'; i++) {
        
        if (format[i] == '%') {
            i++;
            
            // Cada vez que vemos un %, pedimos una entrada del teclado
            uart_gets_input(mi_buffer, 64);

            if (format[i] == 'd') {
                // Sacamos la "dirección de memoria" de la variable (el puntero)
                int *donde_guardar = va_arg(lista, int *);
                // Convertimos el texto del buffer a número y lo guardamos ahí
                *donde_guardar = uart_atoi(mi_buffer);
            } 
            else if (format[i] == 'f') {
                // Lo mismo para decimales
                float *donde_guardar_f = va_arg(lista, float *);
                *donde_guardar_f = uart_atof(mi_buffer);
            }
          
        }
    }

    va_end(lista);
}