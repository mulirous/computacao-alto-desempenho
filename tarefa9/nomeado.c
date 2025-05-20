#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

#define N 1000

typedef struct Node
{
    int num;
    struct Node *proximo;
} Node;

Node *inserir(Node *head, const int *num)
{
    Node *novo = (Node *)malloc(sizeof(Node));

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

    Node *lista1 = NULL;
    Node *lista2 = NULL;

    srand(time(NULL));

    omp_set_num_threads(2);

#pragma omp parallel
    {
#pragma omp single // Faz com que apenas uma thread percorra a lista por vez, ou seja, uma tarefa é criada por vez
        {
            for (int i = 0; i < N; i++)
            {
#pragma omp task // Transforma em uma tarefa para que seja executada assincronamente pelas outras threads
                {
                    int value = rand() % 1000;
                    int choice = rand() % 2; // 0 ou 1

                    if (!choice)
                    {
#pragma omp critical(lista1) // Protege a região critica da lista 1, ou seja, apenas uma thread por vez pode atualizar a lista 1, contornando o caso de raco condition a atualizar ela
                        {
                            lista1 = inserir(lista1, value);
                        }
                    }
                    else
                    {
#pragma omp critical(lista2) // Protege a região critica da lista 2, ou seja, apenas uma thread por vez pode atualizar a lista 2, contornando o caso de raco condition a atualizar ela
                        {
                            lista2 = inserir(lista2, value);
                        }
                    }
                }
            }
        }
#pragma omp taskwait // Aguarda todas as tarefas serem executadas, previnindo de uma lista ser liberada antes de algaum tarefa vinculada a ela ser executada, gerando erros não determinísticos
    }

    // Impressão das lsitas
    imprimirLista(lista1, 0);
    imprimirLista(lista2, 1);

    liberarLista(lista1);
    liberarLista(lista2);
}
