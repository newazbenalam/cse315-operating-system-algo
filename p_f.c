/* buffer.h */

typedef int buffer_item;
#define BUFFER_SIZE 5

/* buffer.c */

#include <semaphore.h>
#include "buffer.h"

buffer_item buffer[BUFFER_SIZE];
int buffer_index;
sem_t empty;
sem_t full;
sem_t mutex;

void init_buffer() {
    buffer_index = 0;
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);
}

int insert_item(buffer_item item) {
    sem_wait(&empty); /* decrement empty count */
    sem_wait(&mutex); /* enter critical section */
    buffer[buffer_index] = item;
    buffer_index = (buffer_index + 1) % BUFFER_SIZE;
    sem_post(&mutex); /* leave critical section */
    sem_post(&full); /* increment full count */
    return 0;
}

int remove_item(buffer_item *item) {
    sem_wait(&full); /* decrement full count */
    sem_wait(&mutex); /* enter critical section */
    *item = buffer[(buffer_index - 1 + BUFFER_SIZE) % BUFFER_SIZE];
    buffer_index = (buffer_index - 1 + BUFFER_SIZE) % BUFFER_SIZE;
    sem_post(&mutex); /* leave critical section */
    sem_post(&empty); /* increment empty count */
    return 0;
}


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void *producer(void *param) {
    buffer_item item;
    while (1) {
        /* generate a random item */
        item = rand();
        if (insert_item(item) == -1) {
            fprintf(stderr, "Producer error\n");
        }
        sleep(rand() % 5);
    }
}

void *consumer(void *param) {
    buffer_item item;
    while (1) {
        if (remove_item(&item) == -1) {
            fprintf(stderr, "Consumer error\n");
        } else {
            /* consume the item */
        }
        sleep(rand() % 5);
    }
}

int main(int argc, char *argv[]) {
    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    /* initialize buffer and semaphores */
    init_buffer();

    /* create producer threads */
    for (int i = 0; i < num_producers; i++) {
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, producer, NULL);
    }

    /* create consumer threads */
    for (int i = 0; i < num_consumers; i++) {
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, consumer, NULL);
    }

    /* sleep 
