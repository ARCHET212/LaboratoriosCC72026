#ifndef OS_H
#define OS_H

void uart_putc(char c);
char uart_getc();
void uart_puts(const char *s);
void uart_gets_input(char *buffer, int max_length);
int uart_atoi(const char *str);
void uart_itoa(int num, char *buffer);
void uart_ftoa(float n, char *res, int precision);
float uart_atof(const char *s);


#endif