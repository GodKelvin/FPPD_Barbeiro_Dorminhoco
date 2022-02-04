#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "barbearia.h"
//#include <unistd.h>

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

    if(qtd_trabalho_minimo < 1 || qtd_cadeiras_espera < 1)
    {
        //Entao todos ja realizaram o trabalho minimo(0x)
        //Ou entao, a barbearia nao estah apta a receber nem um cliente.
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
            printf("NUMERO DE THREADS O TAMANHO MAXIMO!\n");
            printf("-----\n");
            printf("O numero maximo de threads suportado pelo kernel dos sistemas "); 
            printf("operacionais 64 bits eh de cerca de 32 mil . Isto eh uma convencao, nao uma regra. ");
            printf("Como eu crio threads clientes ate todos os barbeiros atingerem o trabalho minimo, ");
            printf("pode acontecer de threads clientes terem sido criados mas outras anteriores ");
            printf("ainda nao terminarem os seus respectivos processos (corte de cabelo/entrar e sair ");
            printf("da barbearia) e por conta disso, o sistema encerra o programa para nao usarem ");
            printf("uma quantidade enorme de recursos. Por favor, teste valores menores.\n");
            printf("REFERENCIAS SOBRE ISSO: \n");
            printf("https://stackoverflow.com/questions/3419283/pthread-create-enomem-around-32000-threads\n");
            return 0;
        }
        id_cliente++;
    }

    /*--RESULTADOS DO EXPEDIENTE DOS BARBEIROS--*/
    for(int i = 0; i < qtd_barbeiros_cadeiras; i++)
    {
        printf("barbeiro %d atendeu %d clientes\n", barbeiros[i].id, barbeiros[i].qtd_clientes_atendidos);
    }

    /* Barbeiros morrem */

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