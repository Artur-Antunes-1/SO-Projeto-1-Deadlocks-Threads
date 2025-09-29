#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex_barbearia;
pthread_cond_t cond_barbeiro_livre;

int barbeiros_dormindo = 0;
int clientes_esperando = 0;
int MAX_CADEIRAS_ESPERA;
int TEMPO_CORTE;

void* thread_barbeiro(void* arg) {
    int id = *(int*)arg;
    free(arg);

    while (1) {
        pthread_mutex_lock(&mutex_barbearia);

        while (clientes_esperando == 0) {
            printf("Barbeiro %d está dormindo (sem clientes).\n", id);
            barbeiros_dormindo++;
            pthread_cond_wait(&cond_barbeiro_livre, &mutex_barbearia);
            barbeiros_dormindo--;
            printf("Barbeiro %d foi acordado por um cliente.\n", id);
        }

        clientes_esperando--;
        printf("Barbeiro %d chamou um cliente. Clientes na espera: %d\n", id, clientes_esperando);

        pthread_mutex_unlock(&mutex_barbearia);

        printf("Barbeiro %d está cortando o cabelo...\n", id);
        sleep(TEMPO_CORTE);
        printf("Barbeiro %d terminou o corte.\n", id);
    }
    return NULL;
}

void* fabrica_de_clientes(void* arg) {
    int intervalo_chegada = *(int*)arg;
    int cliente_id = 1;

    while (1) {
        sleep(intervalo_chegada);

        printf(">> Cliente %d chegou na barbearia.\n", cliente_id++);

        pthread_mutex_lock(&mutex_barbearia);

        if (clientes_esperando < MAX_CADEIRAS_ESPERA) {
            clientes_esperando++;
            printf("   Cliente sentou para esperar. Total na espera: %d\n", clientes_esperando);

            if (barbeiros_dormindo > 0) {
                pthread_cond_signal(&cond_barbeiro_livre);
            }
        } else {
            printf("   Barbearia cheia, cliente foi embora.\n");
        }
        
        pthread_mutex_unlock(&mutex_barbearia);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <barbeiros> <cadeiras> <tempo_corte> <intervalo_clientes>\n", argv[0]);
        return 1;
    }

    int num_barbeiros = atoi(argv[1]);
    MAX_CADEIRAS_ESPERA = atoi(argv[2]);
    TEMPO_CORTE = atoi(argv[3]);
    int intervalo_clientes = atoi(argv[4]);

    if (num_barbeiros <= 0 || MAX_CADEIRAS_ESPERA < 0 || TEMPO_CORTE <= 0 || intervalo_clientes <= 0) {
        fprintf(stderr, "Os argumentos devem ser números inteiros positivos.\n");
        return 1;
    }

    pthread_mutex_init(&mutex_barbearia, NULL);
    pthread_cond_init(&cond_barbeiro_livre, NULL);

    pthread_t barbeiros[num_barbeiros];
    pthread_t fabrica_clientes_thread;

    printf("Barbearia aberta com %d barbeiro(s) e %d cadeira(s) de espera.\n\n", num_barbeiros, MAX_CADEIRAS_ESPERA);

    for (int i = 0; i < num_barbeiros; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&barbeiros[i], NULL, thread_barbeiro, id) != 0) {
            perror("Erro ao criar thread de barbeiro.");
            return 1;
        }
    }

    if (pthread_create(&fabrica_clientes_thread, NULL, fabrica_de_clientes, &intervalo_clientes) != 0) {
        perror("Erro ao criar thread de clientes.");
        return 1;
    }

    pthread_join(fabrica_clientes_thread, NULL);

    pthread_mutex_destroy(&mutex_barbearia);
    pthread_cond_destroy(&cond_barbeiro_livre);

    return 0;
}