#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "functions.h"

#define NUM_THREADS (FOREST_SIZE * FOREST_SIZE)

char forest[FOREST_SIZE][FOREST_SIZE];
pthread_t threads[NUM_THREADS];
pthread_t fire_generator_thread;
pthread_t central_thread;

int main() {
    srand(time(NULL));
    initForest();
    
    pthread_mutex_init(&forest_mutex, NULL);
    pthread_cond_init(&fire_detected_cond, NULL);
    
    printLegend();
    printForest();
    
    // Criar threads para cada nó sensor
    for (int i = 0; i < FOREST_SIZE; i++) {
        for (int j = 0; j < FOREST_SIZE; j++) {
            int *params = malloc(2 * sizeof(int));
            params[0] = i;
            params[1] = j;
            pthread_create(&threads[i * FOREST_SIZE + j], NULL, sensorNode, params);
        }
    }
    
    pthread_create(&fire_generator_thread, NULL, fireGenerator, NULL);
    pthread_create(&central_thread, NULL, centralControl, NULL);
    
    // Aguarda threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(fire_generator_thread, NULL);
    pthread_join(central_thread, NULL);
    
    pthread_mutex_destroy(&forest_mutex);
    pthread_cond_destroy(&fire_detected_cond);
    
    return 0;
}