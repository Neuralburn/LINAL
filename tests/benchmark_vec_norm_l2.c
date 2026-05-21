/*
 * @file benchmark_vec_norm_l2.c
 * @brief Benchmark vec_norm_l2 across vector sizes with correctness validation.
 *
 * Uses CLOCK_MONOTONIC_RAW for nanosecond precision.
 * Runs multiple repeats per size and reports median to reduce noise.
 */

#include "linal.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPEATS 20
#define EPSILON 1e-6

static const size_t sizes[] = {
    65536, 262144, 1048576, 4194304, 16777216, 67108864
};
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

static double
median_double(double *arr, int n)
{
    /* Simple insertion sort for small n */
    for (int i = 1; i < n; i++) {
        double key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
    if (n % 2 == 0)
        return (arr[n / 2 - 1] + arr[n / 2]) / 2.0;
    return arr[n / 2];
}

int main(void)
{
    bool all_valid = true;
    double primary_us = 0.0;

    printf("=== vec_norm_l2 benchmark (median of %d runs) ===\n", REPEATS);
    printf("%-12s %12s %10s\n", "Size", "Time(us)", "Valid");

    double times[REPEATS];

    for (int s = 0; s < n_sizes; s++) {
        size_t dim = sizes[s];

        Vector v = vec_create(dim);

        if (!v.data) {
            printf("FAIL allocation %zu\n", dim);
            all_valid = false;
            continue;
        }

        fill_vector(&v, 42 + s * 7);

        /* Compute reference */
        double ref_sum = 0.0;
        for (size_t i = 0; i < dim; i++)
            ref_sum += v.data[i] * v.data[i];
        double ref = sqrt(ref_sum);

        double result = vec_norm_l2(v);
        bool valid = fabs(result - ref) < EPSILON;

        if (!valid) {
            printf("FAIL %zu: validation error (got %.10f, expected %.10f)\n",
                   dim, result, ref);
            all_valid = false;
            continue;
        }

        /* Warmup */
        vec_norm_l2(v);
        vec_norm_l2(v);
        vec_norm_l2(v);

        /* Timed runs */
        for (int r = 0; r < REPEATS; r++) {
            struct timespec ts, te;
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
            result = vec_norm_l2(v);
            clock_gettime(CLOCK_MONOTONIC_RAW, &te);

            double ns = (double)(te.tv_sec - ts.tv_sec) * 1e9
                       + (double)(te.tv_nsec - ts.tv_nsec);
            times[r] = ns / 1000.0; /* Convert to µs */
        }

        /* Prevent compiler from optimizing away the result */
        if (result != result) { /* Never true, but uses 'result' */
            printf("unexpected\n");
        }

        double median_us = median_double(times, REPEATS);

        if (s == n_sizes - 1)
            primary_us = median_us;

        printf("%12zu %12.3f %5s\n", dim, median_us, "OK");

        vec_free(&v);
    }

    printf("\n=== Correctness: %s ===\n", all_valid ? "PASS" : "FAIL");

    if (all_valid)
        printf("METRIC total_us=%.3f\n", primary_us);
    else
        printf("METRIC total_us=999999.000\n");

    return EXIT_SUCCESS;
}
