#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 100  // Total de células na barra
#define T 1000 // Iterações no tempo
#define ALPHA 0.1

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_n = N / size + 2; // +2 para células fantasmas
    double *atual = malloc(local_n * sizeof(double));
    double *anterior = malloc(local_n * sizeof(double));

    // Inicializa
    memset(atual, 0, local_n * sizeof(double));
    memset(anterior, 0, local_n * sizeof(double));

    if (rank == 0)
        for (int i = 1; i < local_n - 1; i++)
            anterior[i] = 100.0; // Calor no início da barra

    double start = MPI_WTime();

    for (int t = 0; t < T; t++)
    {
        // Mesma ideia, mas computa interior enquanto espera bordas

        MPI_Request reqs[4];
        int flag;

        if (rank > 0)
            MPI_Isend(&anterior[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[0]);
        if (rank < size - 1)
            MPI_Irecv(&anterior[local_n - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[1]);

        if (rank < size - 1)
            MPI_Isend(&anterior[local_n - 2], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &reqs[2]);
        if (rank > 0)
            MPI_Irecv(&anterior[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &reqs[3]);

        // Cálculo dos pontos internos (sem depender de bordas)
        for (int i = 2; i < local_n - 2; i++)
        {
            atual[i] = anterior[i] + ALPHA * (anterior[i - 1] - 2 * anterior[i] + anterior[i + 1]);
        }

        // Verifica se as comunicações de borda terminaram
        if (rank > 0)
        {
            MPI_Test(&reqs[3], &flag, MPI_STATUS_IGNORE); // esquerda
            atual[1] = anterior[1] + ALPHA * (anterior[0] - 2 * anterior[1] + anterior[2]);
        }
        if (rank < size - 1)
        {
            MPI_Test(&reqs[1], &flag, MPI_STATUS_IGNORE); // direita
            atual[local_n - 2] = anterior[local_n - 2] + ALPHA * (anterior[local_n - 3] - 2 * anterior[local_n - 2] + anterior[local_n - 1]);
        }

        double *tmp = anterior;
        anterior = atual;
        atual = tmp;
    }

    double end = MPI_WTime();

    double tempo = (end - start) / 1000;

    printf("O processo durou cerca de %.3f segundos", tempo);

    MPI_Finalize();
    return 0;
}
