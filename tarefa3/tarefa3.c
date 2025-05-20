/* -> Enunciado: Aprovar a necessidade de se ter maquinas maiores (mais cores para resolver um problema grande): Na supercomputação é resolvida com o aumento exponencial do número de cores ao invés de uma melhora dos cores em si, ou seja,
quanto maior o número de processamentos, maior a cobertura de processamento. O intuito é ver como o processamento paralelo é preciso/necessário para a computação de alto desempenho.
Fazer um programa que calcule o número aproximado de pi usando uma série matemática, variando o número de iterações e medindo o tempo de execução.
Comparar os valores obtidos pelo valor real de pi e analise como a accuracy melhora com mais processamentos.
Reflita sobre como esse comporamente se repete em aplicações reais que demandam mais precisão, como ismulações fisicas.


-> Relatório <-
Foi utilizado a Fóruma da série de Leibniz, a qual diz que o somatório de -1^n / (2 * n + 1), com  n indo de 0 á infinito, é possivel chegar ao resultado de PI/4, portando ao multiplicarmos o resultado por 4, é possível obter 4.
No programa, foram feitas 9 baterias de testes com 9 diferentes números de iterações, com a próxima iteração sempre sendo 10 vezes maior que a anterior.
Assim, medindo o tempo o tempo e a precisão de cada iteração é possível observar que: quanto maior o número de iterações, maior a precisão do resultado.
Portanto, nesse sentido, a computação paralela se faz vantajosa, pois com ela é possível termos inúmeras iterações simultaneamente, aumentando a "chance", por assim dizer, de termos uma precisão de 100%.
Isso ocorre, pois

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double leibnizSum(int iterations)
{
    double sum = 0.0;
    for (int i = 0; i < iterations; i++)
    {
        sum += pow(-1, i) / (2.0 * i + 1.0);
    }
    return 4.0 * sum;
}

double accuracyTax(double calculated, double actual)
{
    double absolute_error = fabs(actual - calculated);
    double max_possible_error = fabs(actual);

    double normalized_error = absolute_error / max_possible_error;

    return 1.0 - normalized_error;
}

int main()
{
    double pi_actual = M_PI;
    long long tests[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000, 1000000000, 10000000000};

    for (long long i = 0; i < 9; i++)
    {
        clock_t start = clock();
        double pi_calculated = leibnizSum(tests[i]);
        clock_t end = clock();

        double fullTime = ((double)(end - start)) / CLOCKS_PER_SEC;
        double accuracy = accuracyTax(pi_calculated, pi_actual);

        printf("Numero do teste: %lld\n", i + 1);
        printf("Numero de iteracoes: %lld\n", tests[i]);
        printf("Tempo de calculo: %.11f segundos\n", fullTime);
        printf("Numero aproximado de pi: %.11f\n", pi_calculated);
        printf("Valor real de pi: %.11f\n", pi_actual);
        printf("Precisao: %.11f%%\n\n", accuracy * 100);
    }

    return 0;
}