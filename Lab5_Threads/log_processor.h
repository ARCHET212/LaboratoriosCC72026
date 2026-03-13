#ifndef LOG_PROCESSOR_H
#define LOG_PROCESSOR_H

#include <stdio.h>
#include <pthread.h>


#define MAX_LINE_LENGTH 1024
#define MAX_URL_LENGTH 256
#define HASH_TABLE_SIZE 1000 

/**
 * Estructura para entrada de hash table (para IPs y URLs)
 */
typedef struct HashEntry {
    char* key;                 // La IP o URL
    int count;                 // Contador (para URLs) o 1 para IPs
    struct HashEntry* next;    // Para manejar colisiones
} HashEntry;

/**
 * Estructura para tabla hash
 */
typedef struct {
    HashEntry* buckets[HASH_TABLE_SIZE];
    int size;                   // Número de elementos únicos
} HashTable;

/**
 * Estructura para almacenar los resultados de un hilo individual.
 */
typedef struct {
    int thread_id;              // ID del hilo para depuración
    long start_offset;          // Posición de inicio en el archivo (bytes)
    long end_offset;            // Posición final en el archivo (bytes)
    
    // Tablas hash para este hilo
    HashTable* ip_table;        // Para IPs únicas
    HashTable* url_table;       // Para URLs y sus frecuencias
    int error_count;            // Conteo de errores HTTP (400-599)
    
} ThreadResult;

/**
 * Funciones para manejo de hash tables
 */
unsigned int hash_function(const char* key);
HashTable* create_hash_table();
void hash_table_insert(HashTable* table, const char* key);
int hash_table_get_count(HashTable* table, const char* key);
void hash_table_increment(HashTable* table, const char* key);
void free_hash_table(HashTable* table);

/**
 * Funciones principales del programa
 */
void* process_log_chunk(void* arg);
void parse_line(const char* line, ThreadResult* result);
void merge_results(ThreadResult* final_result, ThreadResult* thread_results, int num_threads);
void find_most_visited_url(HashTable* url_table, char* most_visited_url, int* max_count);
void print_final_report(ThreadResult* final_result);

#endif // LOG_PROCESSOR_H