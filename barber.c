#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define n_clientes 15
#define n_barbeiros 2
#define n_cadeiras 5

sem_t sem_cadeiras;
sem_t sem_cad_barbeiro[n_barbeiros];
sem_t sem_cabelo_cortado[n_barbeiros];
sem_t sem_cliente_cadeira[n_barbeiros];

sem_t sem_escreve_visor, sem_le_visor;

//Identificacao do barbeiro que esta cortando o cabelo
int visor;

//Thread do barbeiro trabalhando (ou dormindo...)
void *barbeiro_trabalha(void *arg)
{
    //Converter para o ID do barbeiro?
    int id = *(int*)arg;
    while(1)
    {
        //Espera algum cliente o acordar
        sem_wait(&sem_escreve_visor);
        visor = id;

        //Informo ao cliente para vir cortar o cabelo
        sem_post(&sem_le_visor);

        //Informo que o barbeiro X esta cortando cabelo
        sem_wait(&sem_cliente_cadeira[id]);

        //Informo que o barbeiro terminou de cortar o cabelo
        sem_post(&sem_cabelo_cortado[id]);
    }

    return NULL;
}

//Thread do cliente indo cortar o cabelo
void *cortar_cabelo(void* cliente)
{
    int id = *(int*) cliente;
    int qual_cadeira;


    /*Verifico se tem vaga disponivel na barbearia,
    ou seja, se positivo, tem pelo menos uma cadeira disponivel
    para espera. O caso de sucesso eh quando tem barbeiro
    disponivel tambem*/
    if(sem_trywait(&sem_cadeiras) == 0)
    {
        printf("Cliente conseguiu vaga: %d\n", id);

        //Verifica qual barbeiro estah dormindo
        sem_wait(&sem_le_visor);

        //Vai ate a cadeira do barbeiro que estava dormindo
        qual_cadeira = visor;
        printf("Cliente: %d, Barbeiro: %d\n", id, qual_cadeira);

        //Acorda o barbeiro para ser atendido
        sem_post(&sem_escreve_visor);

        //Senta na cadeira e aguarda o corte
        sem_wait(&sem_cliente_cadeira[qual_cadeira]);

        //Depois do corte, saio da cadeiro do respectivo barbeiro
        sem_post(&sem_cad_barbeiro[qual_cadeira]);

        printf("Cliente %d saiu da barbearia\n", id);

        return NULL;

    }
    /*Nao tem cadeira disponivel e por consequencia,
    tambem nao tem barbeiro disponivel,
    e o cliente sai da barbearia*/
    else
    {
        printf("SAINDO\n");
        return NULL;
    }
}

int main()
{
    pthread_t clientes[n_clientes], barbeiros[n_barbeiros];
    int id_clientes[n_clientes], id_barbeiros[n_barbeiros];

    
    /*-Iniciando os semaforos-*/

    //Quantidade de cadeiras disponiveis para espera
    sem_init(&sem_cadeiras, 0, 5);
    
    //Informar qual barbeiro vai atender
    sem_init(&sem_escreve_visor, 0, 1);

    //Identificar qual barbeiro esta cortando o cabelo
    sem_init(&sem_le_visor, 0, 0);

    for(int i = 0; i < n_barbeiros; i++)
    {
        sem_init(&sem_cad_barbeiro[i], 0, 1);
        sem_init(&sem_cliente_cadeira[i], 0, 0);
        sem_init(&sem_cabelo_cortado[i], 0, 0);
    }

    for(int i = 0; i < n_clientes; i++)
    {
        id_clientes[i] = i;
        pthread_create(&clientes[i], NULL, cortar_cabelo, (void*)&id_clientes[i]);
    }

    for(int i = 0; i < n_barbeiros; i++)
    {
        id_barbeiros[i] = i;
        pthread_create(&barbeiros[i], NULL, barbeiro_trabalha, (void*)&id_barbeiros[i]);
    }

    for(int i = 0; i< n_clientes; i++)
    {
        pthread_join(clientes[i], NULL);
    }

    printf("BORA PRA CASA SEUS CORNOS\n");
    return 0;
}