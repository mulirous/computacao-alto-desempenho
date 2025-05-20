// Objetivo: refletir e comentar o por que que dá errado e/como mitigar alguma uma ineficiência

#include <sys/time.h>
#include <stdio.h>
#include <omp.h>

// Função para verificar se um número é primo
int ehPrimo(int num)
{
    if (num < 2)
        return 0;
    for (int i = 2; i * i <= num; i++)
    {
        if (num % i == 0)
            return 0;
    }
    return 1;
}

#define N 100000

// Função principal
int main()
{
    int contadorSequencial, contadorParalelo = 0;
    struct timeval start, end;
    double tempoSequencial, tempoParalelo;

    gettimeofday(&start, NULL); // Início do tempo

    for (long i = 2; i <= N; i++)
    {
        if (ehPrimo(i))
        {
            contadorSequencial++;
        }
    }

    gettimeofday(&end, NULL); // Fim do tempo

    tempoSequencial = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("Tempo da contagem sequencial: %f\n\n", tempoSequencial);
    printf("Numero de primos encontrados: %d\n\n", contadorSequencial);

    gettimeofday(&start, NULL); // Início do tempo

#pragma omp parallel for num_threads(10)
    for (long i = 2; i <= N; i++)
    {
#pragma omp critical // Faz com que entre uma thread por vez, portanto resolvendo a race condition não havendo erro na contagem do número de primos, apenas no desempenho
        if (ehPrimo(i))
        {
            contadorParalelo++;
        }
    }

    gettimeofday(&end, NULL); // Fim do tempo

    tempoParalelo = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("Tempo da contagem paralela: %f\n\n", tempoParalelo);
    printf("Numero de primos encontrados: %d\n\n", contadorParalelo);

    return 0;
}
