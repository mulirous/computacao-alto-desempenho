#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#define N 1000000000 // Número de pontos

void monteCarloPi(long n)
{
    int hit = 0;
    double x, y;
    struct timeval start, end;
    gettimeofday(&start, NULL);

#pragma omp parallel reduction(+ : hit) // A cláusula reduction no OpenMP permite que cada thread mantenha uma cópia privada de uma variável durante a execução paralela e, no final, combine (reduza) esses valores em um único resultado usando uma operação associativa como +, *, min, max, etc.
                                        // Evita também o uso de métodos de sincronização manual, como o atomic e o critical, dando melhor desempenho
                                        // Esta claúsula não tem parâmetros default, ela precisa dos seguintes parametros (operacao : variável)
    {
        unsigned int seed = omp_get_thread_num() + 1;
        double x, y;

#pragma omp for
        for (long i = 0; i < n; i++)
        {
            x = (double)rand_r(&seed) / RAND_MAX;
            y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0)
                hit++;
        }
    }

    double pi = 4.0 * hit / n;

    gettimeofday(&end, NULL);
    double tempo = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("\nREDUCTION\n");
    printf("Estimativa de pi: %f\n", pi);
    printf("Tempo de execução: %f segundos\n", tempo);
}

int main()
{
    monteCarloPi(N);
    return 0;
}