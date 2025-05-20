// Sugestão do professor -> fazer um loop alterando o número de threads

// #define MAX 128;
// int main()
// {
//     int sum = 0;
// #pragma omp parallel for num_threads(x) // Colocar o número de cores do processador
//     for (int i = 0, i < MAX; i++)
//     {
//         sum += tan(sin(i) * cos(1)) + sqrt(i);
//     }
// }

// Desenvolver em casa
// imeplementar e verificar o tempo, testar diversos números de loop
// Comparar o tempo com o memory-bound

// // Programação Hardware MultiThread com OpenMP

#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

#define N 10000000000 // aqui há 10 zeros, onde causa uma perca de eficiência, contudo em 8 zeros ainda há ganho

int main()
{
    long n = N;
    long sum;

    struct timeval start, end;
    double tempo;

    gettimeofday(&start, NULL); // Início do tempo

    // Este loop irá fazer com que cada thread consuma um custo computacional consideravel, havendo uam "concorrência" ca capacidade computacional da máquina, acabando por "estourar" a capacidade computacional da máquina (compute-bound)
    // #pragma omp parallel for num_threads(10) // Quantidade de core's da minha máquina
    for (long i = 0; i < n; i++)
    {
        sum += tan(sin(i) * cos(1)) + sqrt(i);
    }

    gettimeofday(&end, NULL); // Fim do tempo

    // Calcula o tempo em segundos (diminui os segundos do termino dos segundos do início somado á diferença dos microssegundos do término aos microssegundos do início )
    tempo = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("\n\nTempo de execução com OpenMP para CPU bound (gettimeofday): %f segundos\n\n", tempo);

    return 0;
}
