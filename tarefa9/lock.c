#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 1000

typedef struct Node
{
    int valor;
    struct Node *proximo;
} Node;

Node *inserir(Node *head, int valor)
{
    Node *novo = (Node *)malloc(sizeof(Node));
    novo->valor = valor;
    novo->proximo = head;
    return novo;
}

void liberarLista(Node *head)
{
    Node *atual = head;
    while (atual)
    {
        Node *temp = atual;
        atual = atual->proximo;
        free(temp);
    }
}

void imprimirLista(Node *head, int id)
{
    printf("Lista %d: ", id);
    Node *atual = head;
    while (atual)
    {
        printf("%d -> ", atual->valor);
        atual = atual->proximo;
    }
    printf("NULL\n");
}

int main()
{
    int K;

    printf("Digite o número de listas: ");
    scanf("%d", &K);

    Node **listas = (Node **)malloc(K * sizeof(Node *));
    omp_lock_t *locks = (omp_lock_t *)malloc(K * sizeof(omp_lock_t)); // Alocando um vetor de locks para fechar de acordo com a lista correspondente

    for (int i = 0; i < K; i++)
    {
        listas[i] = NULL;         // Inicializa as listas conforme o número dado pelo usuário
        omp_init_lock(&locks[i]); // Inicializa as variáveis de lock dentro do array de locks conforme o número dado pelo usuário
    }

    omp_set_num_threads(4); // Faz o set do número de threads
    srand(time(NULL));      // Inicializa a seed

#pragma omp parallel // Paraleliza a região do bloco
    {
#pragma omp single // Faz com que apenas uma thread possa entrar por vez na região imediatamente abaixo, enquanto as outras threads esperam
        {
            for (int i = 0; i < N; i++)
            {
#pragma omp task // Transforma em uma tarefa para que seja executada assincronamente pelas outras threads
                {
                    int valor = rand() % 1000;
                    int choice = rand() % K;

                    omp_set_lock(&locks[choice]);                    // Seta o lock explicitamente, com o lock correspondente ao número da lista, para que nenhuma outra thread possa entrar e acabar modificando a posição da lista simultaneamente, ocorrendo concorrência
                    listas[choice] = inserir(listas[choice], valor); // Modifica o valor da lista, e se outra thread já estiver dentro (modificando a mesma lista), essa thread fica bloqueada até que o lock seja liberado -> Garante Exclusão Mútua
                    omp_unset_lock(&locks[choice]);                  // Retira o lock explicito, e aí libera a thread que estava esperando o mesmo lock entrar e utilizar o lock para fechar a operação
                }
            }
        }
#pragma omp taskwait // Aguarda todas as tasks serem executadas
    }

    for (int i = 0; i < K; i++) // Impressão da lista e liberação dela da memória
    {
        imprimirLista(listas[i], i + 1);
        liberarLista(listas[i]);
        omp_destroy_lock(&locks[i]); // Remoção do lock dentro do vetor de locks
    }

    free(listas); // Free da matriz
    free(locks);  // Free do array de locks

    return 0;
}
