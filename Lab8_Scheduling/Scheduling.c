#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_TIME 5000 

typedef struct {
    int id;
    int burst_time;
    int arrival_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    int has_started;
} Process;

// --- ESTRUCTURA PARA PUNTOS EXTRA ---
int gantt_log[MAX_TIME];
int total_execution_duration = 0;

void reset_gantt() {
    for (int i = 0; i < MAX_TIME; i++) gantt_log[i] = -1;
    total_execution_duration = 0;
}

// Función para dibujar el Diagrama de Gantt en ASCII 
void draw_gantt_chart(int end_time) {
    printf("\n--- Gantt Chart (Visual Timeline) ---\n ");
    
    // Parte superior
    for (int i = 0; i < end_time; i++) printf("---");
    printf("\n|");
    
    // Contenido del diagrama
    for (int i = 0; i < end_time; i++) {
        if (gantt_log[i] == -1) printf("  |"); // Tiempo ocioso 
        else printf("P%d|", gantt_log[i]);
    }
    
    printf("\n ");
    // Parte inferior
    for (int i = 0; i < end_time; i++) printf("---");
    printf("\n0");
    
    // Eje de tiempo
    for (int i = 1; i <= end_time; i++) printf("%3d", i);
    printf("\n");
}

// --- FUNCIONES EXISTENTES MODIFICADAS PARA EL LOG ---

void get_timestamp(char *buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 50, "[%a %b %d %H:%M:%S %Y]", t);
}

void print_dataset(Process p[], int n) {
    printf("Dataset: %d threads, Burst Times: [", n);
    for (int i = 0; i < n; i++) printf("%d%s", p[i].burst_time, (i == n - 1) ? "" : ", ");
    printf("], Arrival Times: [");
    for (int i = 0; i < n; i++) printf("%d%s", p[i].arrival_time, (i == n - 1) ? "" : ", ");
    printf("]\n");
}

void print_stats(Process p[], int n) {
    float sum_w = 0, sum_t = 0;
    printf("Waiting Times: [");
    for (int i = 0; i < n; i++) {
        printf("%.2f%s", (float)p[i].waiting_time, (i == n - 1) ? "" : ", ");
        sum_w += p[i].waiting_time;
    }
    printf("]\nAvg Waiting Time: %.2f seconds\n", sum_w / n);
    printf("Turnaround Times: [");
    for (int i = 0; i < n; i++) {
        printf("%.2f%s", (float)p[i].turnaround_time, (i == n - 1) ? "" : ", ");
        sum_t += p[i].turnaround_time;
    }
    printf("]\nAvg Turnaround Time: %.2f seconds\n", sum_t / n); 
}

void simulate_fifo(Process p[], int n) {
    reset_gantt();
    int current_time = 0;
    char ts[50];
    printf("\n--- FIFO Scheduling ---\n");
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (p[j].arrival_time > p[j+1].arrival_time) {
                Process temp = p[j]; p[j] = p[j+1]; p[j+1] = temp;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        get_timestamp(ts);
        if (current_time < p[i].arrival_time) current_time = p[i].arrival_time;
        p[i].waiting_time = current_time - p[i].arrival_time;
        
        printf("%s Process %d: Started\n", ts, p[i].id); 
        for (int t = 0; t < p[i].burst_time; t++) gantt_log[current_time + t] = p[i].id;
        
        current_time += p[i].burst_time;
        p[i].turnaround_time = current_time - p[i].arrival_time;
        get_timestamp(ts);
        printf("%s Process %d: Completed\n", ts, p[i].id); 
    }
    print_stats(p, n);
    draw_gantt_chart(current_time);
}

void simulate_rr(Process p[], int n, int quantum) {
    reset_gantt();
    int current_time = 0, completed = 0;
    char ts[50];
    printf("\n--- Round Robin Scheduling (Quantum %d) ---\n", quantum); 
    while (completed < n) {
        int idle = 1;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival_time <= current_time && p[i].remaining_time > 0) {
                idle = 0;
                get_timestamp(ts);
                if (!p[i].has_started) {
                    p[i].has_started = 1;
                }
                int run = (p[i].remaining_time > quantum) ? quantum : p[i].remaining_time;
                for (int t = 0; t < run; t++) gantt_log[current_time + t] = p[i].id;
                
                current_time += run;
                p[i].remaining_time -= run;
                if (p[i].remaining_time == 0) {
                    completed++;
                    p[i].turnaround_time = current_time - p[i].arrival_time;
                    p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
                }
            }
        }
        if (idle) current_time++;
    }
    print_stats(p, n);
    draw_gantt_chart(current_time);
}

void simulate_sjf(Process p[], int n) {
    reset_gantt();
    int current_time = 0, completed = 0;
    char ts[50];
    int is_completed[n];
    for(int i = 0; i < n; i++) is_completed[i] = 0;
    printf("\n--- SJF Scheduling ---\n");
    while (completed < n) {
        int idx = -1, min_burst = 999;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival_time <= current_time && !is_completed[i]) {
                if (p[i].burst_time < min_burst) { min_burst = p[i].burst_time; idx = i; }
            }
        }
        if (idx != -1) {
            p[idx].waiting_time = current_time - p[idx].arrival_time;
            for (int t = 0; t < p[idx].burst_time; t++) gantt_log[current_time + t] = p[idx].id;
            current_time += p[idx].burst_time;
            p[idx].turnaround_time = current_time - p[idx].arrival_time;
            is_completed[idx] = 1;
            completed++;
        } else current_time++;
    }
    print_stats(p, n);
    draw_gantt_chart(current_time);
}

void simulate_srtf(Process p[], int n) {
    reset_gantt();
    int current_time = 0, completed = 0;
    printf("\n--- SRTF Scheduling ---\n");
    while (completed < n) {
        int idx = -1, min_r = 999;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival_time <= current_time && p[i].remaining_time > 0) {
                if (p[i].remaining_time < min_r) { min_r = p[i].remaining_time; idx = i; }
            }
        }
        if (idx != -1) {
            gantt_log[current_time] = p[idx].id;
            p[idx].remaining_time--;
            current_time++;
            if (p[idx].remaining_time == 0) {
                completed++;
                p[idx].turnaround_time = current_time - p[idx].arrival_time;
                p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
            }
        } else current_time++;
    }
    print_stats(p, n);
    draw_gantt_chart(current_time);
}

int main() {
    srand(time(NULL));
    int n = rand() % 11 + 5; 
    Process original[n], work[n];
    for (int i = 0; i < n; i++) {
        original[i].id = i;
        original[i].burst_time = rand() % 10 + 1;
        original[i].arrival_time = rand() % 30; 
        original[i].remaining_time = original[i].burst_time;
        original[i].has_started = 0;
    }
    print_dataset(original, n); 
    
    memcpy(work, original, sizeof(original)); simulate_fifo(work, n);
    for(int i=0; i<n; i++) { work[i]=original[i]; work[i].has_started=0; }
    simulate_rr(work, n, 2);
    for(int i=0; i<n; i++) { work[i]=original[i]; work[i].has_started=0; }
    simulate_sjf(work, n);
    for(int i=0; i<n; i++) { work[i]=original[i]; work[i].has_started=0; }
    simulate_srtf(work, n);
    return 0;
}