/*
 * @file benchmark_mat_add.c
 * @brief Benchmark mat_add across matrix sizes with correctness validation.
 */

#include "linal.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPEATS_SMALL 50    /* For small matrices */
#define REPEATS_LARGE 10    /* For large matrices */
#define EPSILON 1e-6

static const size_t sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
static const int n_sizes = sizeof(sizes) / sizeof(sizes[0]);

/* Fill matrix with deterministic pseudo-random values */
static void
fill_matrix(Matrix *m, size_t seed)
{
    double *d = m->data;
    uint64_t state = seed;
    for (size_t i = 0; i < m->rows * m->cols; i++) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        d[i] = (double)((state >> 11) & 0x3FFFFF) / (double)(1 << 21) - 1.0;
    }
}

static double
clock_ms(struct timespec *start, struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) * 1000.0
           + (end->tv_nsec - start->tv_nsec) / 1e6;
}

static bool
validate(const Matrix *a, const Matrix *b, const Matrix *result)
{
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; i++) {
        double expected = a->data[i] + b->data[i];
        if (fabs(result->data[i] - expected) > EPSILON) {
            return false;
        }
    }
    return true;
}

int
main(void)
{
    bool all_valid = true;
    int fail_count = 0;
    double primary_ns_elt = 0.0; /* ns/element for largest matrix */

    printf("=== mat_add benchmark ===\n");
    printf("%-8s %12s %10s %10s\n", "Size", "Total(µs)", "ns/elt", "Valid");

    for (int s = 0; s < n_sizes; s++) {
        size_t dim = sizes[s];
        int repeats = (dim <= 64) ? REPEATS_SMALL : REPEATS_LARGE;

        Matrix a = mat_create(dim, dim);
        Matrix b = mat_create(dim, dim);
        Matrix result = mat_create(dim, dim);

        if (!a.data || !b.data || !result.data) {
            printf("FAIL allocation %zu\n", dim);
            fail_count++;
            all_valid = false;
            continue;
        }

        fill_matrix(&a, 42 + s * 7);
        fill_matrix(&b, 137 + s * 11);

        /* Correctness check */
        int rc = mat_add(a, b, &result);
        if (rc != 0 || !validate(&a, &b, &result)) {
            printf("FAIL %zu x %zu: validation error\n", dim, dim);
            fail_count++;
            all_valid = false;
            continue;
        }

        /* Warmup */
        mat_add(a, b, &result);

        /* Benchmark with clock_gettime */
        struct timespec t_start, t_end;
        double total_us = 0.0;

        clock_gettime(CLOCK_MONOTONIC, &t_start);
        for (int r = 0; r < repeats; r++) {
            mat_add(a, b, &result);
        }
        clock_gettime(CLOCK_MONOTONIC, &t_end);

        total_us = clock_ms(&t_start, &t_end) * 1000.0;
        double ns_per_elt = total_us * 1000.0 / ((double)(repeats * dim * dim));

        /* Store primary metric from largest matrix */
        if (s == n_sizes - 1) {
            primary_ns_elt = ns_per_elt;
        }

        printf("%4zu×%4zu %12.1f %9.3f %5s\n",
               dim, dim, total_us, ns_per_elt, "OK");

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
    }

    printf("\n=== Correctness: %s ===\n", all_valid ? "PASS" : "FAIL");

    /* Output structured METRIC lines for autoresearch parsing. */
    if (all_valid) {
        printf("METRIC mat_add_ns_elt=%.6f\n", primary_ns_elt);
    } else {
        printf("METRIC mat_add_ns_elt=999999.000000\n");
    }

    return EXIT_SUCCESS;
}
