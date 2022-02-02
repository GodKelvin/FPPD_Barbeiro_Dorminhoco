#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

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
    sem_t *sem_cadeiras;

    /*Para verificar qual o nome do barbeiro
    que foi acordado*/
    sem_t *sem_le_nome_identificador
    
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
    sem_t *sem_cabelo_cortado
}

struct Barbeiro
{
    //ID Criado pela thread
    pthread_t id_thread_barber;


    //ID Criado por mim, 
    int id;

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

#define n_clientes 15
#define n_barbeiros 2
#define n_cadeiras 5

sem_t sem_cadeiras;
sem_t sem_cad_barbeiro[n_barbeiros];
sem_t sem_cabelo_cortado[n_barbeiros];
sem_t sem_cliente_cadeira[n_barbeiros];

/*Semaforos utilizados para saber qual barbeiro foi acordado
e por consequencia, atendendo*/
sem_t sem_escreve_nome_identificador, sem_le_nome_identificador;

//Identificacao do barbeiro que esta cortando o cabelo
int identificador_barbeiro;

//Thread do barbeiro trabalhando (ou dormindo...)
void *barbeiro_trabalha(void *arg)
{
    //Converter para o ID do barbeiro?
    int id = *(int*)arg;
    while(1)
    {
        //Espera algum cliente o acordar
        sem_wait(&sem_escreve_nome_identificador);
        identificador_barbeiro = id;

        //Informo ao cliente para vir cortar o cabelo
        sem_post(&sem_le_nome_identificador);

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
        sem_wait(&sem_le_nome_identificador);

        //Vai ate a cadeira do barbeiro que estava dormindo
        qual_cadeira = identificador_barbeiro;
        //printf("Cliente: %d, Barbeiro: %d\n", id, qual_cadeira);

        //Acorda o barbeiro para ser atendido
        sem_post(&sem_escreve_nome_identificador);

        //Senta na cadeira do respectivo barbeiro
        sem_wait(&sem_cad_barbeiro[qual_cadeira]);

        //Informa que esta pronto para o corte
        sem_post(&sem_cliente_cadeira[qual_cadeira]);

        //Informa que tem cadeiras disponiveis na fila de espera
        sem_post(&sem_cadeiras);

        //Espera o barbeiro realizar o corte
        sem_wait(&sem_cabelo_cortado[qual_cadeira]);

        //Depois do corte, saio da cadeiro do respectivo barbeiro
        sem_post(&sem_cad_barbeiro[qual_cadeira]);

        printf("Cliente %d saiu da barbearia\n", id);

    }
    /*Nao tem cadeira disponivel e por consequencia,
    tambem nao tem barbeiro disponivel,
    e o cliente sai da barbearia*/
    else
    {
        printf("SAINDO\n");
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc <= 3)
    {
        printf("Favor informar a quantidade de barbeiros e suas cadeiras,\n");
        printf("a quantidade de cadeiras de espera para clientes, \n");
        printf("a quantidade de trabalho minimo de cada barbeiro.\n");

        return 0;
    }
    int qtd_barbeiros_cadeiras = atoi(argv[1]);
    int qtd_cadeiras_espera = atoi(argv[2]);
    int qtd_trabalho_minimo = atoi(argv[3]);

    /*--Alocando memoria para o array de barbeiros--*/
    Barbeiro *barbeiros;
    barbeiros = malloc(sizeof(Barbeiro) * qtd_barbeiros_cadeiras);
    if(barbeiros == NULL)
    {
        printf("ERRO AO CONTRATAR BARBEIROS\n");
        return 0;
    }

    /*--CRIANDO OS SEMAFOROS--*/

    //Utilizado para verificar as cadeiras de espera
    sem_t sem_cadeiras;

    /*Semaforos utilizados para saber qual barbeiro foi acordado
    e por consequencia, atendendo*/
    sem_t sem_escreve_nome_identificador;
    sem_t sem_le_nome_identificador;

    //Array dada a quantidade de barbeiros
    sem_t *sem_cad_barbeiro;

    //Array dada a quantidade de barbeiros
    sem_t *sem_cabelo_cortado;

    //Array dada a quantidade de barbeiros
    sem_t *sem_cliente_cadeira;

    sem_cad_barbeiro = (sem_t*) malloc(sizeof(sem_t) * qtd_barbeiros_cadeiras);
    sem_cabelo_cortado = (sem_t*) malloc(sizeof(sem_t) * qtd_barbeiros_cadeiras);
    sem_cliente_cadeira = (sem_t*) malloc(sizeof(sem_t) * qtd_barbeiros_cadeiras);

    /*--INICIALIZANDO OS SEMAFOROS--*/
    //Quantidade de cadeiras disponiveis para espera
    sem_init(&sem_cadeiras, 0, qtd_cadeiras_cliente);
    //Informar qual barbeiro vai atender
    sem_init(&sem_escreve_nome_identificador, 0, 1);
    //Identificar qual barbeiro esta cortando o cabelo
    sem_init(&sem_le_nome_identificador, 0, 0);

    for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
    {
        sem_init(&sem_cad_barbeiro[i], 0, 1);
        sem_init(&sem_cliente_cadeira[i], 0, qtd_cadeiras_cliente);
        sem_init(&sem_cabelo_cortado[i], 0, 0);
    }

    /*--CRIANDO O VETOR DE TRABALHO--*/
    int trabalhos_barbeiro[qtd_barbeiros_cadeiras];
    int identificador_barbeiro;

    if(qtd_min_atender < 1)
    {
        //Entao todos ja realizaram o trabalho minimo(0x)
        for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
        {
            trabalhar_barbeiro[i] = 1;
        }
    }
    else
    {
        for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
        {
            trabalhar_barbeiro[i] = 0;
        }
    }

    /*--ASSOCIANDO AS ESTRUTURAS CRIADAS--*/
    for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
    {
        barbeiros[i].id = i;
        barbeiros[i].qtd_clientes_atendidos = 0;
        barbeiros[i].qtd_min_atender = qtd_trabalho_minimo;
        //O ID do barbeiro eh a posicao no vetor de trabalho
        barbeiros[i].trabalhos_barbeiro = trabalhos_barbeiro;
        barbeiros[i].sem_escreve_nome_identificador = &sem_escreve_nome_identificador;
        barbeiros[i].sem_le_nome_identificador = &sem_le_nome_identificador;
        barbeiros[i].sem_cliente_cadeira = sem_cliente_cadeira;
        barbeiros[i].sem_cabelo_cortado = sem_cabelo_cortado;

    }


    //pthread_t clientes[n_clientes], barbeiros[n_barbeiros];
    //int id_clientes[n_clientes], id_barbeiros[n_barbeiros];

    
    /*-Iniciando os semaforos-*/

    //Quantidade de cadeiras disponiveis para espera
    sem_init(&sem_cadeiras, 0, 5);
    
    //Informar qual barbeiro vai atender
    sem_init(&sem_escreve_nome_identificador, 0, 1);

    //Identificar qual barbeiro esta cortando o cabelo
    sem_init(&sem_le_nome_identificador, 0, 0);

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