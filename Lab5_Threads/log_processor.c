#include "log_processor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// FUNCIONES DE HASH TABLE

/**
 * Función hash simple (djb2)
    -inicia con numero de semilla 5381
    -para cada caracter, multiplica el hash por 33 y suma el valor del caracter
    -devuelve el hash modulado por el tamaño de la tabla
 */
unsigned int hash_function(const char* key) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash % HASH_TABLE_SIZE;
}

/**
 * Crear una nueva tabla hash vacía
 */
HashTable* create_hash_table() {
    HashTable* table = malloc(sizeof(HashTable));
    if (!table) return NULL;
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    table->size = 0;
    
    return table;
}

/**
 * Buscar una entrada en la tabla hash
 */
HashEntry* hash_table_find(HashTable* table, const char* key) {
    unsigned int index = hash_function(key);
    HashEntry* entry = table->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    
    return NULL;
}

/**
 * Insertar una nueva clave en la tabla (para IPs únicas)
 */
void hash_table_insert(HashTable* table, const char* key) {
    if (hash_table_find(table, key)) {
        return; // Ya existe
    }
    
    unsigned int index = hash_function(key);
    
    HashEntry* new_entry = malloc(sizeof(HashEntry));
    new_entry->key = malloc(strlen(key) + 1);
    strcpy(new_entry->key, key);
    new_entry->count = 1;
    new_entry->next = table->buckets[index];
    
    table->buckets[index] = new_entry;
    table->size++;
}

/**
 * Incrementar el contador de una clave (para URLs)
 */
void hash_table_increment(HashTable* table, const char* key) {
    HashEntry* entry = hash_table_find(table, key);
    
    if (entry) {
        entry->count++;
    } else {
        // No existe, insertar con count=1
        unsigned int index = hash_function(key);
        
        HashEntry* new_entry = malloc(sizeof(HashEntry));
        new_entry->key = malloc(strlen(key) + 1);
        strcpy(new_entry->key, key);
        new_entry->count = 1;
        new_entry->next = table->buckets[index];
        
        table->buckets[index] = new_entry;
        table->size++;
    }
}

/**
 * Obtener el contador de una clave
 */
int hash_table_get_count(HashTable* table, const char* key) {
    HashEntry* entry = hash_table_find(table, key);
    return entry ? entry->count : 0;
}

/**
 * Liberar toda la memoria de una tabla hash
 */
void free_hash_table(HashTable* table) {
    if (!table) return;
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashEntry* entry = table->buckets[i];
        while (entry) {
            HashEntry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    
    free(table);
}


// FUNCIONES DE PROCESAMIENTO DE LOG

/**
 * Parsear una línea individual del log
 */
void parse_line(const char* line, ThreadResult* result) {
    char ip[20] = {0};
    char method[10] = {0};
    char url[MAX_URL_LENGTH] = {0};
    int status_code = 0;

    // Formato: IP - - [fecha] "MÉTODO URL" CÓDIGO
    int items = sscanf(line, "%19s - - [%*[^]]] \"%9s %[^\"]\" %d",
                       ip, method, url, &status_code);

    if (items == 4) {
        // 1. Insertar IP única en su tabla hash
        hash_table_insert(result->ip_table, ip);
        
        // 2. Incrementar contador para esta URL
        hash_table_increment(result->url_table, url);
        
        // 3. Contar errores HTTP (400-599) 
        if (status_code >= 400 && status_code <= 599) {
            result->error_count++;
        }
    }
}

/**
 * Función que ejecuta cada hilo - procesa un chunk del archivo
 */
void* process_log_chunk(void* arg) {
    ThreadResult* result = (ThreadResult*)arg;
    FILE* file = fopen("access.log", "r");
    
    if (!file) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    // Posicionarse en el offset de inicio
    fseek(file, result->start_offset, SEEK_SET);

    char line[MAX_LINE_LENGTH];
    
    // Si no empezamos al principio, leer y descartar la primera línea parcial
    if (result->start_offset != 0) {
        if (fgets(line, sizeof(line), file) == NULL) {
            // Si falla, seguimos adelante
        }
    }

    int lines_processed = 0;
    while (ftell(file) < result->end_offset && fgets(line, sizeof(line), file)) {
        parse_line(line, result);
        lines_processed++;
    }

    printf("HILO %d: Procesé %d líneas. IPs únicas: %d, URLs únicas: %d, Errores: %d\n", 
           result->thread_id, lines_processed, 
           result->ip_table->size, result->url_table->size, 
           result->error_count);

    fclose(file);
    return NULL;
}


// FUNCIONES DE COMBINACIÓN DE RESULTADOS

/**
 * Combinar dos tablas hash (para merge de resultados)
 */
void merge_hash_tables(HashTable* dest, HashTable* src) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashEntry* entry = src->buckets[i];
        while (entry) {
            // Para cada entrada en src, incrementar en dest
            int current_count = hash_table_get_count(dest, entry->key);
            if (current_count == 0) {
                // No existe, insertar
                hash_table_increment(dest, entry->key);
                // Ajustar el contador al valor de src
                HashEntry* dest_entry = hash_table_find(dest, entry->key);
                if (dest_entry) {
                    dest_entry->count = entry->count;
                }
            } else {
                // Ya existe, sumar contadores
                for (int j = 0; j < entry->count; j++) {
                    hash_table_increment(dest, entry->key);
                }
            }
            entry = entry->next;
        }
    }
}

/**
 * Combinar resultados de todos los hilos 
 */
void merge_results(ThreadResult* final_result, ThreadResult* thread_results, int num_threads) {
    // Inicializar el resultado final
    final_result->ip_table = create_hash_table();
    final_result->url_table = create_hash_table();
    final_result->error_count = 0;
    final_result->thread_id = -1; // ID especial para resultado combinado
    
    // Combinar resultados de cada hilo
    for (int i = 0; i < num_threads; i++) {
        ThreadResult* thr = &thread_results[i];
        
        // Sumar errores
        final_result->error_count += thr->error_count;
        
        // Combinar tablas hash de IPs
        merge_hash_tables(final_result->ip_table, thr->ip_table);
        
        // Combinar tablas hash de URLs
        merge_hash_tables(final_result->url_table, thr->url_table);
    }
}

/**
 * Encontrar la URL más visitada
 */
void find_most_visited_url(HashTable* url_table, char* most_visited_url, int* max_count) {
    *max_count = 0;
    most_visited_url[0] = '\0';
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashEntry* entry = url_table->buckets[i];
        while (entry) {
            if (entry->count > *max_count) {
                *max_count = entry->count;
                strcpy(most_visited_url, entry->key);
            }
            entry = entry->next;
        }
    }
}

/**
 * Imprimir reporte final 
 */
void print_final_report(ThreadResult* final_result) {
    char most_visited[MAX_URL_LENGTH];
    int max_count;
    
    find_most_visited_url(final_result->url_table, most_visited, &max_count);
    
    printf("\n========================================\n");
    printf("   REPORTE DEL ANALIZADOR DE LOGS\n");
    printf("========================================\n");
    printf("Total de IPs únicas: %d\n", final_result->ip_table->size);
    printf("URL más visitada: %s (%d veces)\n", most_visited, max_count);
    printf("Total de Errores HTTP (400-599): %d\n", final_result->error_count);
    printf("========================================\n");
}