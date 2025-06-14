#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void run_test(int M, int N, int rank, int size)
{
    int linhas_por_proc = M / size;
    double *A = NULL;
    double *x = malloc(N * sizeof(double));
    double *local_A = malloc(linhas_por_proc * N * sizeof(double));
    double *local_y = malloc(linhas_por_proc * sizeof(double));
    double *y = NULL;

    if (rank == 0)
    {
        A = malloc(M * N * sizeof(double));
        y = malloc(M * sizeof(double));
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
                A[i * N + j] = 1.0; // ou rand()/100.0;

        for (int i = 0; i < N; i++)
            x[i] = 2.0;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Início do cronômetro
    double start = MPI_Wtime();

    // Broadcast do vetor x
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Scatter das linhas da matriz A
    MPI_Scatter(A, linhas_por_proc * N, MPI_DOUBLE,
                local_A, linhas_por_proc * N, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    // Produto local
    for (int i = 0; i < linhas_por_proc; i++)
    {
        local_y[i] = 0.0;
        for (int j = 0; j < N; j++)
        {
            local_y[i] += local_A[i * N + j] * x[j];
        }
    }

    // Gather dos resultados
    MPI_Gather(local_y, linhas_por_proc, MPI_DOUBLE,
               y, linhas_por_proc, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0)
    {
        double soma = 0;
        for (int i = 0; i < M; i++)
        {
            soma += y[i];
        }

        // A soma deve ser igual ao quadrado de N multiplicado por 2
        printf("M=%4d N=%4d Tempo=%.6f s | Soma total = %.0f\n", M, N, end - start, soma);
        free(A);
        free(y);
    }

    free(x);
    free(local_A);
    free(local_y);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Testes para diferentes valores de N (assumindo M = N para simplificar)
    int Ns[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
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
