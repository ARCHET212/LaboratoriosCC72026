#define UART0_BASE 0x101f1000

#define UART_DR      0x00  // Data Register
#define UART_FR      0x18  // Flag Register
#define UART_FR_TXFF 0x20  // Transmit FIFO Full
#define UART_FR_RXFE 0x10  // Receive FIFO Empty


volatile unsigned int * const UART0 = (unsigned int *)UART0_BASE;

// Function to send a single character via UART
void uart_putc(char c) {
    // Wait until there is space in the FIFO
    while (UART0[UART_FR / 4] & UART_FR_TXFF);
    UART0[UART_DR / 4] = c;
}

// Function to receive a single character via UART
char uart_getc() {
    // Wait until data is available
    while (UART0[UART_FR / 4] & UART_FR_RXFE);
    return (char)(UART0[UART_DR / 4] & 0xFF);
}

// Function to send a string via UART
void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

// Function to receive a line of input via UART
void uart_gets_input(char *buffer, int max_length) {
    int i = 0;
    char c;
    while (i < max_length - 1) { // Leave space for null terminator
        c = uart_getc();
        if (c == '\n' || c == '\r') {
            uart_putc('\n'); // Echo newline
            break;
        }
        uart_putc(c); // Echo character
        buffer[i++] = c;
    }
    buffer[i] = '\0'; // Null terminate the string
}

// Simple function to convert string to integer
int uart_atoi(const char *s) {
    int num = 0;
    int sign = 1;
    int i = 0;

    // Handle optional sign
    if (s[i] == '-') {
        sign = -1;
        i++;
    }

    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        num = num * 10 + (s[i] - '0');
    }

    return sign * num;
}

// Function to convert integer to string
void uart_itoa(int num, char *buffer) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0 && i < 14) { // Reserve space for sign and null terminator
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // Reverse the string
    int start = 0, end = i - 1;
    char temp;
    while (start < end) {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

// Función para convertir float a string (Nivel OS)
void uart_ftoa(float n, char *res, int precision) {
    if (n < 0) {
        *res++ = '-';
        n = -n;
    }

    int ipart = (int)n;
    float fpart = n - (float)ipart;

    uart_itoa(ipart, res); // Convierte la parte entera

    while (*res) res++; // Mueve el puntero al final del string

    if (precision > 0) {
        *res++ = '.';
        for (int i = 0; i < precision; i++) {
            fpart *= 10;
            int digit = (int)fpart;
            *res++ = digit + '0';
            fpart -= digit;
        }
    }
    *res = '\0';
}

// Convierte string a float (ASCII to Float)
float uart_atof(const char *s) {
    float res = 0.0;
    float divisor = 1.0;
    int dot_seen = 0;
    float sign = 1.0;

    // Manejar signo
    if (*s == '-') {
        sign = -1.0;
        s++;
    }

    for (; *s; s++) {
        if (*s == '.') {
            dot_seen = 1;
            continue;
        }
        if (*s < '0' || *s > '9') break; // Si hay basura (como un espacio), para

        res = res * 10.0 + (*s - '0');
        if (dot_seen) divisor *= 10.0;
    }
    return (res / divisor) * sign;
}