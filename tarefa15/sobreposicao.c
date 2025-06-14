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
    double *current = malloc(local_n * sizeof(double));
    double *previous = malloc(local_n * sizeof(double));

    // Inicializa
    memset(current, 0, local_n * sizeof(double));
    memset(previous, 0, local_n * sizeof(double));

    if (rank == 0)
        for (int i = 1; i < local_n - 1; i++)
            previous[i] = 100.0; // Calor no início da barra

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    for (int t = 0; t < T; t++)
    {
        // Mesma ideia, mas computa interior enquanto espera bordas

        MPI_Request reqs[4];
        int flag;

        if (rank > 0)
            MPI_Isend(&previous[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[0]);
        if (rank < size - 1)
            MPI_Irecv(&previous[local_n - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[1]);

        if (rank < size - 1)
            MPI_Isend(&previous[local_n - 2], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &reqs[2]);
        if (rank > 0)
            MPI_Irecv(&previous[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &reqs[3]);

        // Cálculo dos pontos internos (sem depender de bordas)
        for (int i = 2; i < local_n - 2; i++)
        {
            current[i] = previous[i] + ALPHA * (previous[i - 1] - 2 * previous[i] + previous[i + 1]);
        }

        // Verifica se as comunicações de borda terminaram
        if (rank > 0)
        {
            MPI_Test(&reqs[3], &flag, MPI_STATUS_IGNORE); // esquerda
            current[1] = previous[1] + ALPHA * (previous[0] - 2 * previous[1] + previous[2]);
        }
        if (rank < size - 1)
        {
            MPI_Test(&reqs[1], &flag, MPI_STATUS_IGNORE); // direita
            current[local_n - 2] = previous[local_n - 2] + ALPHA * (previous[local_n - 3] - 2 * previous[local_n - 2] + previous[local_n - 1]);
        }

        if (t % 100 == 0)
        {
            printf("Rank %d - Iteração %d - Temp. centro local: %.2f\n", rank, t, previous[local_n / 2]);
        }

        double *tmp = previous;
        previous = current;
        current = tmp;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    printf("Rank %d finalizou em %.6f segundos\n", rank, end - start);

    free(current);
    free(previous);

    MPI_Finalize();
    return 0;
}
