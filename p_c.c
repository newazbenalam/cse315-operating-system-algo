#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 10

int array[ARRAY_SIZE] = { 10, 2, 8, 6, 3, 7, 1, 9, 5, 4 };
int sorted_array[ARRAY_SIZE];

void merge(int start, int middle, int end) {
    int left_index = start;
    int right_index = middle;
    int merged_index = start;

    while (left_index < middle && right_index < end) {
        if (array[left_index] < array[right_index]) {
            sorted_array[merged_index++] = array[left_index++];
        } else {
            sorted_array[merged_index++] = array[right_index++];
        }
    }

    while (left_index < middle) {
        sorted_array[merged_index++] = array[left_index++];
    }

    while (right_index < end) {
        sorted_array[merged_index++] = array[right_index++];
    }

    for (int i = start; i < end; i++) {
        array[i] = sorted_array[i];
    }
}

void* sort(void* arg) {
    int start = *(int*) arg;
    int end = start + ARRAY_SIZE / 2;
    for (int i = start; i < end - 1; i++) {
        for (int j = i + 1; j < end; j++) {
            if (array[i] > array[j]) {
                int temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[2];
    int thread_args[2];

    // create first sorting thread
    thread_args[0] = 0;
    pthread_create(&threads[0], NULL, sort, &thread_args[0]);

    // create second sorting thread
    thread_args[1] = ARRAY_SIZE / 2;
    pthread_create(&threads[1], NULL, sort, &thread_args[1]);

    // wait for sorting threads to finish
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    // merge the two sublists
    merge(0, ARRAY_SIZE / 2, ARRAY_SIZE);

    // print the sorted array
    printf("Sorted array: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}
