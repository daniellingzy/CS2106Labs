#include "packer.h"
#include <semaphore.h>
#include <stddef.h>
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
int packet[NUM_COLOURS][BUFFER_CAPACITY];
int in_index[NUM_COLOURS];
int out_index[NUM_COLOURS];
int counter[NUM_COLOURS];
int N;

void packer_init(int balls_per_pack) {
	N = balls_per_pack;
    for (int i = 0; i < NUM_COLOURS; i++) {
        sem_init(&pack[i], 0, 0);
        sem_init(&mutex[i], 0, 1);
        in_index[i] = 0;
        out_index[i] = 0;
        counter[i] = 0;
    }
}

void packer_destroy(void) {
    for (int i = 0; i < NUM_COLOURS; i++) {
        sem_destroy(&pack[i]);
        sem_destroy(&mutex[i]);
    }
}

void pack_ball(int colour, int id, int *other_ids) {
    sem_wait(&mutex[colour-1]);
    if (counter[colour-1] == N-1) {
    	// With this ball, we have enough balls to make a full pack
    	// Put new ball in buffer
        buffer[colour-1][in_index[colour-1]].colour = colour;
        buffer[colour-1][in_index[colour-1]].id = id;
        // Shift in_index index
    	in_index[colour-1] = (in_index[colour-1] + 1) % BUFFER_CAPACITY;
    	
    	// Extract out N balls from buffer using out_index
    	for (int i = 0; i < N; i++) {
    		packet[colour-1][i] = buffer[colour-1][out_index[colour-1]].id;
    		// Shift out_index index
        	out_index[colour-1] = (out_index[colour-1] + 1) % BUFFER_CAPACITY;
    	}
    	// Reset counter
    	counter[colour-1] = 0;
    	// Signal pack N-1 times to unblock those balls buffering
    	for (int i = 0; i < N-1; i++) {
    		sem_post(&pack[colour-1]);
    	}
    	
    	// we need populate the other_ids array with the N-1 ids excluding current ball
    	int j = 0;
    	for (int i = 0; i < N; i++) {
    		// copy id inside o/p array if not current ball
    		if (packet[colour-1][i] != id) {
    			other_ids[j] = packet[colour-1][i];
    			j++;
    		}
    	}
    	sem_post(&mutex[colour-1]);
    } else {
    	// Havent accumulate enough for full pack
    	// Put new ball in buffer
        buffer[colour-1][in_index[colour-1]].colour = colour;
        buffer[colour-1][in_index[colour-1]].id = id;
        // Shift in_index index
    	in_index[colour-1] = (in_index[colour-1] + 1) % BUFFER_CAPACITY;
    	counter[colour-1]++;
    	
    	// Wait for the other balls to appear
        sem_post(&mutex[colour-1]);
        sem_wait(&pack[colour-1]);
        // we need populate the other_ids array with the N-1 ids excluding current ball
    	int j = 0;
    	for (int i = 0; i < N; i++) {
    		// copy id inside o/p array if not current ball
    		if (packet[colour-1][i] != id) {
    			other_ids[j] = packet[colour-1][i];
    			j++;
    		}
    	}
    }
}
