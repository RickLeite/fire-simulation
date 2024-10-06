#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

#define FOREST_SIZE 30
#define FIRE_INTERVAL 3

extern char forest[FOREST_SIZE][FOREST_SIZE];
extern pthread_mutex_t forest_mutex;
extern pthread_cond_t fire_detected_cond;

void initForest();
void printLegend();
void printForest();
void* sensorNode(void* arg);
void notifyNeighbors(int row, int col);
void* fireGenerator(void* arg);
void* centralControl(void* arg);
int fireDetectedOnBorder();
void combatFire();

#endif // FUNCTIONS_H