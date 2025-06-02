#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // ID do processo (0 ou 1)
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Quantidade de processos

    if (size != 2)
    {
        if (rank == 0)
            printf("Este programa precisa de exatamente 2 processos!\n");
        MPI_Finalize();
        return 1;
    }

    int tamanhos[] = {8, 64, 512, 1024, 8192, 65536, 1048576}; // em bytes (até 1MB)
    int num_tamanhos = sizeof(tamanhos) / sizeof(int);
    int repeticoes = 1000;

    for (int t = 0; t < num_tamanhos; t++)
    {
        int tamanho = tamanhos[t];
        char *buffer = malloc(tamanho);

        if (rank == 0)
        {
            memset(buffer, 'A', tamanho); // preencher a mensagem com algo
            double start = MPI_Wtime();

            for (int i = 0; i < repeticoes; i++)
            {
                MPI_Send(buffer, tamanho, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(buffer, tamanho, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            double end = MPI_Wtime();
            printf("Tamanho: %7d bytes | Tempo total: %.6f s | Média: %.6f ms\n",
                   tamanho, end - start, 1000 * (end - start) / repeticoes);
        }
        else if (rank == 1)
        {
            for (int i = 0; i < repeticoes; i++)
            {
                MPI_Recv(buffer, tamanho, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(buffer, tamanho, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

        free(buffer);
    }

    MPI_Finalize();
    return 0;
}

// mpicc -o pingpong pingpong.c
// mpirun -np 2 ./pingpong
