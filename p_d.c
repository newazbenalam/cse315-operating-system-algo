#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_READERS 10

// Shared resource
int data = 0;

// Semaphores
sem_t mutex, count;

// Reader thread function
void* reader(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&mutex);
        sem_post(&count);
        printf("Reader %d is reading data: %d\n", id, data);
        sem_wait(&count);
        sem_post(&mutex);
        // Reading is finished, do something else for a while
        usleep(rand() % 1000000);
    }
    pthread_exit(NULL);
}

// Writer thread function
void* writer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&mutex);
        printf("Writer %d is writing data\n", id);
        data++; // Write to the shared resource
        sem_post(&mutex);
        // Writing is finished, do something else for a while
        usleep(rand() % 1000000);
    }
    pthread_exit(NULL);
}

int main() {
    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&count, 0, MAX_READERS);

    // Create reader threads
    pthread_t reader_threads[MAX_READERS];
    int reader_ids[MAX_READERS];
    for (int i = 0; i < MAX_READERS; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&reader_threads[i], NULL, reader, &reader_ids[i]);
    }

    // Create writer threads
    pthread_t writer_threads[2];
    int writer_ids[2] = {1, 2};
    for (int i = 0; i < 2; i++) {
        pthread_create(&writer_threads[i], NULL, writer, &writer_ids[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < MAX_READERS; i++) {
        pthread_join(reader_threads[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(writer_threads[i], NULL);
    }

    // Clean up semaphores
    sem_destroy(&mutex);
    sem_destroy(&count);

    return 0;
}
