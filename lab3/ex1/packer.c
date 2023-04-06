#include "packer.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_COLOURS 3
#define BUFFER_CAPACITY 64

typedef struct {
    int colour;
    int id;
} Ball;

sem_t pack[NUM_COLOURS];
sem_t mutex[NUM_COLOURS];
Ball buffer[NUM_COLOURS][BUFFER_CAPACITY];
int in_index[NUM_COLOURS];
int out_index[NUM_COLOURS];

void packer_init(void) {
    for (int i = 0; i < NUM_COLOURS; i++) {
        sem_init(&pack[i], 0, 0);
        sem_init(&mutex[i], 0, 1);
        in_index[i] = 0;
        out_index[i] = 0;
    }
}

void packer_destroy(void) {
    for (int i = 0; i < NUM_COLOURS; i++) {
        sem_destroy(&pack[i]);
        sem_destroy(&mutex[i]);
    }
}

int pack_ball(int colour, int id) {
    sem_wait(&mutex[colour-1]);

    int other_id;
    if (in_index[colour-1] == out_index[colour-1]) {
        // buffer is empty, put new ball in
        buffer[colour-1][in_index[colour-1]].colour = colour;
        buffer[colour-1][in_index[colour-1]].id = id;
        // Shift in_index index
    	in_index[colour-1] = (in_index[colour-1] + 1) % BUFFER_CAPACITY;
        
        // Wait for the other ball to appear
        sem_post(&mutex[colour-1]);
        sem_wait(&pack[colour-1]);
        other_id = buffer[colour-1][out_index[colour-1]].id;
        // Shift out_index index
        out_index[colour-1] = (out_index[colour-1] + 1) % BUFFER_CAPACITY;
    } else {
        // buffer has a ball waiting, pair with it
        // Place current ball into buffer
        buffer[colour-1][in_index[colour-1]].colour = colour;
        buffer[colour-1][in_index[colour-1]].id = id;
        // Shift in_index index
    	in_index[colour-1] = (in_index[colour-1] + 1) % BUFFER_CAPACITY;
        // Get ID of next ball to be packed
        other_id = buffer[colour-1][out_index[colour-1]].id;
        // Shift out_index index
        out_index[colour-1] = (out_index[colour-1] + 1) % BUFFER_CAPACITY;
        sem_post(&pack[colour-1]);
        sem_post(&mutex[colour-1]);
    }
    return other_id;
}
