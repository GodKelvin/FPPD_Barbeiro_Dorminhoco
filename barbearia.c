#include <semaphore.h>
#include <stdio.h>
//#include <unistd.h>

#include "barbearia.h"

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
    while(barbearia_aberta(barbeiro->trabalhos_barbeiro, barbeiro->colegas_trabalho))
    //while(1)
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
