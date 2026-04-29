/*
 * @file benchmark_mat_inv.c
 * @brief Benchmark mat_inv across matrix sizes with correctness validation.
 */

#include "linal.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPEATS_SMALL 10 /* For small matrices */
#define REPEATS_LARGE 3  /* For large matrices — O(n^3) is expensive */
#define EPSILON 1e-4     /* Relaxed tolerance for inversion error */

static const size_t sizes[] = {4, 8, 16, 32, 64, 128, 256};
static const int n_sizes = sizeof(sizes) / sizeof(sizes[0]);

/* Fill matrix with deterministic pseudo-random values */
static void
fill_matrix(Matrix *m, uint64_t seed)
{
    double *d = m->data;
    uint64_t state = seed;
    for (size_t i = 0; i < m->rows * m->cols; i++) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        d[i] = (double)((state >> 11) & 0x3FFFFF) / (double)(1 << 21) - 1.0;
    }
}

/* Make matrix diagonally dominant to guarantee non-singularity */
static void
make_non_singular(Matrix *m, double diag_scale)
{
    size_t n = m->rows;
    for (size_t i = 0; i < n; i++) {
        double sum = 0.0;
        for (size_t j = 0; j < n; j++) {
            if (j != i) {
                sum += fabs(m->data[i * n + j]);
            }
        }
        m->data[i * n + i] = diag_scale + sum;
    }
}

static double
clock_ms(struct timespec *start, struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) * 1000.0
           + (end->tv_nsec - start->tv_nsec) / 1e6;
}

/* Validate: A * inv(A) should be close to identity */
static bool
validate_inverse(const Matrix *a, const Matrix *inv_a)
{
    size_t n = a->rows;
    Matrix product = mat_create(n, n);
    if (!product.data) return false;

    mat_mul(*a, *inv_a, &product);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            if (fabs(product.data[i * n + j] - expected) > EPSILON) {
                mat_free(&product);
                return false;
            }
        }
    }

    mat_free(&product);
    return true;
}

int
main(void)
{
    bool all_valid = true;
    int fail_count = 0;
    double primary_ms = 0.0; /* ms for largest matrix */

    printf("=== mat_inv benchmark ===\n");
    printf("%-8s %12s %10s\n", "Size", "Time(ms)", "Valid");

    for (int s = 0; s < n_sizes; s++) {
        size_t n = sizes[s];
        int repeats = (n <= 32) ? REPEATS_SMALL : REPEATS_LARGE;

        Matrix a = mat_create(n, n);
        Matrix inv = mat_create(n, n);

        if (!a.data || !inv.data) {
            printf("FAIL allocation %zu\n", n);
            fail_count++;
            all_valid = false;
            continue;
        }

        fill_matrix(&a, 42 + s * 7);
        make_non_singular(&a, 5.0); /* Strong diagonal dominance */

        /* Correctness check */
        int rc = mat_inv(a, &inv);
        if (rc != 0 || !validate_inverse(&a, &inv)) {
            printf("FAIL %zu x %zu: validation error\n", n, n);
            fail_count++;
            all_valid = false;
            continue;
        }

        /* Warmup */
        mat_inv(a, &inv);

        /* Benchmark */
        struct timespec t_start, t_end;
        double total_ms = 0.0;

        clock_gettime(CLOCK_MONOTONIC, &t_start);
        for (int r = 0; r < repeats; r++) {
            mat_inv(a, &inv);
        }
        clock_gettime(CLOCK_MONOTONIC, &t_end);

        total_ms = clock_ms(&t_start, &t_end);
        double avg_ms = total_ms / (double)repeats;

        /* Store primary metric from largest matrix */
        if (s == n_sizes - 1) {
            primary_ms = avg_ms;
        }

        printf("%4zu×%4zu %12.3f %5s\n", n, n, avg_ms, "OK");

        mat_free(&a);
        mat_free(&inv);
    }

    printf("\n=== Correctness: %s ===\n", all_valid ? "PASS" : "FAIL");

    /* Output structured METRIC lines for autoresearch parsing. */
    if (all_valid) {
        printf("METRIC mat_inv_ms=%.6f\n", primary_ms);
    } else {
        printf("METRIC mat_inv_ms=999999.000000\n");
    }

    return EXIT_SUCCESS;
}
