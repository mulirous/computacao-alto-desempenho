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

    double start = MPI_WTime();

    for (int t = 0; t < T; t++)
    {
        // Troca com vizinhos (bloqueante)
        if (rank > 0)
            MPI_Send(&previous[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
        if (rank < size - 1)
            MPI_Recv(&previous[local_n - 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (rank < size - 1)
            MPI_Send(&previous[local_n - 2], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
        if (rank > 0)
            MPI_Recv(&previous[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 1; i < local_n - 1; i++)
        {
            current[i] = previous[i] + ALPHA * (previous[i - 1] - 2 * previous[i] + previous[i + 1]);
        }

        double *tmp = previous;
        previous = current;
        current = tmp;
    }

    double end = MPI_WTime();

    double tempo = (end - start) / 1000;

    printf("O processo durou cerca de %.3f segundos", tempo);

    MPI_Finalize();
    return 0;
}
