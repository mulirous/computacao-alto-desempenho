/*
Implemente três laços em C para
investigar os efeitos do paralelismo ao
nível de instrução (ILP): 1) inicialize um
vetor com um cálculo simples; 2) some
seus elementos de forma acumulativa,
criando dependência entre as iterações;
e 3) quebre essa dependência utilizando
múltiplas variáveis. Compare o tempo de
execução das versões compiladas com
diferentes níveis de otimização (O0, O2,
O3) e analise como o estilo do código e
as dependências influenciam o
desempenho.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void population(long long *array, long long array_size, clock_t *start, clock_t *end, double *time_spent)
{
    *start = clock();
    for (long long i = 0; i < array_size; i++)
    {
        array[i] = i + 2;
    }
    *end = clock();

    *time_spent = (double)(*end - *start) / CLOCKS_PER_SEC;

    printf("1 - Tempo de populacao do vetor: %.11f segundos\n", *time_spent);
}

void arraySum(long long *array, long long array_size, clock_t *start, clock_t *end, double *time_spent, long long *sum)
{
    *sum = 0; // Inicializa a soma
    *start = clock();
    for (long long i = 0; i < array_size; i++)
    {
        *sum += array[i];
    }
    *end = clock();

    *time_spent = (double)(*end - *start) / CLOCKS_PER_SEC;

    printf("2 - Tempo da soma acumulativa: %.11f segundos\n", *time_spent);
}

void arraySumMultipleVars(long long *array, long long array_size, clock_t *start, clock_t *end, double *time_spent, long long *totalSum)
{
    long long sum1 = 0;
    long long sum2 = 0;

    *start = clock();
    // Garantir que não ultrapassamos os limites do array
    long long limit = array_size - (array_size % 2);

    for (long long i = 0; i < limit; i += 2)
    {
        sum1 += array[i];
        sum2 += array[i + 1];
    }

    // Tratar elemento final se o tamanho for ímpar
    if (array_size % 2 != 0)
    {
        sum1 += array[array_size - 1];
    }

    *end = clock();
    *totalSum = sum1 + sum2;

    *time_spent = (double)(*end - *start) / CLOCKS_PER_SEC;

    printf("3 - Tempo da soma com multiplas variaveis: %.11f segundos\n", *time_spent);
}

int main()
{
    // Tamanho do vetor
    long long arr_size[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    for (int i = 0; i < 8; i++)
    {
        printf("\n\nTeste de numero: %d\n", i + 1);
        printf("Tamanho do vetor: %lld\n", arr_size[i]);

        // Alocação correta do vetor
        long long *arr = malloc(arr_size[i] * sizeof(long long));

        if (arr == NULL)
        {
            printf("Erro na alocação do vetor\n");
            return -1;
        }

        // Variáveis para medição de tempo
        clock_t begin, end;
        double time_spent;

        // Laço 1: População do vetor
        population(arr, arr_size[i], &begin, &end, &time_spent);

        // Laço 2: Soma acumulativa
        long long sum = 0;
        arraySum(arr, arr_size[i], &begin, &end, &time_spent, &sum);

        // Laço 3: Soma com múltiplas variáveis
        long long totalSum = 0;
        arraySumMultipleVars(arr, arr_size[i], &begin, &end, &time_spent, &totalSum);

        printf("\nResultado da soma do primeiro vetor: %lld\n", sum);
        printf("Resultado da soma com multiplas variaveis: %lld\n", totalSum);

        // Verificação das somas
        if (sum == totalSum)
        {
            printf("\nAs duas somas são iguais.\n");
            printf("Soma acumulativa: %lld\n", sum);
            printf("Soma com múltiplas variáveis: %lld\n", totalSum);
        }
        else
        {
            printf("\nAs duas somas são diferentes!\n");
            printf("Soma acumulativa: %lld\n", sum);
            printf("Soma com múltiplas variáveis: %lld\n", totalSum);
        }

        free(arr); // Libera a memória alocada
    }
    return 0;
}