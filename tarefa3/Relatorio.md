# Relatório de Computação de Alto Desempenho: Aproximação de Pi

## 1. Enunciado

Aprovar a necessidade de se ter maquinas maiores (mais cores para resolver um problema grande): Na supercomputação é resolvida com o aumento exponencial do número de cores ao invés de uma melhora dos cores em si, ou seja,
quanto maior o número de processamentos, maior a cobertura de processamento. O intuito é ver como o processamento paralelo é preciso/necessário para a computação de alto desempenho.
Fazer um programa que calcule o número aproximado de pi usando uma série matemática, variando o número de iterações e medindo o tempo de execução.
Comparar os valores obtidos pelo valor real de pi e analise como a precisão melhora com mais processamentos.
Reflita sobre como esse comporamente se repete em aplicações reais que demandam mais precisão, como simulações fisicas.

## 2. Metodologia Aplicada

### 2.1 Série de Leibniz

A série de Leibniz para aproximação de pi é definida pela fórmula:

π/4 = Σ ((-1)^n / (2n + 1)), onde n vai de 0 a infinito

Matematicamente, isso significa:
- Somar termos alternados (-1 elevado a n)
- Dividir cada termo por (2n + 1)
- Multiplicar o resultado final por 4 para obter pi

### 2.2 Configuração dos Testes
- Número de séries de testes: 9
- Iterações testadas: 10, 100, 1.000, 10.000, 100.000, 1.000.000, 10.000.000, 1.000.000.000, 10.000.000.000
- Métricas coletadas:
  * Número de iterações
  * Tempo de processamento
  * Valor calculado de pi
  * Precisão (accuracy)

### 2.3 Algoritmo Implementado
```c
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

        printf("Numero do teste: %d\n", i + 1);
        printf("Numero de iteracoes: %d\n", tests[i]);
        printf("Tempo de calculo: %.11f segundos\n", fullTime);
        printf("Numero aproximado de pi: %.11f\n", pi_calculated);
        printf("Valor real de pi: %.11f\n", pi_actual);
        printf("Precisao: %.11f%%\n\n", accuracy * 100);
    }

    return 0;
}
```

## 3. Resultados e Análise

### 3.1 Análise

```powershell
Numero do teste: 1
Numero de iteracoes: 10
Tempo de calculo: 0.00000000000 segundos
Numero aproximado de pi: 3.04183961893
Valor real de pi: 3.14159265359
Precisao: 96.82476228908%

Numero do teste: 2
Numero de iteracoes: 100
Tempo de calculo: 0.00000000000 segundos
Numero aproximado de pi: 3.13159290356
Valor real de pi: 3.14159265359
Precisao: 99.68169807057%

Numero do teste: 3
Numero de iteracoes: 1000
Tempo de calculo: 0.00000000000 segundos
Numero aproximado de pi: 3.14059265384
Valor real de pi: 3.14159265359
Precisao: 99.96816901934%

Numero do teste: 4
Numero de iteracoes: 10000
Tempo de calculo: 0.00000000000 segundos
Numero aproximado de pi: 3.14149265359
Valor real de pi: 3.14159265359
Precisao: 99.99681690115%

Numero do teste: 5
Numero de iteracoes: 100000
Tempo de calculo: 0.00100000000 segundos
Numero aproximado de pi: 3.14158265359
Valor real de pi: 3.14159265359
Precisao: 99.99968169011%

Numero do teste: 6
Numero de iteracoes: 1000000
Tempo de calculo: 0.01400000000 segundos
Numero aproximado de pi: 3.14159165359
Valor real de pi: 3.14159265359
Precisao: 99.99996816901%

Numero do teste: 7
Numero de iteracoes: 10000000
Tempo de calculo: 0.13700000000 segundos
Numero aproximado de pi: 3.14159255359
Valor real de pi: 3.14159265359
Precisao: 99.99999681690%

Numero do teste: 8
Numero de iteracoes: 1000000000
Tempo de calculo: 13.33800000000 segundos
Numero aproximado de pi: 3.14159265259
Valor real de pi: 3.14159265359
Precisao: 99.99999996811%

Numero do teste: 9
Numero de iteracoes: 1410065408
Tempo de calculo: 18.89200000000 segundos
Numero aproximado de pi: 3.14159265288
Valor real de pi: 3.14159265359
Precisao: 99.99999997737%
```

### 3.2 O que se pode extrair dos resultados?

1. A precisão aumenta significativamente com o número de iterações
2. Crescimento do tempo de processamento com o aumento das iterações
3. Próxima de 100% com 10 milhões de iterações

## 4. Conclusões sobre Processamento Paralelo

- Cada iteração da série pode ser calculada independentemente, assim não haveria um tempo de espera muito significativo para se obter os resultados, como observamos o teste 8 levou cerca de 13 segundos para processar, e o teste 9 levou cerca de 19 segundos, portando, ao invés de esperar 13 segundos para processar o teste 8 e depois mais 19 segundos para processar o teste 9, somando quase 33 segundos para termos ambos os resultados, no processamento paralelo devaria apenas cerca de 6 segundos de diferença de processamento entre um teste e outro, pois os dois estariam sendo executados simultaneamente, e ao todo levaria apenas o tempo de 19 segundos, quase metade do tempo, para termos ambos os resultados, alcançando um alto desempenho computacional.
- Então, para problemas de dimensões imensuravelmente maiores e com uma necessidade de resultado rápido, o processamento paralelo é extremamente eficiente e funcional. 