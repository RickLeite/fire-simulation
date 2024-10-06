#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "functions.h"

pthread_mutex_t forest_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fire_detected_cond = PTHREAD_COND_INITIALIZER;

typedef struct {
    int row;
    int col;
} FireMessage;

FireMessage fire_message;
int new_fire_message = 0;

void initForest() {
    for (int i = 0; i < FOREST_SIZE; i++) {
        for (int j = 0; j < FOREST_SIZE; j++) {
            forest[i][j] = '-';
        }
    }
}

void printLegend() {
    printf("\n=== Simulação de Detecção de Incêndios ===\n\n");
    printf("Legenda:\n");
    printf("  - : Área livre\n");
    printf("  T : Sensor ativo\n");
    printf("  @ : Fogo ativo\n");
    printf("  / : Área queimada\n\n");
}

void printForest() {
    system("clear");
    
    for (int i = 0; i < FOREST_SIZE; i++) {
        for (int j = 0; j < FOREST_SIZE; j++) {
            switch(forest[i][j]) {
                case '-': printf("\033[0m- "); break; // Área livre (cor padrão)
                case 'T': printf("\033[1;34mT "); break; // Sensor ativo (azul)
                case '@': printf("\033[1;31m@ "); break; // Fogo ativo (vermelho)
                case '/': printf("\033[0;33m/ "); break; // Área queimada (amarelo)
            }
        }
        printf("\033[0m\n"); // Resetar cor
    }
    printf("\n");
}

void* sensorNode(void* arg) {
    int* params = (int*)arg;
    int row = params[0];
    int col = params[1];
    free(params);

    while (1) {
        pthread_mutex_lock(&forest_mutex);
        
        if (forest[row][col] == '@') {
            printf("Sensor em (%d, %d) detectou fogo!\n", row, col);
            notifyNeighbors(row, col);
            
            if (row == 0 || row == FOREST_SIZE - 1 || col == 0 || col == FOREST_SIZE - 1) {
                printf("Fogo detectado na borda em (%d, %d). Notificando central.\n", row, col);
                fire_message.row = row;
                fire_message.col = col;
                new_fire_message = 1;
                pthread_cond_signal(&fire_detected_cond);
            }
        }
        
        pthread_mutex_unlock(&forest_mutex);
        
        sleep(1);
    }
    
    return NULL;
}


void notifyNeighbors(int row, int col) {
    int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
    
    for (int i = 0; i < 8; i++) {
        int newRow = row + dx[i];
        int newCol = col + dy[i];
        
        if (newRow >= 0 && newRow < FOREST_SIZE && newCol >= 0 && newCol < FOREST_SIZE) {
            if (forest[newRow][newCol] == '-') {
                forest[newRow][newCol] = 'T';
                printf("Sensor em (%d, %d) ativado.\n", newRow, newCol);
            }
        }
    }
}

void* fireGenerator(void* arg) {
    (void)arg;
    while (1) {
        sleep(FIRE_INTERVAL);
        
        pthread_mutex_lock(&forest_mutex);
        
        int row = rand() % FOREST_SIZE;
        int col = rand() % FOREST_SIZE;
        
        if (forest[row][col] == '-') {
            forest[row][col] = '@';
            printf("\nINCÊNDIO GERADO em (%d, %d)\n", row, col);
        }
        
        pthread_mutex_unlock(&forest_mutex);
        
        printForest();
    }
    
    return NULL;
}

void* centralControl(void* arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&forest_mutex);
        
        while (!new_fire_message && !fireDetectedOnBorder()) {
            pthread_cond_wait(&fire_detected_cond, &forest_mutex);
        }
        
        if (new_fire_message) {
            printf("\nCENTRAL: Incêndio detectado na borda em (%d, %d). Iniciando combate...\n", 
                   fire_message.row, fire_message.col);
            new_fire_message = 0;
        } else if (fireDetectedOnBorder()) {
            printf("\nCENTRAL: Incêndio detectado na borda durante verificação de rotina. Iniciando combate...\n");
        }
        
        combatFire();
        
        pthread_mutex_unlock(&forest_mutex);
        
        printForest();
    }
    
    return NULL;
}

int fireDetectedOnBorder() {
    for (int i = 0; i < FOREST_SIZE; i++) {
        if (forest[0][i] == '@' || forest[FOREST_SIZE-1][i] == '@' ||
            forest[i][0] == '@' || forest[i][FOREST_SIZE-1] == '@') {
            return 1;
        }
    }
    return 0;
}

void combatFire() {
    for (int i = 0; i < FOREST_SIZE; i++) {
        for (int j = 0; j < FOREST_SIZE; j++) {
            if (forest[i][j] == '@') {
                forest[i][j] = '/';
                printf("Fogo combatido em (%d, %d)\n", i, j);
            }
        }
    }
}