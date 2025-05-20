#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

// Implemente em C a estimativa estocástica de π. Paralelize com #pragma omp parallel for e explique o resultado incorreto. Corrija a condição de corrida utilizando o #pragma omp critical e reestruturando com #pragma omp parallel seguido de #pragma omp for e aplicando as cláusulas private, firstprivate, lastprivate e shared. Teste diferentes combinações e explique como cada cláusula afeta o comportamento do programa. Comente também como a cláusula default(none) pode ajudar a tornar o escopo mais claro em programas complexos.

#define N 100000000 // Número de pontos

void monteCarloPi(long n)
{
    int hit = 0;
    double x, y;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    // O código é seuqencial, portando faz uma iteração de cada vez
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
