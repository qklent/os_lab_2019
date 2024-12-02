#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <getopt.h>

typedef struct {
    long long start;
    long long end;
    long long mod;
    long long* result;
} ThreadData;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* calculate_factorial_part(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long long local_result = 1;

    for (long long i = data->start; i <= data->end; i++) {
        local_result = (local_result * i) % data->mod;
    }

    pthread_mutex_lock(&mutex);
    *data->result = (*data->result * local_result) % data->mod;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

long long parallel_factorial(long long k, int pnum, long long mod) {
    pthread_t* threads = malloc(pnum * sizeof(pthread_t));
    ThreadData* thread_data = malloc(pnum * sizeof(ThreadData));
    long long result = 1;
    long long chunk_size = k / pnum;

    for (int i = 0; i < pnum; i++) {
        thread_data[i].start = i * chunk_size + 1;
        thread_data[i].end = (i == pnum - 1) ? k : (i + 1) * chunk_size;
        thread_data[i].mod = mod;
        thread_data[i].result = &result;

        if (pthread_create(&threads[i], NULL, calculate_factorial_part, &thread_data[i]) != 0) {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }

    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread\n");
            exit(1);
        }
    }

    free(threads);
    free(thread_data);

    return result;
}

int main(int argc, char* argv[]) {
    long long k = 0;
    int pnum = 0;
    long long mod = 0;

    struct option long_options[] = {
        {"pnum", required_argument, 0, 'p'},
        {"mod", required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "k:p:m:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'k':
                k = atoll(optarg);
                break;
            case 'p':
                pnum = atoi(optarg);
                break;
            case 'm':
                mod = atoll(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<modulus>\n", argv[0]);
                exit(1);
        }
    }

    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Invalid arguments\n");
        exit(1);
    }

    if (pnum > k) {
        pnum = k; 
    }

    long long result = parallel_factorial(k, pnum, mod);
    printf("%lld\n", result);

    return 0;
}