#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_PRODUTORES 2
#define NUM_CONSUMIDORES 3

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t empty;
sem_t full;
sem_t mutex;

void* produtor(void* arg) {
    int id = *(int*)arg;
    free(arg);
    int item;

    while (1) {
        item = rand() % 100;
        sleep(rand() % 3 + 1);

        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        printf("Produtor %d produziu %d. Buffer[in=%d, out=%d]\n", id, item, in, out);

        sem_post(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void* consumidor(void* arg) {
    int id = *(int*)arg;
    free(arg);
    int item;

    while (1) {
        sem_wait(&full);
        sem_wait(&mutex);

        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        printf("Consumidor %d consumiu %d. Buffer[in=%d, out=%d]\n", id, item, in, out);

        sem_post(&mutex);
        sem_post(&empty);

        sleep(rand() % 4 + 1);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t produtores[NUM_PRODUTORES];
    pthread_t consumidores[NUM_CONSUMIDORES];

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    for (int i = 0; i < NUM_PRODUTORES; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&produtores[i], NULL, produtor, id);
    }

    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&consumidores[i], NULL, consumidor, id);
    }

    for (int i = 0; i < NUM_PRODUTORES; i++) {
        pthread_join(produtores[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(consumidores[i], NULL);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    return 0;
}