#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> 
#include <time.h>

#define NUM_VIRTUAL_PAGES 256
#define NUM_FRAMES 100

// Colores ANSI para la terminal
#define RESET "\033[0m"
#define RED   "\033[1;31m"
#define GREEN "\033[1;32m"
#define BLUE  "\033[1;34m"
#define MAGENTA "\033[1;35m"

// Dueños de marcos en ram_fisica
#define FREE 0
#define SYSTEM 1
#define PROC1 2
#define PROC2 3

int ram_fisica[NUM_FRAMES]; //inicia vacio

typedef struct {
    int valid; // 1 si está en RAM, 0 si no
    int pfn;   // El índice del frame en ram_fisica
} PageTableEntry;

// Dos tablas de páginas
PageTableEntry pt1[NUM_VIRTUAL_PAGES];
PageTableEntry pt2[NUM_VIRTUAL_PAGES];


void translate_address(uint32_t virtual_address, int v_pages, PageTableEntry table[], int proc_id){
    if(virtual_address > 0xFFFF){
        printf("P%d: VA=%-10u ERROR=VA_OUT_OF_RANGE\n", proc_id, virtual_address);
        return;
    }

    uint16_t offset = virtual_address & 0xFF;
    uint16_t vpn = (virtual_address >> 8) & 0xFF;

    if(vpn >= v_pages){
        printf("P%d: VA=%-10u ERROR=VPN_OUT_OF_RANGE (vpn=%d, V=%d)\n", proc_id, virtual_address, vpn, v_pages);
        return;
    }

    if (table[vpn].valid) {
        int pfn = table[vpn].pfn;
        uint32_t physical_address = (pfn * 256) + offset;
        printf("P%d: VA=0x%04X (%-5u) VPN=0x%02X OFF=0x%02X PFN=%d PA=0x%04X\n", 
                proc_id, virtual_address, virtual_address, vpn, offset, pfn, physical_address);
    } else {
        printf("P%d: VA=%-10u ERROR=PAGE_NOT_MAPPED\n", proc_id, virtual_address);
    }
}

void init_ram(int total_v_needed, int seed){
    if(seed != 0){
        srand(seed);
    }else{
        srand(time(NULL));
    }

    int free_count;

    do {
        for(int i = 0; i < NUM_FRAMES; i++){
            ram_fisica[i] = FREE;
        }

        int to_occupy = (rand() % 51) + 10;
        for(int i = 0; i < to_occupy; i++){
            ram_fisica[rand() % NUM_FRAMES] = SYSTEM;//marcar como ocupado
        }
        
        //cuantos espacios libreas quedaron
        free_count = 0;
        for (int i = 0; i < NUM_FRAMES; i++){
            if(ram_fisica[i] == FREE){
                free_count++;
            }
        }
    } while(free_count < total_v_needed || free_count < 15);
}


void print_ram_map() {
    int f = 0, s = 0, p1 = 0, p2 = 0;
    printf("\n--- VISUALIZACIÓN DE RAM (10x10) ---\n");
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (ram_fisica[i] == FREE) { 
            printf(GREEN " %2d:F " RESET, i); 
            f++; 
        }
        else if (ram_fisica[i] == SYSTEM) { 
            printf(RED " %2d:X " RESET, i); 
            s++; 
        }
        else if (ram_fisica[i] == PROC1) { 
            printf(BLUE " %2d:1 " RESET, i); 
            p1++; 
        }
        else if (ram_fisica[i] == PROC2) { 
            printf(MAGENTA " %2d:2 " RESET, i); 
            p2++; 
        }
        
        if ((i + 1) % 10 == 0) printf("\n");
    }
    printf("\nESTADO: " GREEN "LIBRE:%d " RED "SISTEMA:%d " BLUE "P1:%d " MAGENTA "P2:%d\n" RESET, f, s, p1, p2);
}

int allocate_frame(int owner_id) {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (ram_fisica[i] == FREE) {
            ram_fisica[i] = owner_id;
            return i;
        }
    }
    return -1;
}

int load_process(int v_pages, PageTableEntry table[], int owner_id) {
    // 1. Sanity Check: Hay suficiente espacio total antes de empezar?
    int currently_free = 0;
    for(int i = 0; i < NUM_FRAMES; i++) {
        if(ram_fisica[i] == FREE) currently_free++;
    }

    if (v_pages > currently_free) {
        printf(RED "Error: Sanity check failed for P%d. V(%d) > FREE(%d)\n" RESET, owner_id, v_pages, currently_free);
        return 0;
    }

    // 2. Intento de carga página por página
    for (int i = 0; i < v_pages; i++) {
        int frame = allocate_frame(owner_id);
        
        // Si allocate_frame falla
        if (frame == -1) {
            printf(RED "Error: Abortando carga P%d. Fallo inesperado en página %d.\n" RESET, owner_id, i);
            
            // --- ROLLBACK COMPLETO ---
            // Recorremos solo lo que habíamos asignado hasta el momento (j < i)
            for (int j = 0; j < i; j++) {
                int frame_to_free = table[j].pfn; // Buscamos qué marco le dimos
                ram_fisica[frame_to_free] = FREE; // Lo devolvemos a la RAM
                table[j].valid = 0;               // Invalidamos la entrada en la tabla
                table[j].pfn = -1;                // Limpiamos el PFN (opcional, pero limpio)
            }
            return 0; // Indicamos fallo
        }

        // Si todo va bien, registramos en la tabla
        table[i].valid = 1;
        table[i].pfn = frame;
    }

    // 3. Print de éxito 
    printf("Load process P%d: V=%d -> VPN 0..%d mapped to PFNs [", owner_id, v_pages, v_pages-1);
    for (int i = 0; i < v_pages; i++) {
        printf("%d%s", table[i].pfn, (i == v_pages - 1) ? "" : ", ");
    }
    printf("]\n");

    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <V_PAGES> <FILENAME> [SEED]\n", argv[0]);
        return 1;
    }

    int v_pages = atoi(argv[1]);
    char *filename = argv[2];
    int seed = (argc > 3) ? atoi(argv[3]) : 0;

    // Inicializar tablas
    for(int i = 0; i < NUM_VIRTUAL_PAGES; i++) pt1[i].valid = pt2[i].valid = 0;

    // Espacio para P1 y P2 
    init_ram(v_pages * 2, seed);
    
    printf("--- MAPA INICIAL ---");
    print_ram_map();

  
    if (load_process(v_pages, pt1, PROC1)) printf("\nProceso 1 cargado.");
    if (load_process(v_pages, pt2, PROC2)) printf("\nProceso 2 cargado.");

    printf("\n--- MAPA DESPUÉS DE CARGAR PROCESOS ---");
    print_ram_map();

    FILE *file = fopen(filename, "r");
    if (!file) return 1;

    printf("\n--- TRADUCCIONES BATCH ---\n");
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char *endptr;
        uint32_t addr = (uint32_t)strtol(line, &endptr, 0);
        if (line != endptr) {
            translate_address(addr, v_pages, pt1, 1);
            translate_address(addr, v_pages, pt2, 2);
            printf("-----------------------------------\n");
        }
    }

    fclose(file);
    return 0;
}