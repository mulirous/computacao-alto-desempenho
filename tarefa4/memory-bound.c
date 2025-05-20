
// // Programação Hardware MultiThread com OpenMP

// Sugestão do professor

// int main()
// {
// // gettimeofday(&start, NULL)
// #pragma omp parallel for num_threads(x) // Colocar o número de cores do processador
//     for (int i = 0; i < n; i++)
//     {
//         a[i] = c[i] + b[i] - d[i]
//     }
//     // gettimeofday(&end, NULL)
// }

// Verificar na programação parelala o tempo e comparar com o compute-bound

// // Programação Hardware MultiThread com OpenMP

// Sugestão do professor -> fazer um loop alterando o número de threads

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define N 100000000

int main()
{
    int n = N;
    double *a = (double *)malloc(n * sizeof(double));
    double *b = (double *)malloc(n * sizeof(double));
    double *c = (double *)malloc(n * sizeof(double));
    double *d = (double *)malloc(n * sizeof(double));

    struct timeval start, end;
    double tempo;

    // Inicializando os vetores, contudo esta técnica usando ponteiros não é eficaz, pois os dados são espalhados na memória, e não alocados sequencialmente, como em arrays, assim sendo limitado pelo gargalo de von-noimann, se tornando uma memory-bound task
    for (int i = 0; i < n; i++)
    {
        b[i] = i * 1.0;
        c[i] = i * 2.0;
        d[i] = i * 3.0;
    }

    gettimeofday(&start, NULL); // Início do tempo

    // Este loop vai alternar os acessos da memória, assim o bit da memória em que a thread irá acessar ia ser mpurrados para niveis mais baixo da cache, ou até chegando a memória principal, e talvéz "estoure" o acesso á memória (memory bound) -> O acesso a memória é frequente neste caso, causando o galgalo da limitação pela memória
    // #pragma omp parallel for num_threads(10) // Quantidade de core's da minha máquina
    for (int i = 0; i < n; i++)
    {
        a[i] = c[i] + b[i] - d[i];
    }

    gettimeofday(&end, NULL); // Fim do tempo

    // Calcula o tempo em segundos (diminui os segundos do termino dos segundos do início somado á diferença dos microssegundos do término aos microssegundos do início )
    tempo = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("\n\nTempo de execucao com OpenMP para o memory bound (gettimeofday): %f segundos\n\n", tempo);

    // Libera memória das listas unicamente encadeadas
    free(a);
    free(b);
    free(c);
    free(d);

    return 0;
}

// Verificar na programação parelala o tempo e comparar com o compute-bound