#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define CYN   "\x1B[36m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"

#define NUM_CARS 10
#define MAX_PARK_TIME 5
#define MIN_PARK_TIME 1

sem_t parking_semaphore;
pthread_mutex_t log_mutex;
pthread_mutex_t stats_mutex;
pthread_mutex_t gui_mutex;

int total_cars_parked = 0;
double total_wait_time = 0.0;
int *parking_lot; 
int total_spaces;

// Función para dibujar un dashboard visualmente atractivo
void draw_dashboard() {
    pthread_mutex_lock(&gui_mutex);
    printf("\n" CYN "===============================================" RESET "\n");
    printf(CYN "   ESTADO ACTUAL DEL ESTACIONAMIENTO" RESET "\n");
    printf("   ");
    for (int i = 0; i < total_spaces; i++) {
        if (parking_lot[i] == -1) {
            printf(GRN "[ LIBRE ] " RESET);
        } else {
            printf(RED "[ CAR %02d ] " RESET, parking_lot[i]);
        }
    }
    printf("\n" CYN "===============================================" RESET "\n\n");
    pthread_mutex_unlock(&gui_mutex);
}

void get_timestamp(char *buffer, size_t size) {
    time_t raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(buffer, size, "%H:%M:%S", time_info);
}

void log_event(int car_id, const char *message, double wait_time, const char* color) {
    pthread_mutex_lock(&log_mutex);
    char timestamp[100];
    get_timestamp(timestamp, sizeof(timestamp));
    
    printf("%s[%s] Car %d: %s", color, timestamp, car_id, message);
    if (wait_time >= 0) printf(" (waited %.2f s)", wait_time);
    printf(RESET "\n");
    
    pthread_mutex_unlock(&log_mutex);
}

void* car_behavior(void* arg) {
    int car_id = *(int*)arg;
    struct timespec start_wait, end_wait;
    
    // Arribo 
    log_event(car_id, "Arrived at parking lot", -1, YEL);

    clock_gettime(CLOCK_REALTIME, &start_wait);
    sem_wait(&parking_semaphore); // Esperar espacio
    
    clock_gettime(CLOCK_REALTIME, &end_wait);
    double wait_time = (end_wait.tv_sec - start_wait.tv_sec) + 
                       (end_wait.tv_nsec - start_wait.tv_nsec) / 1e9;

    // Asignar lugar visual
    int spot_assigned = -1;
    pthread_mutex_lock(&gui_mutex);
    for (int i = 0; i < total_spaces; i++) {
        if (parking_lot[i] == -1) {
            parking_lot[i] = car_id;
            spot_assigned = i;
            break;
        }
    }
    pthread_mutex_unlock(&gui_mutex);

    // Park 
    log_event(car_id, "Parked successfully", wait_time, GRN);
    draw_dashboard();

    pthread_mutex_lock(&stats_mutex);
    total_cars_parked++;
    total_wait_time += wait_time;
    pthread_mutex_unlock(&stats_mutex);

    // Simular tiempo (1-5s) 
    sleep(MIN_PARK_TIME + rand() % (MAX_PARK_TIME - MIN_PARK_TIME + 1));

    // Leave 
    log_event(car_id, "Leaving parking lot", -1, RED);
    
    pthread_mutex_lock(&gui_mutex);
    parking_lot[spot_assigned] = -1; 
    pthread_mutex_unlock(&gui_mutex);
    
    sem_post(&parking_semaphore); //el espacio queda libre
    draw_dashboard();

    return NULL;
}

int main() {
    pthread_t cars[NUM_CARS];
    int car_ids[NUM_CARS];

    printf(CYN "╔═══════════════════════════════════════════╗" RESET "\n");
    printf(CYN "║    SMART PARKING SYSTEM - LAB 006         ║" RESET "\n");
    printf(CYN "╚═══════════════════════════════════════════╝" RESET "\n");
    
    printf("Ingrese el número de espacios (N): ");
    if (scanf("%d", &total_spaces) != 1 || total_spaces <= 0) return 1;

    parking_lot = malloc(total_spaces * sizeof(int));
    for (int i = 0; i < total_spaces; i++) parking_lot[i] = -1;

    srand(time(NULL));
    sem_init(&parking_semaphore, 0, total_spaces); 
    pthread_mutex_init(&log_mutex, NULL); 
    pthread_mutex_init(&stats_mutex, NULL); 
    pthread_mutex_init(&gui_mutex, NULL);

    for (int i = 0; i < NUM_CARS; i++) {
        car_ids[i] = i;
        pthread_create(&cars[i], NULL, car_behavior, &car_ids[i]); 
        usleep(rand() % 250000);
    }

    for (int i = 0; i < NUM_CARS; i++) pthread_join(cars[i], NULL);

    // Estadísticas finales [cite: 32]
    printf("\n" YEL "=== ESTADÍSTICAS FINALES ===" RESET "\n");
    printf("Total de carros estacionados: %d\n", total_cars_parked); 
    if (total_cars_parked > 0) {
        printf("Tiempo promedio de espera: " GRN "%.2f segundos" RESET "\n", 
                total_wait_time / total_cars_parked); 
    }

    free(parking_lot);
    sem_destroy(&parking_semaphore);
    pthread_mutex_destroy(&log_mutex);
    pthread_mutex_destroy(&stats_mutex);
    pthread_mutex_destroy(&gui_mutex);
    return 0;
}