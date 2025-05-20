#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#define N 1000000000 // Número de pontos

void monteCarloPi(long n)
{
    int globalHit = 0;
    double x, y;
    struct timeval start, end;
    gettimeofday(&start, NULL);

#pragma omp parallel default(none) private(x, y) shared(globalHit, n)
    {
        int privateHit = 0;
        long local_n = n / omp_get_num_threads(); // Divide o trabalho entre as threads

        unsigned int seed = omp_get_thread_num() + 1; // Seed para gerar o número aleatório

        for (long i = 0; i < local_n; i++)
        {
            x = (double)rand_r(&seed) / RAND_MAX; // A função rand_r() é thread_safe, ou seja, tem mecanismos de sincronização ao acesso a recursos compartilhados
            y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0)
                privateHit++;
        }

#pragma omp critical // Esta diretiva define uma região crítica que apenas uma thread por vez pode executar. Todas as outras threads que tentarem entrar na região enquanto uma thread já estiver lá, ficam bloqueadas até a liberação. Protege uma região inteira de código, independentemente de quantas variáveis ou instruções existam dentro dela.
        {
            globalHit += privateHit; // Utilizada quando há mais de uma variável compartilhada sendo modificada.
        }
    }

    double pi = 4.0 * globalHit / n;

    gettimeofday(&end, NULL);
    double tempo = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("\nCRITICAL\n");
    printf("Estimativa de pi: %f\n", pi);
    printf("Tempo de execução: %f segundos\n", tempo);
}

int main()
{
    monteCarloPi(N);
    return 0;
}