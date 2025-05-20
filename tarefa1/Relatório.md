# Relatório Tarefa 1

**Disciplina:** Computação de Alto Desempenho - IMD1116  
**Aluno:** Murilo Antonio Lima da Costa  
**Data:** 31 de março de 2025

## 1. Introdução

Este relatório apresenta uma análise da comparação de duas implementações diferentes para a multiplicação de matriz por vetor (MxV). Para a implementação do algoritmo é utilizada a linguagem C, a qual permite uma melhor manipulação da memória. O objetivo principal é investigar como o padrão de acesso à memória impacta o desempenho de algoritmos aparentemente equivalentes do ponto de vista matemático.

As duas implementações analisadas são:

- **Row Major (acesso por linhas)**: Percorre a matriz com laços aninhados onde o laço externo itera sobre as linhas e o laço interno sobre as colunas.
- **Column Major (acesso por colunas)**: Percorre a matriz com laços aninhados onde o laço externo itera sobre as colunas e o laço interno sobre as linhas.

## 2. Metodologia

### 2.1 Implementação

Foram implementadas duas funções para multiplicação de matriz por vetor:

**Row Major**

```c
void rowMajor(int **matrix, int *vector, int *result, long size, clock_t *start_time, clock_t *end_time)
{
    *start_time = clock();

    for (long i = 0; i < size; i++)
    {
        result[i] = 0;
        for (long j = 0; j < size; j++)
        {
            result[i] += matrix[i][j] * vector[j];
        }
    }

    *end_time = clock();
}
```

**Column Major**

```c
void columnMajor(int **matrix, int *vector, int *result, long size, clock_t *start_time, clock_t *end_time)
{
    *start_time = clock();

    for (long i = 0; i < size; i++)
    {
        result[i] = 0;
    }

    for (long j = 0; j < size; j++)
    {
        for (long i = 0; i < size; i++)
        {
            result[i] += matrix[i][j] * vector[j];
        }
    }

    *end_time = clock();
}
```

### 2.2 Ambiente de Testes

- **Sistema Operacional:** Ubuntu 22.04.5 LTS
- **Processador:** 12th Gen Intel(R) Core(TM) i5-1235 (12 CPUs)
- **Memória RAM:** 16000MB RAM
- **Compilador:** gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0

### 2.3 Procedimento de Teste

1. Foram realizados testes com matrizes quadradas de diferentes dimensões: 100, 500, 1000, 2000, 5000 e 10000.
2. Para cada dimensão, foram executadas as duas implementações (Row Major e Column Major).
3. O tempo de execução de cada implementação foi medido utilizando a função `clock()` da biblioteca `time.h`.
4. Para garantir que o compilador não otimizasse demais o código removendo cálculos aparentemente não utilizados, os resultados foram usados em operações subsequentes.

## 3. Resultados

### 3.1 Tempos de Execução

| Tamanho da Matriz | Tempo Row Major (s) | Tempo Column Major (s) | Diferença (s) |
| ----------------- | ------------------- | ---------------------- | ------------- |
| 100 × 100         | 0.00005500000       | 0.00005500000          | 0.00000000000 |
| 500 × 500         | 0.00121000000       | 0.00128200000          | 0.00007200000 |
| 1000 × 1000       | 0.00681200000       | 0.00859500000          | 0.00178300000 |
| 2000 × 2000       | 0.01508200000       | 0.03320400000          | 0.00178300000 |
| 5000 × 5000       | 0.11359400000       | 0.28830400000          | 0.17471000000 |
| 10000 × 10000     | 0.24638100000       | 0.91110200000          | 0.66472100000 |

### 3.2 Ponto de Divergência Significativa

Com base nos resultados obtidos, observamos que os tempos de execução começam a divergir significativamente a partir de matrizes de tamanho 10000 × 10000, pois como mostrado no DataFrame acima a diferença é quase de 7ms.

## 4. Análises

### 4.1 Padrão de Acesso à Memória

**Row Major (acesso por linhas):**

- Acessa os elementos da matriz de forma contínua na memória, já que em C as matrizes são armazenadas em ordem de linha.
- Para cada linha processada, o vetor é iterado.
- O padrão de acesso à memória é consistente com a forma como os dados são organizados fisicamente.

**Column Major (acesso por colunas):**

- Acessa elementos não contínuos na memória, realizando "saltos" entre posições.
- Para cada coluna processada, todas as linhas são visitadas (cada elemento da coluna), causando acesso não sequencial à matriz.
- O padrão de acesso à memória vai contra a organização física dos dados.

### 4.2 Impacto da Memória Cache

**Princípio da Localidade:**

1. **Localidade Temporal:** Dados acessados recentemente têm alta probabilidade de serem acessados novamente em breve.
2. **Localidade Espacial:** Dados próximos a um endereço de memória recentemente acessado têm maior probabilidade de serem acessados em seguida.

**Comportamento das Implementações:**

**Row Major:**

- **Localidade Espacial:** Beneficia-se fortemente do princípio de localidade espacial, pois acessa elementos consecutivos na memória, devido a sua organização física.
- **Cache Hits:** Alta taxa de acertos no cache (cache hits), minimizando acessos à memória principal.

**Column Major:**

- **Localidade Espacial:** Viola o princípio de localidade espacial ao realizar saltos entre diferentes linhas.
- **Cache Misses:** Alta taxa de falhas no cache (cache misses), necessitando de mais acessos à memória principal.

### 4.3 Hierarquia de Memória e Níveis de Cache

Em sistemas computacionais modernos, a memória é organizada em uma hierarquia:

1. **Registradores** (nanossegundos)
2. **Cache L1** (poucos nanossegundos)
3. **Cache L2** (dezenas de nanossegundos)
4. **Cache L3** (centenas de nanossegundos)
5. **Memória Principal (RAM)** (centenas de nanossegundos)
6. **Armazenamento Secundário** (milissegundos)

O impacto da diferença entre os padrões de acesso aumenta significativamente a medida em que o tamanho do item a ser iterado, o qual ultrapassa o limite dos níveis da memória cache.

### 4.4 Explicação da Divergência em Função do Tamanho da Matriz

A divergência significativa observada a partir de matrizes de tamanho [tamanho] × [tamanho] pode ser explicada pelos seguintes fatores:

1. **Tamanho do Cache:** Quando a matriz ultrapassa o tamanho do cache L3, o número de cache misses aumenta drasticamente para o acesso por colunas.

2. **Largura das Linhas de Cache:** Cada linha de cache carrega múltiplos elementos contíguos. O acesso por linhas aproveita todos os elementos carregados, enquanto o acesso por colunas utiliza apenas um elemento de cada linha carregada.

## 5. Conclusão

A análise dos resultados demonstra o impacto que o padrão de acesso à memória tem sobre o desempenho. As duas implementações da multiplicação de matriz por vetor apresentam diferenças significativas de desempenho, especialmente para matrizes de grande dimensão.

O método Row Major (acesso por linhas) demonstrou ser consistentemente mais eficiente, com a diferença se tornando claramente significativa para matrizes de dimensão 10000 × 10000, conforme as análises. Esta superioridade se deve principalmente à melhor utilização da hierarquia de memória, especificamente:

1. **Maior aproveitamento da localidade espacial**, reduzindo a necessidade de buscas na memória principal.
2. **Melhor utilização das linhas de cache**, maximizando o uso dos dados já carregados.
3. **Redução de cache misses**, diminuindo a latência média de acesso à memória.

Estes resultados demonstram claramente que para alcançar alto desempenho computacional não basta apenas implementar algoritmos matematicamente corretos, mas é fundamental adaptá-los à arquitetura do hardware, maximizando a sua eficiência.

## 6. Código Completo da Implementação

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void rowMajor(int **matrix, int *vector, int *result, long size, clock_t *start_time, clock_t *end_time)
{
    *start_time = clock();

    for (long i = 0; i < size; i++)
    {
        result[i] = 0;
        for (long j = 0; j < size; j++)
        {
            result[i] += matrix[i][j] * vector[j];
        }
    }

    *end_time = clock();
}

void columnMajor(int **matrix, int *vector, int *result, long size, clock_t *start_time, clock_t *end_time)
{
    *start_time = clock();

    for (long i = 0; i < size; i++)
    {
        result[i] = 0;
    }

    for (long j = 0; j < size; j++)
    {
        for (long i = 0; i < size; i++)
        {
            result[i] += matrix[i][j] * vector[j];
        }
    }

    *end_time = clock();
}

void sumVector(int *vector, long size)
{
    long sum = 0;
    for (long i = 0; i < size; i++)
    {
        sum += vector[i];
    }
    printf("%ld\n", sum);
}

int main()
{
    long sizes[] = {100, 500, 1000, 2000, 5000, 10000};
    int num_tests = sizeof(sizes) / sizeof(sizes[0]);

    for (int test = 0; test < num_tests; test++)
    {
        long size = sizes[test];
        printf("\nTeste com matriz de tamanho %ld x %ld\n", size, size);

        int **matrix = malloc(size * sizeof(int *));
        for (long i = 0; i < size; i++)
        {
            matrix[i] = malloc(size * sizeof(int));
        }

        int *vector = malloc(size * sizeof(int));
        int *result_row = malloc(size * sizeof(int));
        int *result_col = malloc(size * sizeof(int));

        for (long i = 0; i < size; i++)
        {
            vector[i] = i + 1;
            for (long j = 0; j < size; j++)
            {
                matrix[i][j] = i * size + j + 1;
            }
        }

        clock_t row_start, row_end;
        clock_t col_start, col_end;

        rowMajor(matrix, vector, result_row, size, &row_start, &row_end);

        columnMajor(matrix, vector, result_col, size, &col_start, &col_end);

        double row_time = ((double)(row_end - row_start)) / CLOCKS_PER_SEC;
        double col_time = ((double)(col_end - col_start)) / CLOCKS_PER_SEC;

        printf("Tempo Row Major:    %f segundos\n", row_time);
        printf("Tempo Column Major: %f segundos\n", col_time);

        double diff = fabs(col_time - row_time);

        printf("Diferença de tempo: %f segundos\n", diff);

        for (long i = 0; i < size; i++)
        {
            free(matrix[i]);
        }
        free(matrix);
        free(vector);
        free(result_row);
        free(result_col);
    }

    return 0;
}
```
