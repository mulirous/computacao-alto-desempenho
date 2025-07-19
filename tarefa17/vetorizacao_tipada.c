#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void run_test(int M, int N, int rank, int size)
{
    int colunas_por_processo = N / size;

    // Criação do tipo derivado para uma coluna
    MPI_Datatype coltype, coltype_resized;
    MPI_Type_vector(M, 1, N, MPI_DOUBLE, &coltype);
    // redimensiona o tipo
    MPI_Type_create_resized(coltype, 0, sizeof(double), &coltype_resized);
    // salva o tipo
    MPI_Type_commit(&coltype_resized);

    double *A = NULL;
    double *x = malloc(N * sizeof(double));
    double *local_A = malloc(M * colunas_por_processo * sizeof(double));
    double *local_x = malloc(colunas_por_processo * sizeof(double));
    double *y_partial = calloc(M, sizeof(double));
    double *y = NULL;

    if (rank == 0)
    {
        A = malloc(M * N * sizeof(double));
        y = malloc(M * sizeof(double));
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
                A[i * N + j] = 1.0;

        for (int i = 0; i < N; i++)
            x[i] = 2.0;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double start = MPI_Wtime();

    // Cada processo pega só seu pedaço de x
    MPI_Scatter(x, colunas_por_processo, MPI_DOUBLE,
                local_x, colunas_por_processo, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    // Scatter das colunas da matriz A
    MPI_Scatter(A, colunas_por_processo, coltype_resized,
                local_A, M * colunas_por_processo, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    // Cada processo calcula sua contribuição parcial para todo y
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < colunas_por_processo; j++)
        {
            y_partial[i] += local_A[i * colunas_por_processo + j] * local_x[j];
        }
    }

    // Redução das contribuições parciais para o processo 0
    MPI_Reduce(y_partial, y, M, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0)
    {
        double soma = 0;
        for (int i = 0; i < M; i++)
        {
            soma += y[i];
        }

        printf("M=%4d N=%4d Tempo=%.6f s | Soma total = %.0f\n", M, N, end - start, soma);
        free(A);
        free(y);
    }

    free(x);
    free(local_x);
    free(local_A);
    free(y_partial);

    MPI_Type_free(&coltype_resized);
    MPI_Type_free(&coltype);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int Ns[] = {2, 4, 8, 16, 32, 64, 128, 256, 512};
    for (int i = 0; i < sizeof(Ns) / sizeof(Ns[0]); i++)
    {
        int N = Ns[i];
        int M = N;

        if (M % size == 0)
            run_test(M, N, rank, size);
        else if (rank == 0)
            printf("Pulando M=%d N=%d pois não é divisível por número de processos\n", M, N);

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
