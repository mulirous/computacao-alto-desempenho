#include <stdio.h>
#include <omp.h>

#define N 1000000
#define TOL 1.0e-6

int main()
{
    float a[N], b[N], c[N], res[N];
    int err = 0;
// fill the arrays
#pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        a[i] = (float)i;
        b[i] = 2.0 * (float)i;
        c[i] = 0.0;
        res[i] = i + 2 * i;
    }

    double t1 = omp_get_wtime();

// add two vectors
#pragma omp target
#pragma omp loop
    for (int i = 0; i < N; i++)
    {
        c[i] = a[i] + b[i];
    }

    double t2 = omp_get_wtime();

    printf("\n\nCPU time: %f\n\n", t1 - t2);
// test results
#pragma omp parallel for reduction(+ : err)
    for (int i = 0; i < N; i++)
    {
        float val = c[i] - res[i];
        val = val * val;
        if (val > TOL)
            err++;
    }
    printf("vectors added with %d errors\n", err);
    return 0;
}