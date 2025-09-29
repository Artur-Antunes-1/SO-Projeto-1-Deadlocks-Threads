#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_PRODUTORES 2
#define NUM_CONSUMIDORES 3

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int count = 0;

pthread_mutex_t mutex;
pthread_cond_t can_produce;
pthread_cond_t can_consume;

void* produtor(void* arg) {
    int id = *(int*)arg;
    free(arg);
    int item;

    while (1) {
        item = rand() % 100;
        sleep(rand() % 3 + 1);

        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE) {
            printf("Produtor %d encontrou buffer CHEIO. Esperando...\n", id);
            pthread_cond_wait(&can_produce, &mutex);
        }

        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("Produtor %d produziu %d. Itens no buffer: %d\n", id, item, count);

        pthread_cond_signal(&can_consume);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumidor(void* arg) {
    int id = *(int*)arg;
    free(arg);
    int item;

    while (1) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            printf("Consumidor %d encontrou buffer VAZIO. Esperando...\n", id);
            pthread_cond_wait(&can_consume, &mutex);
        }

        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("Consumidor %d consumiu %d. Itens no buffer: %d\n", id, item, count);

        pthread_cond_signal(&can_produce);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 4 + 1);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t produtores[NUM_PRODUTORES];
    pthread_t consumidores[NUM_CONSUMIDORES];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&can_produce, NULL);
    pthread_cond_init(&can_consume, NULL);

    for (int i = 0; i < NUM_PRODUTORES; i++) {
        int* id = malloc(sizeof(int)); *id = i + 1;
        pthread_create(&produtores[i], NULL, produtor, id);
    }

    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        int* id = malloc(sizeof(int)); *id = i + 1;
        pthread_create(&consumidores[i], NULL, consumidor, id);
    }

    for (int i = 0; i < NUM_PRODUTORES; i++) pthread_join(produtores[i], NULL);
    for (int i = 0; i < NUM_CONSUMIDORES; i++) pthread_join(consumidores[i], NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&can_produce);
    pthread_cond_destroy(&can_consume);

    return 0;
}