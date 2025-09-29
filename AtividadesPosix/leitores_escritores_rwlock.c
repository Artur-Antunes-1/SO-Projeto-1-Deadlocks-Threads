#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int shared_data = 0;
pthread_rwlock_t rwlock;

void* escritor(void* arg) {
    int id = *(int*)arg;
    free(arg);

    while(1) {
        sleep(rand() % 5 + 2);
        
        pthread_rwlock_wrlock(&rwlock);

        shared_data++;
        printf("Escritor %d escreveu: %d\n", id, shared_data);
        
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}

void* leitor(void* arg) {
    int id = *(int*)arg;
    free(arg);

    while(1) {
        sleep(rand() % 3 + 1);
        
        pthread_rwlock_rdlock(&rwlock);

        printf("Leitor %d leu: %d\n", id, shared_data);
        
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}

int main() {
    pthread_t leitores[5], escritores[2];
    
    pthread_rwlock_init(&rwlock, NULL);

    for(int i = 0; i < 2; i++) { int* id = malloc(sizeof(int)); *id = i + 1; pthread_create(&escritores[i], NULL, escritor, id); }
    for(int i = 0; i < 5; i++) { int* id = malloc(sizeof(int)); *id = i + 1; pthread_create(&leitores[i], NULL, leitor, id); }

    for(int i = 0; i < 2; i++) pthread_join(escritores[i], NULL);
    for(int i = 0; i < 5; i++) pthread_join(leitores[i], NULL);

    pthread_rwlock_destroy(&rwlock);

    return 0;
}