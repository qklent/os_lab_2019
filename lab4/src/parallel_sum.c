#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "sum_lib.h"

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    return (void *)(size_t)Sum(sum_args);
}

void GenerateArray(int *array, unsigned int array_size, unsigned int seed) {
    srand(seed);
    for (unsigned int i = 0; i < array_size; i++) {
        array[i] = rand() % 100;
    }
}

int main(int argc, char **argv) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;


    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "--threads_num") == 0) {
            threads_num = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "--seed") == 0) {
            seed = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "--array_size") == 0) {
            array_size = atoi(argv[i + 1]);
        }
    }

    if (threads_num == 0 || array_size == 0) {
        printf("Usage: ./psum --threads_num <num> --seed <num> --array_size <num>\n");
        return 1;
    }


    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);

    pthread_t *threads = malloc(sizeof(pthread_t) * threads_num);
    struct SumArgs *args = malloc(sizeof(struct SumArgs) * threads_num);


    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    int elements_per_thread = array_size / threads_num;
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * elements_per_thread;
        args[i].end = (i == threads_num - 1) ? array_size : 
                                              (i + 1) * elements_per_thread;

        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }


    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        int sum = 0;
        pthread_join(threads[i], (void **)&sum);
        total_sum += sum;
    }


    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    double time_taken = (end.tv_sec - start.tv_sec) * 1000.0 + 
                       (end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("Total sum: %d\n", total_sum);
    printf("Time taken: %.2f milliseconds\n", time_taken);

    free(array);
    free(threads);
    free(args);

    return 0;
}