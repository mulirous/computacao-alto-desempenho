#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

#define N 100

typedef struct Node
{
    int num;
    struct Node *proximo;
} Node;

Node *inserir(Node *head, const int *num)
{
    Node *novo = (Node *)malloc(sizeof(Node));

    // copia a num para dentro do nó
    novo->num = num;
    novo->proximo = head;

    return novo;
}

void liberarLista(Node *head)
{
    Node *atual = head;
    while (atual != NULL)
    {
        Node *temp = atual;
        atual = atual->proximo;
        free(temp); // libera o nó
    }
}

void imprimirLista(Node *head, int id)
{
    printf("\n\nLista %d:\n\n ", id);
    Node *atual = head;
    while (atual)
    {
        printf("%d -> ", atual->num);
        atual = atual->proximo;
    }
    printf("NULL\n");
}

int main()
{

    int K; // Variáveis que rerpresenta a quantidade de listas

    printf("\nDigite o número de listas: ");
    scanf("%d", &K);

    // Alocação das K listas
    Node **listas = (Node **)malloc(K * sizeof(Node *));

    srand(time(NULL));

    omp_set_num_threads(10);

#pragma omp parallel
    {
#pragma omp single // Faz com que apenas uma thread percorra a lista por vez, ou seja, uma tarefa é criada por vez
        {
            for (int i = 0; i < N; i++)
            {
#pragma omp task // Transforma em uma tarefa para que seja executada assincronamente pelas outras threads
                {
                    int value = rand() % 1000;
                    int choice = rand() % K; // 0 ou 1

#pragma omp critical(listas[chioce]) // Neste caso não vai ser possível utilizar a região crítica nomeada, pois a região crítica não é conhecida em tempo de compilação (não é fixa), pois ela é uma região dinâmica que é informada pelo usuário
                    {
                        listas[choice] = inserir(listas[choice], value); // Modificação da lista X
                    }
                }
            }
        }
#pragma omp taskwait
    }

    for (int i = 0; i < K; i++)
    {
        imprimirLista(listas[i], i); // Impressão das listass
    }
}
