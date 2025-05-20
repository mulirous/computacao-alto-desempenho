#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>

// Implemente em C a estimativa estocástica de π. Paralelize com #pragma omp parallel for e explique o resultado incorreto. Corrija a condição de corrida utilizando o #pragma omp critical e reestruturando com #pragma omp parallel seguido de #pragma omp for e aplicando as cláusulas private, firstprivate, lastprivate e shared. Teste diferentes combinações e explique como cada cláusula afeta o comportamento do programa. Comente também como a cláusula default(none) pode ajudar a tornar o escopo mais claro em programas complexos.
// calcular variáveis privadas, e ao final reduzir a uma unica variável global, por meio de criar uma zona crítica em uam zona paralela
// Para reduzir, é preciso fazer uma variável de hits totais (ou seja, global) e uma variável de hits paralelo, assim essa variável de hits paralelo irá ser uma variável privada, e ao final sera feita a redução para a quantidade de hits totais;

#define N 100000000 // Número de pontos

void monteCarloPi(long n)
{
    int hit = 0;
    double x, y;

    struct timeval start, end;
    gettimeofday(&start, NULL);

#pragma omp parallel for // O resultado é incorreto, pois a cláusula 'for' paraleliza o for imediamente abaixo dele, e aí o quebra em pedaços e os distribui em diferentes threads, contudo essas diferentes threads atualizam o mesmo local na memória, e como a sincronização não é definida, nem todas chegam a mesma resposta, portando acabam atualizando de maneiras divergentes o mesmo espaço de memória, acabando por fazer com que o algoritmo certo erre a estimativa ao final
    for (long i = 0; i < n; i++)
    {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;

        if (x * x + y * y <= 1.0)
            hit++;
    }

    gettimeofday(&end, NULL);

    double pi = 4.0 * hit / n;
    double tempo = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("Estimativa de pi: %f\n", pi);
    printf("Tempo de execução: %f segundos\n", tempo);
}

int main()
{
    monteCarloPi(N);
    return 0;
}
