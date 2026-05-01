/*
 * Standalone benchmark for mat_inv. Times multiple matrix sizes,
 * reports median of N iterations per size, validates correctness.
 */
#include "linal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N_ITER 20
#define EPSILON 1e-4

static const size_t SIZES[] = {64, 128, 256};
static const int N_SIZES = sizeof(SIZES) / sizeof(SIZES[0]);

/* Deterministic pseudo-random fill */
static void fill_matrix(Matrix *m, uint64_t seed)
{
    double *d = m->data;
    uint64_t state = seed;
    for (size_t i = 0; i < m->rows * m->cols; i++) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        d[i] = (double)((state >> 11) & 0x3FFFFF) / (double)(1 << 21) - 1.0;
    }
}

/* Make diagonally dominant */
static void make_non_singular(Matrix *m, double diag_scale)
{
    size_t n = m->rows;
    for (size_t i = 0; i < n; i++) {
        double sum = 0.0;
        for (size_t j = 0; j < n; j++) {
            if (j != i) sum += fabs(m->data[i * n + j]);
        }
        m->data[i * n + i] = diag_scale + sum;
    }
}

static double clock_us(struct timespec *s, struct timespec *e)
{
    return (e->tv_sec - s->tv_sec) * 1e6 + (e->tv_nsec - s->tv_nsec) / 1e3;
}

/* Validate A * inv(A) ≈ I */
static int validate(const Matrix *a, const Matrix *inv_a)
{
    size_t n = a->rows;
    Matrix prod = mat_create(n, n);
    if (!prod.data) return 0;
    mat_mul(*a, *inv_a, &prod);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            if (fabs(prod.data[i * n + j] - expected) > EPSILON) {
                mat_free(&prod);
                return 0;
            }
        }
    }
    mat_free(&prod);
    return 1;
}

int main(void)
{
    bool all_ok = true;

    for (int s = 0; s < N_SIZES; s++) {
        size_t n = SIZES[s];
        Matrix a = mat_create(n, n);
        Matrix inv = mat_create(n, n);
        fill_matrix(&a, 42 + s * 7);
        make_non_singular(&a, 5.0);

        /* Correctness check */
        if (mat_inv(a, &inv) != 0 || !validate(&a, &inv)) {
            fprintf(stderr, "VALIDATION FAILED %zux%zu\n", n, n);
            all_ok = false;
        }

        /* Collect timing samples */
        double times[N_ITER];
        for (int r = 0; r < N_ITER; r++) {
            struct timespec t0, t1;
            clock_gettime(CLOCK_MONOTONIC, &t0);
            mat_inv(a, &inv);
            clock_gettime(CLOCK_MONOTONIC, &t1);
            times[r] = clock_us(&t0, &t1) / 1e3; /* ms */
        }

        /* Sort for median */
        double sorted[N_ITER];
        memcpy(sorted, times, sizeof(times));
        for (int i = 0; i < N_ITER - 1; i++)
            for (int j = i + 1; j < N_ITER; j++)
                if (sorted[j] < sorted[i]) {
                    double t = sorted[i];
                    sorted[i] = sorted[j];
                    sorted[j] = t;
                }
        double median = sorted[N_ITER / 2];

        printf("%zux%zu median=%.6f ms\n", n, n, median);
        printf("METRIC mat_inv_%zu_ms=%.6f\n", n, median);

        if (s == N_SIZES - 1)
            printf("METRIC mat_inv_ms=%.6f\n", median); /* primary = largest */

        mat_free(&a);
        mat_free(&inv);
    }

    return all_ok ? 0 : 1;
}
