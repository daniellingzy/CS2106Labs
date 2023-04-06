#include "packer.h"
#include <semaphore.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_PAIRS 2
#define BUFFER_CAPACITY 64

typedef struct {
    int colour;
    int id;
} Ball;

sem_t pack[NUM_PAIRS];
sem_t mutex[NUM_PAIRS];
Ball buffer[NUM_PAIRS][BUFFER_CAPACITY];
int packet[NUM_PAIRS][NUM_PAIRS];
int in_index[NUM_PAIRS];
int out_index[NUM_PAIRS];
int counter[NUM_PAIRS];

void packer_init(void) {
    for (int i = 0; i < NUM_PAIRS; i++) {
        sem_init(&pack[i], 0, 0);
        sem_init(&mutex[i], 0, 1);
        in_index[i] = 0;
        out_index[i] = 0;
        counter[i] = 0;
    }
}

void packer_destroy(void) {
    for (int i = 0; i < NUM_PAIRS; i++) {
        sem_destroy(&pack[i]);
        sem_destroy(&mutex[i]);
    }
}

int pack_ball(int colour, int id) {
    int pair_index = (colour == 1 || colour == 2) ? 0 : 1;
    int other_colour = (colour == 1) ? 2 : ((colour == 2) ? 1 : ((colour == 3) ? 4 : 3));

    sem_wait(&mutex[pair_index]);
    if (counter[pair_index] == 1) {
        // We have enough balls to make a full pack
        // Put new ball in buffer
        buffer[pair_index][in_index[pair_index]].colour = colour;
        buffer[pair_index][in_index[pair_index]].id = id;
        // Shift in_index index
        in_index[pair_index] = (in_index[pair_index] + 1) % BUFFER_CAPACITY;

        // Extract out 2 balls from buffer using out_index
        for (int i = 0; i < NUM_PAIRS; i++) {
            packet[pair_index][i] = buffer[pair_index][out_index[pair_index]].id;
            // Shift out_index index
            out_index[pair_index] = (out_index[pair_index] + 1) % BUFFER_CAPACITY;
        }
        // Reset counter
        counter[pair_index] = 0;
        // Signal pack once to unblock the other ball in the pair
        sem_post(&pack[pair_index]);

        // Return the id of the other ball in the pair
        return packet[pair_index][0] == id ? packet[pair_index][1] : packet[pair_index][0];
    } else {
        // Haven't accumulated enough for full pack
        // Put new ball in buffer
        buffer[pair_index][in_index[pair_index]].colour = colour;
        buffer[pair_index][in_index[pair_index]].id = id;
        // Shift in_index index
        in_index[pair_index] = (in_index[pair_index] + 1) % BUFFER_CAPACITY;
        counter[pair_index]++;

        // Wait for the other ball in the pair to appear
        sem_post(&mutex[pair_index]);
        sem_wait(&pack[pair_index]);
        // Return the id of the other ball in the pair
        return packet[pair_index][0] == id ? packet[pair_index][1] : packet[pair_index][0];
    }
}
