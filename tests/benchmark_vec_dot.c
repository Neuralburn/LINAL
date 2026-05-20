/*
 * @file benchmark_vec_dot.c
 * @brief Benchmark vec_dot across vector sizes with correctness validation.
 */

#include "linal.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPEATS_SMALL 50
#define REPEATS_LARGE 10
#define EPSILON 1e-6

static const size_t sizes[] = {64, 128, 256, 512, 1024, 4096, 16384, 65536};
static const int n_sizes = sizeof(sizes) / sizeof(sizes[0]);

static void fill_vector(Vector *v, uint64_t seed)
{
    double *d = v->data;
    uint64_t state = seed;
    for (size_t i = 0; i < v->size; i++) {
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
    double primary_us = 0.0;

    printf("=== vec_dot benchmark ===\n");
    printf("%-8s %12s %10s\n", "Size", "Time(us)", "Valid");

    for (int s = 0; s < n_sizes; s++) {
        size_t dim = sizes[s];
        int repeats = (dim <= 1024) ? REPEATS_SMALL : REPEATS_LARGE;

        Vector a = vec_create(dim);
        Vector b = vec_create(dim);

        if (!a.data || !b.data) {
            printf("FAIL allocation %zu\n", dim);
            all_valid = false;
            continue;
        }

        fill_vector(&a, 42 + s * 7);
        fill_vector(&b, 99 + s * 13);

        /* Compute reference */
        double ref = 0.0;
        for (size_t i = 0; i < dim; i++)
            ref += a.data[i] * b.data[i];

        double result = vec_dot(a, b);
        bool valid = fabs(result - ref) < EPSILON;

        if (!valid) {
            printf("FAIL %zu: validation error (got %.10f, expected %.10f)\n",
                   dim, result, ref);
            all_valid = false;
            continue;
        }

        /* Warmup */
        vec_dot(a, b);

        struct timespec ts, te;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        for (int r = 0; r < repeats; r++)
            vec_dot(a, b);
        clock_gettime(CLOCK_MONOTONIC, &te);

        double total_us = clock_ms(&ts, &te) * 1000.0 / (double)repeats;

        if (s == n_sizes - 1)
            primary_us = total_us;

        printf("%8zu %12.2f %5s\n", dim, total_us, "OK");

        vec_free(&a);
        vec_free(&b);
    }

    printf("\n=== Correctness: %s ===\n", all_valid ? "PASS" : "FAIL");

    if (all_valid)
        printf("METRIC total_us=%.2f\n", primary_us);
    else
        printf("METRIC total_us=999999.00\n");

    return EXIT_SUCCESS;
}
