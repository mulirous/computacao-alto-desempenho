#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

#define NSTEPS 50 // Tamanho fixo de passos
#define DX 1.0
#define DY 1.0
#define DT 0.1
#define VISC 0.1

typedef struct
{
    int problem_size;
    double T1;
} T1Record;

#define MAX_SIZES 7
T1Record T1_table[MAX_SIZES];
int T1_count = 0;

double **alloc_matrix(int N)
{
    double **mat = (double **)malloc(N * sizeof(double *));
    for (int i = 0; i < N; i++)
    {
        mat[i] = (double *)calloc(N, sizeof(double));
    }
    return mat;
}

void free_matrix(double **mat, int N)
{
    for (int i = 0; i < N; i++)
    {
        free(mat[i]);
    }
    free(mat);
}

double get_T1_for_problem_size(int size)
{
    for (int i = 0; i < T1_count; i++)
    {
        if (T1_table[i].problem_size == size)
            return T1_table[i].T1;
    }
    return -1.0;
}

void store_T1(int size, double T1)
{
    T1_table[T1_count].problem_size = size;
    T1_table[T1_count].T1 = T1;
    T1_count++;
}

void navierStokes(int cores_num, int problem_size)
{
    double **u = alloc_matrix(problem_size);
    double **v = alloc_matrix(problem_size);
    double **u_new = alloc_matrix(problem_size);
    double **v_new = alloc_matrix(problem_size);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    omp_set_num_threads(cores_num);

    int N = problem_size;
    int cx = N / 2;
    int cy = N / 2;
    u[cx][cy] = 1000.0;

    for (int step_num = 0; step_num <= NSTEPS; step_num++)
    {
#pragma omp parallel for schedule(guided)
        for (int i = 1; i < N - 1; i++)
        {
#pragma omp simd
            for (int j = 1; j < N - 1; j++)
            {
                u_new[i][j] = u[i][j] + VISC * DT * ((u[i + 1][j] - 2 * u[i][j] + u[i - 1][j]) / (DX * DX) + (u[i][j + 1] - 2 * u[i][j] + u[i][j - 1]) / (DY * DY));
                v_new[i][j] = v[i][j] + VISC * DT * ((v[i + 1][j] - 2 * v[i][j] + v[i - 1][j]) / (DX * DX) + (v[i][j + 1] - 2 * v[i][j] + v[i][j - 1]) / (DY * DY));
            }
        }

#pragma omp parallel for collapse(2) schedule(guided)
        for (int i = 1; i < N - 1; i++)
        {
            for (int j = 1; j < N - 1; j++)
            {
                u[i][j] = u_new[i][j];
                v[i][j] = v_new[i][j];
            }
        }

        for (int i = 0; i < N; i++)
        {
            u[i][0] = u[i][N - 1] = 0.0;
            v[i][0] = v[i][N - 1] = 0.0;
        }
        for (int j = 0; j < N; j++)
        {
            u[0][j] = u[N - 1][j] = 0.0;
            v[0][j] = v[N - 1][j] = 0.0;
        }
    }

    gettimeofday(&end, NULL);
    double Tp = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    if (cores_num == 1) // Verifica se o código é sequencial para armazenar o tempo do código sequencial por tamanho do problema
    {
        store_T1(problem_size, Tp);
    }

    double T1 = get_T1_for_problem_size(problem_size); // Pega o tempo sequencial com base no tamanho do problema para fazer o cálculo da eficiência
    double speedup = (T1 > 0) ? T1 / Tp : 1.0;         // Calculo SpeedUp
    double efficiency = speedup / cores_num;           // Calculo Eficiência

    printf("\n\n\nProblema: %d | Cores: %d\n", problem_size, cores_num);
    printf("Tempo: %.3f s | Speedup: %.2f | Eficiência: %.2f%%\n\n\n", Tp, speedup, efficiency * 100);

    free_matrix(u, N);
    free_matrix(v, N);
    free_matrix(u_new, N);
    free_matrix(v_new, N);
}

int main()
{
    struct timeval start, end;

    int core_nums[] = {1, 2, 4, 8, 16, 32, 64, 128};
    int problem_sizes[] = {16, 32, 64, 128, 256, 512, 1024};

    gettimeofday(&start, NULL);
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            navierStokes(core_nums[i], problem_sizes[j]);
        }
    }

    gettimeofday(&end, NULL);
    // Tempo total em segundos
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("\n\n\nVerificação de escalabilidade fianlizada em %.3f segundos.\n\n\n", elapsed);

    return 0;
}