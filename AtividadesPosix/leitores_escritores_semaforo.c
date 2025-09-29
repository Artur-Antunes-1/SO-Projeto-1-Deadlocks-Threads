#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int shared_data = 0;
int read_count = 0;
int write_count = 0;

sem_t mutex_read_count;
sem_t mutex_write_count;
sem_t resource_access;
sem_t read_try;

void* escritor(void* arg) {
    int id = *(int*)arg;
    free(arg);

    while(1) {
        sleep(rand() % 5 + 2);
        
        sem_wait(&mutex_write_count);
        write_count++;
        if (write_count == 1) {
            sem_wait(&read_try);
        }
        sem_post(&mutex_write_count);

        sem_wait(&resource_access);
        shared_data++;
        printf("Escritor %d escreveu: %d\n", id, shared_data);
        sem_post(&resource_access);

        sem_wait(&mutex_write_count);
        write_count--;
        if (write_count == 0) {
            sem_post(&read_try);
        }
        sem_post(&mutex_write_count);
    }
    return NULL;
}

void* leitor(void* arg) {
    int id = *(int*)arg;
    free(arg);

    while(1) {
        sleep(rand() % 3 + 1);

        sem_wait(&read_try);
        sem_wait(&mutex_read_count);
        read_count++;
        if (read_count == 1) {
            sem_wait(&resource_access);
        }
        sem_post(&mutex_read_count);
        sem_post(&read_try);

        printf("Leitor %d leu: %d (leitores ativos: %d)\n", id, shared_data, read_count);

        sem_wait(&mutex_read_count);
        read_count--;
        if (read_count == 0) {
            sem_post(&resource_access);
        }
        sem_post(&mutex_read_count);
    }
    return NULL;
}

int main() {
    pthread_t leitores[5], escritores[2];
    
    sem_init(&mutex_read_count, 0, 1);
    sem_init(&mutex_write_count, 0, 1);
    sem_init(&resource_access, 0, 1);
    sem_init(&read_try, 0, 1);

    for(int i = 0; i < 2; i++) { int* id = malloc(sizeof(int)); *id = i + 1; pthread_create(&escritores[i], NULL, escritor, id); }
    for(int i = 0; i < 5; i++) { int* id = malloc(sizeof(int)); *id = i + 1; pthread_create(&leitores[i], NULL, leitor, id); }

    for(int i = 0; i < 2; i++) pthread_join(escritores[i], NULL);
    for(int i = 0; i < 5; i++) pthread_join(leitores[i], NULL);

    sem_destroy(&mutex_read_count);
    sem_destroy(&mutex_write_count);
    sem_destroy(&resource_access);
    sem_destroy(&read_try);

    return 0;
}