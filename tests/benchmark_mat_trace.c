/*
 * @file benchmark_mat_trace.c
 * @brief Benchmark mat_trace across matrix sizes with correctness validation.
 */

#include "linal.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPEATS_SMALL 20
#define REPEATS_LARGE 5
#define EPSILON 1e-6

static const size_t sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
static const int n_sizes = sizeof(sizes) / sizeof(sizes[0]);

static void fill_matrix(Matrix *m, uint64_t seed)
{
    double *d = m->data;
    uint64_t state = seed;
    for (size_t i = 0; i < m->rows * m->cols; i++) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        d[i] = (double)((state >> 11) & 0x3FFFFF) / (double)(1 << 21) - 1.0;
    }
}

static double clock_ms(struct timespec *s, struct timespec *e)
{
    return (e->tv_sec - s->tv_sec) * 1000.0 + (e->tv_nsec - s->tv_nsec) / 1e6;
}

int main(void)
{
    bool all_valid = true;
    int fail_count = 0;
    double primary_ms = 0.0;

    printf("=== mat_trace benchmark ===\n");
    printf("%-8s %12s %10s\n", "Size", "Time(ms)", "Valid");

    for (int s = 0; s < n_sizes; s++) {
        size_t dim = sizes[s];
        int repeats = (dim <= 64) ? REPEATS_SMALL : REPEATS_LARGE;

        Matrix a = mat_create(dim, dim);
        if (!a.data) {
            printf("FAIL allocation %zu\n", dim);
            fail_count++;
            all_valid = false;
            continue;
        }

        fill_matrix(&a, 42 + s * 7);

        /* Compute reference */
        double ref = 0.0;
        for (size_t i = 0; i < dim; i++) {
            ref += a.data[i * dim + i];
        }

        double result = mat_trace(&a);
        bool valid = fabs(result - ref) < EPSILON;

        if (!valid) {
            printf("FAIL %zu x %zu: validation error (got %.10f, expected %.10f)\n",
                   dim, dim, result, ref);
            fail_count++;
            all_valid = false;
            mat_free(&a);
            continue;
        }

        /* Warmup */
        mat_trace(&a);

        struct timespec ts, te;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for (int r = 0; r < repeats; r++)
            mat_trace(&a);
        clock_gettime(CLOCK_MONOTONIC, &te);

        double total_ms = clock_ms(&ts, &te);
        double avg_ms = total_ms / (double)repeats;

        if (s == n_sizes - 1)
            primary_ms = avg_ms;

        printf("%4zu×%4zu %12.3f %5s\n", dim, dim, avg_ms, "OK");

        mat_free(&a);
    }

    printf("\n=== Correctness: %s ===\n", all_valid ? "PASS" : "FAIL");

    if (all_valid)
        printf("METRIC mat_trace_ms=%.6f\n", primary_ms);
    else
        printf("METRIC mat_trace_ms=999999.000000\n");

    return EXIT_SUCCESS;
}
