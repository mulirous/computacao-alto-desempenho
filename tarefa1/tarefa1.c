/*
Implemente duas versões da
multiplicação de matriz por vetor (MxV)
em C: uma com acesso à matriz por
linhas (linha externa, coluna interna) e
outra por colunas (coluna externa, linha
interna). Meça o tempo de execução de
cada versão com uma função apropriada
e execute testes com diferentes
tamanhos de matriz. Identifique a partir
de que tamanho os tempos passam a
divergir significativamente e explique por
que isso ocorre, relacionando suas
observações com o uso da memória
cache e o padrão de acesso à memória.
*/

// TODO: Ver a quanridade de ciclos que leva para poder acessar o dado na memória cache (e consequentemente o level da cache)

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
    long sizes[] = {100, 500, 1000, 2000, 5000, 10000, 100000}; // A partir de 10.000 a diferença é extremamente significante
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

        printf("Tempo Row Major:    %.11f segundos\n", row_time);
        printf("Tempo Column Major: %.11f segundos\n", col_time);

        double diff = fabs(col_time - row_time);

        printf("Diferença de tempo: %.11f segundos\n", diff);

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

/*
Relatório de Conclusão Expandido
============= O QUE FOI OBSERVADO? ================
> O Row Major é mais eficaz que o Column Major, principalmente quando o tamanho da matriz e vetor aumentam

======= POR QUE ISSO OCORRE DESSA MANEIRA? ========
> Localidade Espacial e Temporal Explicam a Diferença de Desempenho:

1. Localidade Espacial: (Localização no espaço da memória)
   - Row Major: Acessa elementos sequencialmente na memória
   - Column Major: Realiza saltos entre diferentes linhas de memória

2. Localidade Temporal: (Reutilização de dados recentimente acessados)
   - Row Major: Mantém dados no cache, reutilizando-os rapidamente
   - Column Major: Frequentes substituições de cache, menor reaproveitamento de dados

> O método Row Major minimiza tanto os saltos de memória (localidade espacial) quanto as substituições de cache (localidade temporal), resultando em melhor desempenho.
*/