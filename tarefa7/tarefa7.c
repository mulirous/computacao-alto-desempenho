#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

typedef struct Node
{
    char *text;
    struct Node *proximo;
} Node;

Node *inserirInicio(Node *head, const char *text)
{
    Node *novo = (Node *)malloc(sizeof(Node));

    // copia a text para dentro do nó
    novo->text = (char *)malloc(strlen(text) + 1);
    strcpy(novo->text, text);

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
        free(temp->text); // libera a text
        free(temp);       // libera o nó
    }
}

int main()
{

    for (int i = 0; i < 10; i++)
    {

        Node *listaPlanetas = NULL;

        listaPlanetas = inserirInicio(listaPlanetas, "Mercurio");
        listaPlanetas = inserirInicio(listaPlanetas, "Venus");
        listaPlanetas = inserirInicio(listaPlanetas, "Terra");
        listaPlanetas = inserirInicio(listaPlanetas, "Marte");
        listaPlanetas = inserirInicio(listaPlanetas, "Jupiter");
        listaPlanetas = inserirInicio(listaPlanetas, "Saturno");
        listaPlanetas = inserirInicio(listaPlanetas, "Urano");
        listaPlanetas = inserirInicio(listaPlanetas, "Netuno");
        listaPlanetas = inserirInicio(listaPlanetas, "Plutao");

        printf("\n\n========================================================Tentativa %d========================================================\n\n", i + 1);

#pragma omp parallel
        {
#pragma omp single // Faz com que apenas uma thread percorra a lista por vez, ou seja, uma tarefa é criada por vez
            {
                Node *atual = listaPlanetas; // Faz uma Cópia do primeiro ponteiro da lista de planetas, para perservar a lista
                while (atual != NULL)
                {
                    Node *n = atual; // pega o nó atual dentro do loop
#pragma omp task                     // firstprivate(n)     // Faz com que cada thread tenha uma cópia de atual já inicializado de maneira privada, e aí transforma o processamento (o print) em uma tarefa
                    {
                        int thread_id = omp_get_thread_num();
                        printf("%s -> processada pela thread %d\n", n->text, thread_id);
                    }
                    atual = atual->proximo;
                }

#pragma omp taskwait // Aguarda todas as tarefas serem executadas antes de liberar a lista e acabar a execução do programa
                printf("NULL\n");
            }
        }
        liberarLista(listaPlanetas);
    }
}