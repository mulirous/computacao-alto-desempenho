# Guia Completo de Programação em C

## Índice

1. [Introdução](#introdução)
2. [Estrutura Básica](#estrutura-básica)
3. [Tipos de Dados](#tipos-de-dados)
4. [Ponteiros](#ponteiros)
5. [Compilação](#compilação)
6. [Gerenciamento de Memória](#gerenciamento-de-memória)
7. [Estruturas e Tipos Personalizados](#estruturas-e-tipos-personalizados)
8. [Entrada e Saída](#entrada-e-saída)
9. [Bibliotecas Padrão](#bibliotecas-padrão)
10. [Boas Práticas](#boas-práticas)

## Introdução

C é uma linguagem de programação de propósito geral, de baixo nível, que oferece controle direto sobre o hardware e memória do computador. Desenvolvida por Dennis Ritchie no início dos anos 1970, é amplamente usada em sistemas embarcados, sistemas operacionais e aplicações que requerem alto desempenho.

## Estrutura Básica

```c
#include <stdio.h>  // Inclusão de bibliotecas

// Função principal
int main() {
    // Código do programa
    printf("Olá, Mundo!\n");
    return 0;  // Retorno de status para o sistema operacional
}
```

## Tipos de Dados

### Tipos Primitivos

-   `int`: Números inteiros
-   `float`: Números de ponto flutuante
-   `double`: Números de ponto flutuante de precisão dupla
-   `char`: Caractere único
-   `void`: Tipo sem valor de retorno

### Modificadores

-   `short`
-   `long`
-   `unsigned`
-   `signed`

## Ponteiros

### Conceitos Básicos

-   Ponteiros armazenam endereços de memória
-   Permitem manipulação direta da memória

```c
int x = 10;
int *ptr = &x;  // Ponteiro para x
printf("Valor: %d\n", *ptr);  // Acessa o valor
printf("Endereço: %p\n", (void*)ptr);  // Mostra o endereço
```

### Tipos de Ponteiros

-   Ponteiros para variáveis
-   Ponteiros para funções
-   Ponteiros para estruturas
-   Ponteiros para ponteiros

### Alocação Dinâmica de Memória

```c
int *array = (int*)malloc(5 * sizeof(int));  // Aloca memória
free(array);  // Libera memória
```

## Compilação

### Comandos Básicos

1. Compilação simples:

```bash
gcc programa.c -o programa
```

2. Compilação com warnings:

```bash
gcc -Wall -Wextra programa.c -o programa
```

3. Compilação para depuração:

```bash
gcc -g programa.c -o programa
```

### Níveis de Otimização

-   `-O0`: Sem otimização
-   `-O1`: Otimização básica
-   `-O2`: Otimização intermediária
-   `-O3`: Otimização máxima

## Gerenciamento de Memória

### Alocação

-   `malloc()`: Aloca memória
-   `calloc()`: Aloca e inicializa com zero
-   `realloc()`: Redimensiona memória alocada
-   `free()`: Libera memória alocada

## Estruturas e Tipos Personalizados

```c
struct Pessoa {
    char nome[50];
    int idade;
    float altura;
};

typedef struct Pessoa Pessoa;  // Cria um alias
```

## Entrada e Saída

### Funções Padrão

-   `printf()`: Saída formatada
-   `scanf()`: Entrada formatada
-   `puts()`: Imprime string
-   `gets()`: Entrada de string (descontinuado)

```c
int idade;
printf("Digite sua idade: ");
scanf("%d", &idade);
```

## Bibliotecas Padrão

-   `<stdio.h>`: Entrada/Saída
-   `<stdlib.h>`: Funções utilitárias
-   `<string.h>`: Manipulação de strings
-   `<math.h>`: Funções matemáticas
-   `<time.h>`: Manipulação de tempo

## Boas Práticas

1. Sempre libere memória alocada dinamicamente
2. Verifique retornos de funções
3. Use constantes para valores fixos
4. Minimize o escopo das variáveis
5. Documente seu código
6. Trate possíveis erros de entrada

### Exemplo de Código Seguro

```c
#include <stdio.h>
#include <stdlib.h>

#define MAX_NOME 50

int main() {
    char nome[MAX_NOME];

    printf("Digite seu nome: ");
    if (fgets(nome, sizeof(nome), stdin) != NULL) {
        printf("Olá, %s", nome);
    }

    return 0;
}
```

## Recursos Adicionais

-   [GNU Compiler Collection (GCC)](https://gcc.gnu.org/)
-   [CProgramming.com](https://www.cprogramming.com/)
-   [GeeksforGeeks C Programming](https://www.geeksforgeeks.org/c-programming-language/)

**Nota:** Este guia é uma introdução. C é uma linguagem complexa que requer estudo aprofundado.
