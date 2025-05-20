#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define NX 2048
#define NY 2048
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

    // Inicialização com tudo zero
    for (int i = 0; i < NX; i++)
        for (int j = 0; j < NY; j++)
            u[i][j] = v[i][j] = 0.0;

    // Perturbação no centro
    int cx = NX / 2;
    int cy = NY / 2;
    u[cx][cy] = 1000.0;

    // Condições de contorno (velocidade zero nas bordas)
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

    for (int step_num = 0; step_num <= NSTEPS; step_num++)
    {
        // Salvar campo para visualização
        if (step_num % 50 == 0)
        {
            printf("Salvando passo %d\n", step_num);
            char filename[64];
            sprintf(filename, "vel_%04d.dat", step_num);
            FILE *f = fopen(filename, "w");
            for (int j = 0; j < NY; j++)
            {
                for (int i = 0; i < NX; i++)
                {
                    fprintf(f, "%d %d %f %f\n", i, j, u[i][j], v[i][j]);
                }
                fprintf(f, "\n");
            }
            fclose(f);
        }

        // Atualizar velocidades com difusão
        for (int i = 1; i < NX - 1; i++)
        {
            for (int j = 1; j < NY - 1; j++)
            {
                u_new[i][j] = u[i][j] + VISC * DT * ((u[i + 1][j] - 2 * u[i][j] + u[i - 1][j]) / (DX * DX) + (u[i][j + 1] - 2 * u[i][j] + u[i][j - 1]) / (DY * DY));
                v_new[i][j] = v[i][j] + VISC * DT * ((v[i + 1][j] - 2 * v[i][j] + v[i - 1][j]) / (DX * DX) + (v[i][j + 1] - 2 * v[i][j] + v[i][j - 1]) / (DY * DY));
            }
        }

        // Copiar os novos valores para os vetores antigos
        for (int i = 1; i < NX - 1; i++)
        {
            for (int j = 1; j < NY - 1; j++)
            {
                u[i][j] = u_new[i][j];
                v[i][j] = v_new[i][j];
            }
        }

        // Aplicar novamente condições de contorno
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
