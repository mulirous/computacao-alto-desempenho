#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>

#define NSTEPS 50
#define VISC 0.1
#define DT 0.1
#define DX 1.0
#define DY 1.0

__global__ void navier_cuda(double *u_new, double *u, int N) {
    // Bloco 16x16: shared memory precisa de 18x18 (com borda)
    __shared__ double s_u[34][34];

    int i = blockIdx.y * blockDim.y + threadIdx.y; // linha global
    int j = blockIdx.x * blockDim.x + threadIdx.x; // coluna global
    int li = threadIdx.y + 1; // linha local no shared memory
    int lj = threadIdx.x + 1; // coluna local no shared memory

    // 1. Carregar os dados para o shared memory
    if (i < N && j < N)
        s_u[li][lj] = u[i*N + j];

    // 2. Carregar as bordas do bloco (se na borda do bloco)
    if (threadIdx.y == 0 && i > 0) // topo
        s_u[0][lj] = u[(i-1)*N + j];
    if (threadIdx.y == blockDim.y-1 && i < N-1) // base
        s_u[blockDim.y+1][lj] = u[(i+1)*N + j];
    if (threadIdx.x == 0 && j > 0) // esquerda
        s_u[li][0] = u[i*N + (j-1)];
    if (threadIdx.x == blockDim.x-1 && j < N-1) // direita
        s_u[li][blockDim.x+1] = u[i*N + (j+1)];

    // preenchimento nas quinas/bordas da matriz da memória compartilhada
    // Superior esquerdo
    if (threadIdx.x==0 && threadIdx.y==0 && i>0 && j>0)
        s_u[0][0] = u[(i-1)*N + (j-1)];
    // Superior direito
    if (threadIdx.x==blockDim.x-1 && threadIdx.y==0 && i>0 && j<N-1)
        s_u[0][blockDim.x+1] = u[(i-1)*N + (j+1)];
    // Inferior esquerdo
    if (threadIdx.x==0 && threadIdx.y==blockDim.y-1 && i<N-1 && j>0)
        s_u[blockDim.y+1][0] = u[(i+1)*N + (j-1)];
    // Inferior direito
    if (threadIdx.x==blockDim.x-1 && threadIdx.y==blockDim.y-1 && i<N-1 && j<N-1)
        s_u[blockDim.y+1][blockDim.x+1] = u[(i+1)*N + (j+1)];

    // 3. Sincronizar todas as threads do bloco
    __syncthreads();

    // 4. Fazer o cálculo (apenas para as threads internas, sem bordas)
    if (i > 0 && i < N-1 && j > 0 && j < N-1) {
        int idx = i * N + j;
        u_new[idx] = s_u[li][lj] + VISC * DT * (
            (s_u[li+1][lj] - 2 * s_u[li][lj] + s_u[li-1][lj]) / (DX*DX) +
            (s_u[li][lj+1] - 2 * s_u[li][lj] + s_u[li][lj-1]) / (DY*DY)
        );
    }
}


int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Uso: %s <tamanho_malha>\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);
    size_t size = N*N*sizeof(double);

    double *h_u = (double*)calloc(N*N, sizeof(double));
    double *h_u_new = (double*)calloc(N*N, sizeof(double));
    h_u[(N/2)*N + (N/2)] = 1000.0;

    double *d_u, *d_u_new;
    cudaMalloc(&d_u, size);
    cudaMalloc(&d_u_new, size);
    cudaMemcpy(d_u, h_u, size, cudaMemcpyHostToDevice);

    dim3 block(32, 32);
    dim3 grid((N + 31) / 32, (N + 31) / 32);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    for (int t=0; t<NSTEPS; t++) {
        navier_cuda<<<grid, block>>>(d_u_new, d_u, N);
        double *tmp = d_u; d_u = d_u_new; d_u_new = tmp;
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
