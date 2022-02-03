#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

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

int barbearia_aberta(int *trabalho_barbeiros, int qtd_barbeiros)
{
    for(int i = 0; i < qtd_barbeiros; i++)
    {
        //Algum barbeiro ainda nao trabalhou o suficiente
        if(trabalho_barbeiros[i] == 0) return 1; 
    }
    //Todos os barbeiros ja trabalharam o suficiente
    return 0;
}

//Thread do barbeiro trabalhando (ou dormindo...)
void *barbeiro_trabalha(void *arg)
{
    //Converter para o ID do barbeiro?
    //int id = *(int*)arg;
    Barbeiro *barbeiro = (Barbeiro*) arg;
    //while(barbearia_aberta(barbeiro->trabalhos_barbeiro, barbeiro->colegas_trabalho))
    while(1)
    {
        //Espera algum cliente o acordar
        sem_wait(barbeiro->sem_escreve_nome_identificador);
        *(barbeiro->identificador_barbeiro) = barbeiro->id;

        //Informo ao cliente para vir cortar o cabelo
        sem_post(barbeiro->sem_le_nome_identificador);

        //Informo que o barbeiro X esta cortando cabelo
        sem_wait(&barbeiro->sem_cliente_cadeira[barbeiro->id]);

        //Informo que o barbeiro terminou de cortar o cabelo
        sem_post(&barbeiro->sem_cabelo_cortado[barbeiro->id]);

        //Incremento a quantidade de clientes atendidos
        barbeiro->qtd_clientes_atendidos++;

        //Verifico se esse barbeiro ja trabalhou o suficiente
        if(barbeiro->qtd_clientes_atendidos >= barbeiro->qtd_min_atender)
        {
            /*Informo ao vetor de trabalho que este barbeiro
            ja trabalhou o suficiente*/

            //INSERIR VERIFCIAR DE BARBEARIA ABERTA?
            barbeiro->trabalhos_barbeiro[barbeiro->id] = 1;
        }
    }

    return NULL;
}

//Thread do cliente indo cortar o cabelo
void *cortar_cabelo(void* arg)
{
    int qual_cadeira;
    Cliente *cliente = (Cliente*) arg;
    /*Verifico se tem vaga disponivel na barbearia,
    ou seja, se positivo, tem pelo menos uma cadeira disponivel
    para espera. O caso de sucesso eh quando tem barbeiro
    disponivel tambem*/
    if(sem_trywait(cliente->sem_cad_espera) == 0)
    {
        //printf("Cliente conseguiu vaga: %d\n", cliente->id);

        //Verifica qual barbeiro estah dormindo
        sem_wait(cliente->sem_le_nome_identificador);

        //Vai ate a cadeira do barbeiro que estava dormindo
        qual_cadeira = *(cliente->identificador_barbeiro);
        //printf("Cliente: %d, Barbeiro: %d\n", id, qual_cadeira);

        //Acorda o barbeiro para ser atendido
        sem_post(cliente->sem_escreve_nome_identificador);

        //Senta na cadeira do respectivo barbeiro
        sem_wait(&cliente->sem_cad_barbeiro[qual_cadeira]);

        //Informa que esta pronto para o corte
        sem_post(&cliente->sem_cliente_cadeira[qual_cadeira]);

        //Informa que tem cadeiras disponiveis na fila de espera
        sem_post(cliente->sem_cad_espera);

        //Espera o barbeiro realizar o corte
        sem_wait(&cliente->sem_cabelo_cortado[qual_cadeira]);

        //Depois do corte, saio da cadeiro do respectivo barbeiro
        sem_post(&cliente->sem_cad_barbeiro[qual_cadeira]);

        //printf("Cliente %d saiu da barbearia\n", cliente->id);
        return NULL;
    }
    /*Nao tem cadeira disponivel e por consequencia,
    tambem nao tem barbeiro disponivel,
    e o cliente sai da barbearia*/
    else
    {
        //printf("SAINDO\n");
        return NULL;
    }
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
    sem_t sem_cad_espera;

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
    sem_init(&sem_cad_espera, 0, qtd_cadeiras_espera);
    //Informar qual barbeiro vai atender
    sem_init(&sem_escreve_nome_identificador, 0, 1);
    //Identificar qual barbeiro esta cortando o cabelo
    sem_init(&sem_le_nome_identificador, 0, 0);

    for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
    {
        sem_init(&sem_cad_barbeiro[i], 0, 1);
        sem_init(&sem_cliente_cadeira[i], 0, 0);
        sem_init(&sem_cabelo_cortado[i], 0, 0);
    }

    /*--CRIANDO O VETOR DE TRABALHO--*/
    int trabalhos_barbeiro[qtd_barbeiros_cadeiras];
    int identificador_barbeiro;

    if(qtd_trabalho_minimo < 1)
    {
        //Entao todos ja realizaram o trabalho minimo(0x)
        for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
        {
            trabalhos_barbeiro[i] = 1;
        }
    }
    else
    {
        for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
        {
            trabalhos_barbeiro[i] = 0;
        }
    }

    /*--ASSOCIANDO AS ESTRUTURAS CRIADAS--*/
    for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
    {
        
        barbeiros[i].id = i;
        barbeiros[i].colegas_trabalho = qtd_barbeiros_cadeiras;
        barbeiros[i].qtd_clientes_atendidos = 0;
        barbeiros[i].identificador_barbeiro = &identificador_barbeiro;
        barbeiros[i].qtd_min_atender = qtd_trabalho_minimo;
        //O ID do barbeiro eh a posicao no vetor de trabalho
        barbeiros[i].trabalhos_barbeiro = trabalhos_barbeiro;
        barbeiros[i].sem_escreve_nome_identificador = &sem_escreve_nome_identificador;
        barbeiros[i].sem_le_nome_identificador = &sem_le_nome_identificador;
        barbeiros[i].sem_cliente_cadeira = sem_cliente_cadeira;
        barbeiros[i].sem_cabelo_cortado = sem_cabelo_cortado;

    }
    /*-----CRIACAO DAS THREADS-----*/
    /*--INICIANDO O EXPEDIENTE DOS BARBEIROS--*/
    for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
    {
        if(pthread_create(&barbeiros[i].id_thread_barber, NULL, barbeiro_trabalha, &barbeiros[i]) != 0)
        {
            perror("Pthread Create Barbeiro Falhou\n");
            return 0;
        }
    }

    /*--ENQUANTO A BARBEARIA ESTIVER ABERTA,
    ESTAH RECEBENDO CLIENTES--*/
    int id_cliente = 0;



    while(barbearia_aberta(trabalhos_barbeiro, qtd_barbeiros_cadeiras))
    {
        
        Cliente *cliente = (Cliente*) malloc(sizeof(Cliente));
        cliente->id = id_cliente;
        cliente->identificador_barbeiro = &identificador_barbeiro;
        cliente->sem_cad_espera = &sem_cad_espera;
        cliente->sem_le_nome_identificador = &sem_le_nome_identificador;
        cliente->sem_escreve_nome_identificador = &sem_escreve_nome_identificador;
        cliente->sem_cad_barbeiro = sem_cad_barbeiro;
        cliente->sem_cliente_cadeira = sem_cliente_cadeira;
        cliente->sem_cabelo_cortado = sem_cabelo_cortado;

        //COMO RESOLVER PROBLEMA DE THREAD MAX?
        /*Devido a configuracao de kernel do linux, o numero maximo de threads
        permitido eh de aproximadamente 32 mil. Caso o programa simplesmente
        se encerre do nada, tente executa-lo com valores menores*/
        if(pthread_create(&cliente->id_thread_cliente, NULL, cortar_cabelo, &cliente[0]) != 0)
        {
            printf("ERROR AO CRIAR CLIENTE: %d\n", id_cliente);
            printf("NUMERO DE THREADS NO LINUX ATINGIU O TAMANHO MAXIMO\n!");
            return 0;
        }
        id_cliente++;
    }


    //Aguardar os barbeiros encerrarem os expediente
    /*
    for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
    {
        pthread_join(barbeiros[i].id_thread_barber, NULL);
    }
    */
    


    /*--RESULTADOS DO EXPEDIENTE DOS BARBEIROS--*/
    for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
    {
        printf("barbeiro %d atendeu %d clientes\n", barbeiros[i].id, barbeiros[i].qtd_clientes_atendidos);
    }


    free(barbeiros);
    //free(clientes);
    free(sem_cad_barbeiro);
    free(sem_cabelo_cortado);
    free(sem_cliente_cadeira);

    sem_destroy(sem_cad_barbeiro);
    sem_destroy(sem_cabelo_cortado);
    sem_destroy(sem_cliente_cadeira);
    sem_destroy(&sem_cad_espera);
    sem_destroy(&sem_escreve_nome_identificador);
    sem_destroy(&sem_le_nome_identificador);

    return 0;
}