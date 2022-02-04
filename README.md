# Os Barbeiros Dorminhocos


## Resumo
Trabalho realizado durante a disciplina de Fundamentos de Programação Paralela e Distribuída (2021/2), do curso Bacharelado em Sistemas de informação (BSI) do IFES-Campus Serra.

## Docente da Disciplina
Flávio Severiano Lamas

## Assunto
Sincronização de Threads sem cair em condições de disputa. 

## Para Compilar
```
gcc -Wall -pedantic barbearia.c barbearia.h main.c -pthread -o exec
```

## Para executar
```
./exec N X Z
```
Sendo N Quantidade de barbeiros (e suas cadeiras), X quantidade de cadeiras de espera para clientes e Z o trabalho mínimo dos barbeiros.
## Exemplo
```
./exec 1 1 1
./exec 5 5 5
./exec 10 10 10
./exec 100 10 100
./exec 20 5 500
./exec 100 2 50
```


# Testes

A seguir, alguns testes utilizando diversos valores como entrada.

![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_1.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_10.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_11.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_12.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_13.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_14.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_15.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_16.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_17.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_2.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_3.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_4.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_5.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_6.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_7.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_8.png)
![imagem_exec](https://github.com/GodKelvin/FPPD_Barbeiro_Dorminhoco/blob/master/imagens_readme/teste_9.png)

## Descrição

Na barbearia há N barbeiros, N cadeiras de barbeiro e X cadeiras para eventuais clientes
esperarem a vez.

Quando não há clientes, os barbeiros sentam-se nas suas cadeiras de barbeiro e cai no sono.

Quando chega um cliente, ele precisa acordar algum barbeiro.

Se outros clientes chegarem enquanto o barbeiro estiver cortando o cabelo de um cliente, eles se sentarão (se houver cadeiras vazias) ou sairão da barbearia (se todas as cadeiras estiverem ocupadas)

O problema é programar o barbeiro e os clientes sem cair em condições de disputa. Esse problema é semelhante a situações com várias filas, como uma mesa de atendimento de telemarketing com diversos atendentes e com um sistema computadorizado de chamadas em espera, atendendo a um número limitado de chamadas que chegam.

Seu programa deve paralelizar isso, de forma a evitar todos os problemas padroes da paralelizacao classica, e receber como argumentos N e X pela linha de comando alem de um Z que eh o numero de clientes minimo que cada barbeiro deve atender.

O codigo final tem que imprimir exatamente na ordem dos barbeiros o quanto de cliente eles atenderam.

Seu codigo final NAO DEVE IMPRIMIR NENHUM DEBUG, ou seja, comentem suas linhas de codigo de debug MAS DEIXEM ELA NO CODIGO para eu analisar.

exemplo:

./barbeiros 1 5 10<br>
barbeiro 0 atendeu 10 clientes<br>

./barbeiros 2 5 10<br>
barbeiro 0 atendeu 15 clientes<br>
barbeiro 1 atendeu 10 clientes<br>
