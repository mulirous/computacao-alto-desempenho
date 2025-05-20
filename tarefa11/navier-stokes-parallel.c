#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

#define NX 2024
#define NY 2024
#define NSTEPS 500
#define DX 1.0
#define DY 1.0
#define DT 0.1
#define VISC 0.1

double u[NX][NY], v[NX][NY];
double u_new[NX][NY], v_new[NX][NY];

int main()
{

    struct timeval start, end;
    gettimeofday(&start, NULL);

    omp_set_num_threads(8); // Set do número de threads

    // Inicialização com tudo zero -> Operações que não pesam computacionalmente, acabando por não compensar o custo de paralelização
    for (int i = 0; i < NX; i++)
    {
        for (int j = 0; j < NY; j++)
        {
            u[i][j] = v[i][j] = 0.0;
        }
    }

    // Perturbação no centro -> Operações que não pesam computacionalmente, acabando por não compensar o custo de paralelização
    int cx = NX / 2;
    int cy = NY / 2;
    u[cx][cy] = 1000.0;

    // Condições de contorno (velocidade zero nas bordas) -> Operações que não pesam computacionalmente, acabando por não compensar o custo de paralelização
    for (int i = 0; i < NX; i++)
    {
        u[i][0] = u[i][NY - 1] = 0.0;
        v[i][0] = v[i][NY - 1] = 0.0;
    }
    for (int j = 0; j < NY; j++)
    {
        u[0][j] = u[NX - 1][j] = 0.0;
        v[0][j] = v[NX - 1][j] = 0.0;
    }

    // Paralelizar aqui poderia causar condição de corrida
    for (int step_num = 0; step_num <= NSTEPS; step_num++)
    {
        // // Salvar campo para visualização
        // if (step_num % 50 == 0)
        // {
        //     printf("Salvando passo %d\n", step_num);
        //     char filename[64];
        //     sprintf(filename, "vel_parallel_%04d.dat", step_num);
        //     FILE *f = fopen(filename, "w");
        //     for (int j = 0; j < NY; j++)
        //     {
        //         for (int i = 0; i < NX; i++)
        //         {
        //             fprintf(f, "%d %d %f %f\n", i, j, u[i][j], v[i][j]);
        //         }
        //         fprintf(f, "\n");
        //     }
        //     fclose(f);
        // }

// Atualizar velocidades com difusão -> Computacionalmente intenso, acabando por compensar o custo da paralelização
#pragma omp parallel for schedule(guided)
        for (int i = 1; i < NX - 1; i++)
        {
// Memory-Bound -> Acessos repetivtivos á memória em um espaçõ muito grande
#pragma omp simd
            for (int j = 1; j < NY - 1; j++)
            {
                u_new[i][j] = u[i][j] + VISC * DT * ((u[i + 1][j] - 2 * u[i][j] + u[i - 1][j]) / (DX * DX) + (u[i][j + 1] - 2 * u[i][j] + u[i][j - 1]) / (DY * DY));
                v_new[i][j] = v[i][j] + VISC * DT * ((v[i + 1][j] - 2 * v[i][j] + v[i - 1][j]) / (DX * DX) + (v[i][j + 1] - 2 * v[i][j] + v[i][j - 1]) / (DY * DY));
            }
        }

// Copiar os novos valores para os vetores antigos -> Acaba por compensar a paralelização neste caso
#pragma omp parallel for collapse(2) schedule(guided)
        for (int i = 1; i < NX - 1; i++)
        {
            for (int j = 1; j < NY - 1; j++)
            {
                u[i][j] = u_new[i][j];
                v[i][j] = v_new[i][j];
            }
        }

        // Aplicar novamente condições de contorno -> Operações que não pesam computacionalmente, acabando por não compensar o custo de paralelização
        for (int i = 0; i < NX; i++)
        {
            u[i][0] = u[i][NY - 1] = 0.0;
            v[i][0] = v[i][NY - 1] = 0.0;
        }
        for (int j = 0; j < NY; j++)
        {
            u[0][j] = u[NX - 1][j] = 0.0;
            v[0][j] = v[NX - 1][j] = 0.0;
        }
    }

    gettimeofday(&end, NULL);
    // Tempo total em segundos
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Simulação finalizada em %.3f segundos.\n", elapsed);

    return 0;
}
