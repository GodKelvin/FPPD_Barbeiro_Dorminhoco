# Os Barbeiros Dorminhocos

## Para executar


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
