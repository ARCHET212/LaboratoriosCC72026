#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MAX_CAPACITY 4
#define TOTAL_STUDENTS 10
#define MAX_CONSECUTIVE 5 

// --- Estado Global ---
pthread_mutex_t bridge_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_right = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_left = PTHREAD_COND_INITIALIZER;

int on_bridge = 0;
int current_direction = -1;
int waiting_right = 0;
int waiting_left = 0;
int consecutive_count = 0; // Contador para evitar starvation

double total_wait_time = 0;
int students_crossed = 0;

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// --- Sincronización con Prevención de Inanición ---

void accessBridge(int direction, int id, double arrival_time) {
    pthread_mutex_lock(&bridge_mutex);
    
    if (direction == 0) {
        waiting_right++;
        // Bloqueo si: puente lleno O dirección opuesta O (turno agotado Y hay gente esperando al otro lado) 
        while (on_bridge == MAX_CAPACITY || (current_direction == 1) || 
              (current_direction == 0 && consecutive_count >= MAX_CONSECUTIVE && waiting_left > 0)) {
            pthread_cond_wait(&cond_right, &bridge_mutex);
        }
        waiting_right--;
    } else {
        waiting_left++;
        while (on_bridge == MAX_CAPACITY || (current_direction == 0) || 
              (current_direction == 1 && consecutive_count >= MAX_CONSECUTIVE && waiting_right > 0)) {
            pthread_cond_wait(&cond_left, &bridge_mutex);
        }
        waiting_left--;
    }

    // Si el puente estaba vacío o cambia de dirección, reiniciamos contador
    if (current_direction != direction) {
        consecutive_count = 0;
    }

    consecutive_count++;
    on_bridge++;
    current_direction = direction;

    double entry_time = get_time();
    total_wait_time += (entry_time - arrival_time);
    students_crossed++;

    printf("Inge %02d crosses to the %s (on bridge: %d, consec: %d)\n", 
           id, (direction == 0 ? "Right" : "Left"), on_bridge, consecutive_count);

    pthread_mutex_unlock(&bridge_mutex);
}

void exitBridge(int id) {
    pthread_mutex_lock(&bridge_mutex);

    on_bridge--;
    printf("Inge %02d exits bridge (on bridge: %d)\n", id, on_bridge);

    if (on_bridge == 0) {
        // Si alcanzamos el límite de consecutivos y hay alguien esperando al otro lado, cambiamos forzosamente 
        if (consecutive_count >= MAX_CONSECUTIVE) {
            current_direction = -1;
            consecutive_count = 0;
            // Despertamos a todos para que el lado que esperaba tome el control
            pthread_cond_broadcast(&cond_right);
            pthread_cond_broadcast(&cond_left);
        } else {
            current_direction = -1;
            pthread_cond_broadcast(&cond_right);
            pthread_cond_broadcast(&cond_left);
        }
    } else {
        // Aún hay gente, solo despertamos a los de la misma dirección si no hemos superado el límite
        if (current_direction == 0) pthread_cond_signal(&cond_right);
        else pthread_cond_signal(&cond_left);
    }

    pthread_mutex_unlock(&bridge_mutex);
}

// --- Lógica de Hilos ---

void* student_thread(void* arg) {
    int id = *((int*)arg);
    free(arg);

    sleep(rand() % 6); // Arribo aleatorio 
    double arrival_time = get_time();
    int direction = rand() % 2; // Dirección aleatoria 

    accessBridge(direction, id, arrival_time);
    sleep((rand() % 3) + 1); // Tiempo cruzando 
    exitBridge(id);

    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t students[TOTAL_STUDENTS];

    printf("--- Zombie Bridge: Iniciando con Prevención de Inanición ---\n\n");

    for (int i = 1; i <= TOTAL_STUDENTS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&students[i-1], NULL, student_thread, id);
    }

    for (int i = 0; i < TOTAL_STUDENTS; i++) {
        pthread_join(students[i], NULL);
    }

    printf("\n--- Reporte Final ---\n");
    printf("Tiempo promedio de espera: %.2f segundos\n", total_wait_time / TOTAL_STUDENTS);

    return 0;
}