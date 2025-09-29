#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define VECTOR_SIZE 100000000
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    long long start_index;
    long long end_index;
    double *vector_x;
    double partial_sum;
} ThreadArgs;

double f(double x) {
    double term1_base = (x - 0.1) / 0.9;
    double term1 = pow(2.0, -2.0 * term1_base * term1_base);
    double term2 = pow(sin(5.0 * M_PI * x), 6.0);
    return term1 * term2;
}

void* thread_worker(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    double local_sum = 0.0;

    for (long long i = thread_args->start_index; i < thread_args->end_index; i++) {
        local_sum += f(thread_args->vector_x[i]);
    }

    thread_args->partial_sum = local_sum;

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <numero_de_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "O número de threads deve ser maior que zero.\n");
        return 1;
    }

    printf("Gerando vetor x de %d elementos...\n", VECTOR_SIZE);
    double* vector_x = (double*)malloc(VECTOR_SIZE * sizeof(double));
    if (vector_x == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o vetor x.\n");
        return 1;
    }

    srand(time(NULL));
    for (long long i = 0; i < VECTOR_SIZE; i++) {
        vector_x[i] = (double)rand() / (double)RAND_MAX;
    }
    printf("Vetor gerado com sucesso.\n\n");

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadArgs* thread_args_array = (ThreadArgs*)malloc(num_threads * sizeof(ThreadArgs));

    if (threads == NULL || thread_args_array == NULL) {
        fprintf(stderr, "Erro ao alocar memória para threads.\n");
        free(vector_x);
        return 1;
    }

    long long chunk_size = VECTOR_SIZE / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_args_array[i].start_index = i * chunk_size;
        thread_args_array[i].end_index = (i == num_threads - 1) ? VECTOR_SIZE : (i + 1) * chunk_size;
        thread_args_array[i].vector_x = vector_x;
        thread_args_array[i].partial_sum = 0.0;

        int rc = pthread_create(&threads[i], NULL, thread_worker, &thread_args_array[i]);
        if (rc) {
            fprintf(stderr, "ERRO: impossível criar a thread, %d\n", rc);
            exit(-1);
        }
    }

    double total_sum = 0.0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_args_array[i].partial_sum;
    }

    gettimeofday(&end_time, NULL);

    double average = total_sum / VECTOR_SIZE;
    double execution_time = (end_time.tv_sec - start_time.tv_sec) +
                             (end_time.tv_usec - start_time.tv_usec) / 1e6;

    printf("--- Resultados ---\n");
    printf("Valor médio de f(x) para todos os x: %.10f\n", average);
    printf("Tempo de execução (wall time): %f segundos\n", execution_time);
    printf("Utilizando %d threads.\n", num_threads);

    free(vector_x);
    free(threads);
    free(thread_args_array);

    return 0;
}