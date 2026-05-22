#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_REF 100
#define INF 999999

// Estructura para guardar los resultados finales de cada algoritmo
typedef struct {
    int hits;
    int misses;
} Resultado;

// Funciones de utilidad
bool es_numero(const char *str) {
    if (*str == '\0') return false;
    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

void imprimir_marcos(int frames[], int N) {
    printf("[");
    for (int i = 0; i < N; i++) {
        if (frames[i] == -1) {
            printf("_");
        } else {
            printf("%d", frames[i]);
        }
        if (i < N - 1) printf(", ");
    }
    printf("]");
}

// ==========================================
// ALGORITMO FIFO (Tarea 2 y Tarea 3) 
// Modificado para soportar la traza oculta en la Demo de Belády
// ==========================================
Resultado simular_fifo(int N, int secuencia[], int tam, bool mostrar_traza) {
    Resultado res = {0, 0};
    int frames[N];
    for (int i = 0; i < N; i++) frames[i] = -1; // -1 significa vacío

    int indice_insertar = 0; // Apunta al elemento más antiguo (cola circular) 

    if (mostrar_traza) {
        printf("\n==================================================\n");
        printf("FIFO (N=%d)\n", N);
        printf("==================================================\n");
        printf("%-6s %-5s %-8s %-20s %-6s\n", "step", "ref", "result", "frames[0..N-1]", "victim");
        printf("--------------------------------------------------\n");
    }

    for (int step = 0; step < tam; step++) {
        int ref = secuencia[step];
        bool hit = false;
        int victim = -1;

        // Verificar si ya está en memoria 
        for (int i = 0; i < N; i++) {
            if (frames[i] == ref) {
                hit = true;
                break;
            }
        }

        if (hit) {
            res.hits++;
            if (mostrar_traza) {
                printf("%-6d %-5d %-8s ", step + 1, ref, "HIT");
                imprimir_marcos(frames, N);
                printf(" %-20s %-6s\n", "", "-");
            }
        } else {
            res.misses++; 
            
            // Si la memoria está llena, el que está en 'indice_insertar' es la víctima 
            if (frames[indice_insertar] != -1) {
                victim = frames[indice_insertar]; 
            }

            // Reemplazar o insertar en la posición correspondiente 
            frames[indice_insertar] = ref;
            // Mover el índice de forma circular (el siguiente más antiguo) 
            indice_insertar = (indice_insertar + 1) % N;

            if (mostrar_traza) {
                printf("%-6d %-5d %-8s ", step + 1, ref, "MISS");
                imprimir_marcos(frames, N);
                if (victim != -1) {
                    printf(" %-20s %-6d\n", "", victim);
                } else {
                    printf(" %-20s %-6s\n", "", "-");
                }
            }
        }
    }
    if (mostrar_traza) {
        double hit_rate = ((double)res.hits / (res.hits + res.misses)) * 100; 
        printf("\nTotals: hits=%d misses=%d | Hit Rate: %.2f%%\n", res.hits, res.misses, hit_rate); 
    } 
    return res;
}

// ==========================================
// ALGORITMO MIN / ÓPTIMO (OPT) (Tarea 2) 
// ==========================================
Resultado simular_min(int N, int secuencia[], int tam) {
    Resultado res = {0, 0};
    int frames[N];
    for (int i = 0; i < N; i++) frames[i] = -1; 

    printf("\n==================================================\n");
    printf("MIN / Optimal (N=%d)\n", N);
    printf("==================================================\n");
    printf("%-6s %-5s %-8s %-20s %-6s\n", "step", "ref", "result", "frames[0..N-1]", "victim");
    printf("--------------------------------------------------\n");

    for (int step = 0; step < tam; step++) {
        int ref = secuencia[step];
        bool hit = false;

        for (int i = 0; i < N; i++) {
            if (frames[i] == ref) {
                hit = true;
                break;
            }
        }

        if (hit) {
            res.hits++;
            printf("%-6d %-5d %-8s ", step + 1, ref, "HIT");
            imprimir_marcos(frames, N);
            printf(" %-20s %-6s\n", "", "-");
        } else {
            res.misses++;
            int victim = -1;
            int indice_reemplazo = -1;

            // Ver si hay un espacio vacío primero 
            for (int i = 0; i < N; i++) {
                if (frames[i] == -1) {
                    indice_reemplazo = i;
                    break;
                }
            }

            // Si está lleno, buscar la página que tarda más en usarse en el futuro 
            if (indice_reemplazo == -1) {
                int max_futuro = -1;
                
                for (int i = 0; i < N; i++) {
                    int j;
                    // Buscar la próxima aparición estrictamente después del paso actual 
                    for (j = step + 1; j < tam; j++) {
                        if (secuencia[j] == frames[i]) break;
                    }

                    // Tie-break: Si empatan en distancia o ninguno vuelve a aparecer (j == tam) 
                    // Se aplica el desempate determinista documentado: Menor ID de página 
                    if (j > max_futuro) {
                        max_futuro = j;
                        indice_reemplazo = i;
                    } else if (j == max_futuro) {
                        if (frames[i] < frames[indice_reemplazo]) {
                            indice_reemplazo = i;
                        }
                    }
                }
                victim = frames[indice_reemplazo]; 
            }

            frames[indice_reemplazo] = ref;

            printf("%-6d %-5d %-8s ", step + 1, ref, "MISS");
            imprimir_marcos(frames, N);
            if (victim != -1) {
                printf(" %-20s %-6d\n", "", victim);
            } else {
                printf(" %-20s %-6s\n", "", "-");
            }
        }
    }
    double hit_rate = ((double)res.hits / (res.hits + res.misses)) * 100;
    printf("\nTotals: hits=%d misses=%d | Hit Rate: %.2f%%\n", res.hits, res.misses, hit_rate); 
    return res;
}

// ==========================================
// ALGORITMO LRU (Tarea 2)
// ==========================================
Resultado simular_lru(int N, int secuencia[], int tam) {
    Resultado res = {0, 0};
    int frames[N];
    int ultimo_acceso[N]; // Guarda el número de paso (step) del último acceso de cada celda
    
    for (int i = 0; i < N; i++) {
        frames[i] = -1;
        ultimo_acceso[i] = -1;
    }

    printf("\n==================================================\n");
    printf("LRU (N=%d)\n", N);
    printf("==================================================\n");
    printf("%-6s %-5s %-8s %-20s %-6s\n", "step", "ref", "result", "frames[0..N-1]", "victim");
    printf("--------------------------------------------------\n");

    for (int step = 0; step < tam; step++) {
        int ref = secuencia[step];
        bool hit = false;

        for (int i = 0; i < N; i++) {
            if (frames[i] == ref) {
                hit = true;
                ultimo_acceso[i] = step; // Actualizar recencia tras el hit 
                break;
            }
        }

        if (hit) {
            res.hits++;
            printf("%-6d %-5d %-8s ", step + 1, ref, "HIT");
            imprimir_marcos(frames, N);
            printf(" %-20s %-6s\n", "", "-");
        } else {
            res.misses++;
            int victim = -1;
            int indice_reemplazo = -1;

            // Ver si hay espacio vacío 
            for (int i = 0; i < N; i++) {
                if (frames[i] == -1) {
                    indice_reemplazo = i;
                    break;
                }
            }

            // Si está lleno, buscar el que tenga el menor 'ultimo_acceso' (menos recientemente usado) 
            if (indice_reemplazo == -1) {
                int min_tiempo = INF;
                for (int i = 0; i < N; i++) {
                    if (ultimo_acceso[i] < min_tiempo) {
                        min_tiempo = ultimo_acceso[i];
                        indice_reemplazo = i;
                    } 
                    // Tie-break: En caso de empate teórico estricto, menor ID de página 
                    else if (ultimo_acceso[i] == min_tiempo) {
                        if (frames[i] < frames[indice_reemplazo]) {
                            indice_reemplazo = i;
                        }
                    }
                }
                victim = frames[indice_reemplazo]; 
            }

            frames[indice_reemplazo] = ref;
            ultimo_acceso[indice_reemplazo] = step; // Registrar acceso de la nueva página 

            printf("%-6d %-5d %-8s ", step + 1, ref, "MISS");
            imprimir_marcos(frames, N);
            if (victim != -1) {
                printf(" %-20s %-6d\n", "", victim);
            } else {
                printf(" %-20s %-6s\n", "", "-");
            }
        }
    }
    double hit_rate = ((double)res.hits / (res.hits + res.misses)) * 100; 
    printf("\nTotals: hits=%d misses=%d | Hit Rate: %.2f%%\n", res.hits, res.misses, hit_rate); 
    return res;
}

// ==========================================
// EXTRA POINTS: Demostración de la Anomalía de Belády 
// ==========================================
void demostrar_anomalia_belady() {
    printf("\n==================================================\n");
    printf("   BONUS: DEMOSTRACION DE LA ANOMALIA DE BELADY\n");
    printf("==================================================\n");
    printf("La anomalia de Belady demuestra que, en FIFO, aumentar\n");
    printf("el numero de marcos (N) puede INCREMENTAR los fallos.\n\n");

    // Secuencia clásica de Belády
    int seq_belady[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    int tam_belady = 12;

    printf("Secuencia utilizada: 1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5\n\n");

    // Ejecutar con N = 3 (Falso indica que oculte el paso a paso de la traza)
    Resultado r_N3 = simular_fifo(3, seq_belady, tam_belady, false);
    // Ejecutar con N = 4
    Resultado r_N4 = simular_fifo(4, seq_belady, tam_belady, false);

    printf("Resultados obtenidos con FIFO:\n");
    printf(" -> Para N = 3 marcos: Misses = %d\n", r_N3.misses);
    printf(" -> Para N = 4 marcos: Misses = %d (¡Aumento!)\n", r_N4.misses);
    printf("==================================================\n");
}


int main() {
    char n_input[20];
    char seq_input[500];
    int N;
    int secuencia[MAX_REF];
    int tam = 0;

    // Tarea 1: Interfaz interactiva y validaciones 
    printf("--- Simulador de Reemplazo de Paginas (C Version) ---\n");
    printf("Ingrese el numero de marcos fisicos (N >= 1): "); 
    if (scanf("%19s", n_input) != 1) return 1;

    if (!es_numero(n_input) || atoi(n_input) < 1) {
        printf("Error: El numero de marcos (N) debe ser un entero mayor o igual a 1.\n"); 
        return 1;
    }
    N = atoi(n_input);

    printf("\nEjemplo de secuencia recomendada por el lab:\n7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1\n"); 
    printf("Ingrese la secuencia (separada por espacios): "); 
    
    // Limpiar el buffer de entrada antes de leer la línea de tokens
    while (getchar() != '\n'); 
    if (fgets(seq_input, sizeof(seq_input), stdin) == NULL) return 1;

    // Tokenización y validación estricta de strings 
    char *token = strtok(seq_input, " ,\n");
    if (token == NULL) {
        printf("Error: La secuencia de referencia no puede estar vacia.\n"); 
        return 1;
    }

    while (token != NULL) {
        if (!es_numero(token)) {
            printf("Error: Token no numerico o negativo encontrado ('%s').\n", token); 
            return 1;
        }
        secuencia[tam++] = atoi(token);
        if (tam >= MAX_REF) break;
        token = strtok(NULL, " ,\n");
    }

    // Ejecución secuencial obligatoria (Tarea 2 y Tarea 3) 
    Resultado r_fifo = simular_fifo(N, secuencia, tam, true); // true activa la traza en pantalla
    Resultado r_min  = simular_min(N, secuencia, tam);
    Resultado r_lru  = simular_lru(N, secuencia, tam);

    // Bloque de Resumen Comparativo Final en Tabla ASCII (Recomendado) 
    printf("\n==================================================\n");
    printf("TABLA COMPARATIVA DE RESUMEN\n");
    printf("==================================================\n");
    printf("%-15s | %-10s | %-10s | %-10s\n", "Algoritmo", "Hits", "Misses", "Hit Rate");
    printf("-----------------------------------------------------\n");
    printf("%-15s | %-10d | %-10d | %.2f%%\n", "FIFO", r_fifo.hits, r_fifo.misses, ((double)r_fifo.hits / (r_fifo.hits + r_fifo.misses)) * 100);
    printf("%-15s | %-10d | %-10d | %.2f%%\n", "MIN (Optimal)", r_min.hits, r_min.misses, ((double)r_min.hits / (r_min.hits + r_min.misses)) * 100);
    printf("%-15s | %-10d | %-10d | %.2f%%\n", "LRU", r_lru.hits, r_lru.misses, ((double)r_lru.hits / (r_lru.hits + r_lru.misses)) * 100);
    printf("==================================================\n");

  
    demostrar_anomalia_belady();

    return 0;
}   