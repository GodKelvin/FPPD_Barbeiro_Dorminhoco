#include <semaphore.h>
#ifndef BARBEARIA_H
#define BARBEARIA_H


typedef struct Barbeiro Barbeiro;
typedef struct Cliente Cliente;

struct Cliente
{
    //ID criado pela thread
    pthread_t id_thread_cliente;

    //ID Criado por mim
    int id;

    /*Utiliza o ID do barbeiro para identificacao
    do barbeiro quando ele for acordado*/
    int *identificador_barbeiro;

    /*Para verificar se tem cadeiras disponiveis 
    na barbearia*/
    sem_t *sem_cad_espera;

    /*Para verificar qual o nome do barbeiro
    que foi acordado*/
    sem_t *sem_le_nome_identificador;
    
    /*Para informar que o barbeiro ja pode
    atender*/
    sem_t *sem_escreve_nome_identificador;

    /*Array que contem quais as cadeiras de 
    barbeiro da barbearia*/
    sem_t *sem_cad_barbeiro;

    /*Array que identifica de quem o barbeiro
    cortou o cabelo*/
    sem_t *sem_cliente_cadeira;

    /*Array que identifica a espera do barbeiro
    para realizar o corte*/
    sem_t *sem_cabelo_cortado;
};

struct Barbeiro
{
    //ID Criado pela thread
    pthread_t id_thread_barber;

    //ID Criado por mim, 
    int id;

    /*Quantidade de barbeiros na biblioteca.
    Necessario saber pois os barbeiros trabalham
    enquanto todos nao tiverem atendido o minimo*/
    int colegas_trabalho;

    /*Utiliza o ID do barbeiro para identificacao
    do barbeiro quando ele for acordado*/
    int *identificador_barbeiro;

    //Armazenar quantos clientes o barbeiro atendeu
    int qtd_clientes_atendidos;

    /*Quantidade minima de clientes para 
    atender antes de encerrar o expediente*/
    int qtd_min_atender;

    /*Vetor compartilhado para verificar
    se todo mundo ja atingiu o trabalho minimo*/
    int *trabalhos_barbeiro;

    //Para identificar que foi acordado
    sem_t *sem_escreve_nome_identificador;

    /*Para informar ao cliente que pode vir cortar
    o cabelo*/
    sem_t *sem_le_nome_identificador;

    /*Array que identifica que esta cortando o cabelo*/
    sem_t *sem_cliente_cadeira;
    /*Array que identifica que o cabelo foi cortado*/
    sem_t *sem_cabelo_cortado;
};

int barbearia_aberta(int *trabalho_barbeiros, int qtd_barbeiros);
void *barbeiro_trabalha(void *arg);
void *cortar_cabelo(void* arg);

#endif