#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "log_processor.h"

#ifndef NUM_THREADS
#define NUM_THREADS 8
#endif 

int main() {
    struct stat st;
    if (stat("access.log", &st) != 0) {
        perror("No se pudo encontrar access.log");
        return 1;
    }

    long file_size = st.st_size;
    long chunk_size = file_size / NUM_THREADS;
    
    pthread_t threads[NUM_THREADS];
    ThreadResult results[NUM_THREADS];
    
    clock_t start_time = clock();  // Para medir rendimiento [cite: 50]

    // 1. Creación de hilos y asignación de tareas [cite: 36, 42]
    for (int i = 0; i < NUM_THREADS; i++) {
        results[i].thread_id = i;
        results[i].start_offset = i * chunk_size;
        results[i].end_offset = (i == NUM_THREADS - 1) ? file_size : (i + 1) * chunk_size;
        
        // Crear tablas hash para cada hilo
        results[i].ip_table = create_hash_table();
        results[i].url_table = create_hash_table();
        results[i].error_count = 0;

        pthread_create(&threads[i], NULL, process_log_chunk, &results[i]);
    }

    // 2. Esperar a que todos los hilos terminen [cite: 18, 38]
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // 3. Combinar resultados [cite: 39]
    ThreadResult final_result;
    merge_results(&final_result, results, NUM_THREADS);
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // 4. Imprimir reporte final [cite: 19, 43]
    print_final_report(&final_result);
    printf("Tiempo de procesamiento: %.3f segundos\n", elapsed_time);

    // 5. Liberar memoria
    free_hash_table(final_result.ip_table);
    free_hash_table(final_result.url_table);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        free_hash_table(results[i].ip_table);
        free_hash_table(results[i].url_table);
    }
    
    return 0;
}