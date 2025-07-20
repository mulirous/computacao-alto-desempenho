#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>

#define NSTEPS 50
#define VISC 0.1
#define DT 0.1
#define DX 1.0
#define DY 1.0

__global__ void navier_cuda(double *u_new, double *u, int N) {
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    if (i > 0 && i < N-1 && j > 0 && j < N-1) {
        int idx = i * N + j;
        u_new[idx] = u[idx] + VISC * DT * (
            (u[(i+1)*N + j] - 2 * u[idx] + u[(i-1)*N + j]) / (DX*DX) +
            (u[i*N + (j+1)] - 2 * u[idx] + u[i*N + (j-1)]) / (DY*DY)
        );
    }
}

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Uso: %s <tamanho_malha>\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);

    
    size_t size = N * N * sizeof(double);

    double *h_u = (double *)calloc(N * N, sizeof(double));
    double *h_u_new = (double *)calloc(N * N, sizeof(double));
    h_u[(N / 2) * N + (N / 2)] = 1000.0;

    double *d_u, *d_u_new;
    cudaMalloc(&d_u, size);
    cudaMalloc(&d_u_new, size);
    cudaMemcpy(d_u, h_u, size, cudaMemcpyHostToDevice);

    dim3 block(16, 16);
    dim3 grid((N + 15) / 16, (N + 15) / 16);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    for (int t = 0; t < NSTEPS; t++)
    {
        navier_cuda<<<grid, block>>>(d_u_new, d_u, N);
        double *tmp = d_u;
        d_u = d_u_new;
        d_u_new = tmp;
    }
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float ms = 0;
    cudaEventElapsedTime(&ms, start, stop);

    // Log SLURM-friendly:
    printf("Tamanho: %d x %d | Tempo GPU: %.4f ms\n", N, N, ms);

    cudaMemcpy(h_u, d_u, size, cudaMemcpyDeviceToHost);

    // (Opcional) imprimir soma ou valor central para comparação
    printf("u[%d][%d] = %f\n", N/2, N/2, h_u[(N/2)*N + (N/2)]);

    free(h_u); free(h_u_new);
    cudaFree(d_u); cudaFree(d_u_new);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return 0;
}
