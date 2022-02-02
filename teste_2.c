#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


typedef struct Barbeiro Barbeiro;
typedef struct Cliente Cliente;
typedef struct Barbearia Barbearia;




struct Cliente
{
    pthread_t id_thread_cliente;

    int id;
    int *
    //Necessario?
    sem_t *cadeira_espera;
    pthread_mutex_t *mutex;
};

struct Barbeiro
{
    pthread_t id_thread_barber;

    int id;
    int qtd_clientes_atendidos;
    int qtd_min_atender;

    sem_t *cadeira_trabalho;
    pthread_mutex_t *mutex;

};

void cortar_cabelo(void *arg)
{
    //Tentar cortar o cabelo
    //Se n conseguir cortar, tenta esperar
    //Se n consegui esperar, sai da barbearia
    Cliente cliente = (Cliente*) arg;




}

void trabalhar_barbeiro(void *arg)
{
    //Se nao tiver clientes, dorme
    //Se for acordado por algum cliente, trabalha
        //Contabiliza cliente atendido
    //Volta a dormir se n tiver cliente
    while(1)
    {
        //Dormindo se n tiver clientes
        sem_wait(&clientes);
        //Informa que um cliente saiu da fila de espera
        //MUTEXXXXXXXXXXXXXX

    }
    
}

//Cliente vai tentar entrar na barbearia
//Tentar cortar o cabelo
//Se nao conseguir, tentar esperar
//Se n conseguir esperar, sai da barbearia
int main()
{
    int qtd_cadeiras_barbeiro = 1;
    int qtd_cadeiras_cliente = 5;
    int trabalho_minimo = 10;

    //Vetor de barbeiros dada a quantidade
    Barbeiro *barbeiros;
    barbeiros = malloc(sizeof(Barbeiro) ( qtd_cadeiras_barbeiro);
    if(barbeiros == NULL))
    {
        printf("ERRO AO CRIAR BARBEIROS\n");
        return 0;
    }

    /*-CRIANDO SEMAFOROS E MUTEX-*/
    pthread_mutex_t mutex_acesso_cadeira;
    pthread_mutex_t



    return 0;
}