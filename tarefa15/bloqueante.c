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

    int local_n = N / size + 2; // +2 para células fantasmas (bordas)
    double *current = malloc(local_n * sizeof(double));
    double *previous = malloc(local_n * sizeof(double));

    memset(current, 0, local_n * sizeof(double));
    memset(previous, 0, local_n * sizeof(double));

    // Calor inicial só no processo 0
    if (rank == 0)
        for (int i = 1; i < local_n - 1; i++)
            previous[i] = 100.0;

    MPI_Barrier(MPI_COMM_WORLD); // Garante que todos comecem juntos
    double start = MPI_Wtime();

    for (int t = 0; t < T; t++)
    {
        // Troca de fronteiras com os vizinhos (bloqueante)
        if (rank > 0)
            MPI_Send(&previous[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
        if (rank < size - 1)
            MPI_Recv(&previous[local_n - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (rank < size - 1)
            MPI_Send(&previous[local_n - 2], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
        if (rank > 0)
            MPI_Recv(&previous[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Cálculo de difusão
        for (int i = 1; i < local_n - 1; i++)
        {
            current[i] = previous[i] + ALPHA * (previous[i - 1] - 2 * previous[i] + previous[i + 1]);
        }

        // Log a cada 100 iterações
        if (t % 100 == 0)
        {
            printf("Rank %d - Iteração %d - Temp. centro local: %.2f\n", rank, t, previous[local_n / 2]);
        }

        // Troca dos vetores
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
