/*
 * @file benchmark_vec_add.c
 * @brief Benchmark vec_add across vector sizes with correctness validation.
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

static bool
validate(const Vector *a, const Vector *b, const Vector *result)
{
    for (size_t i = 0; i < a->size; i++) {
        double expected = a->data[i] + b->data[i];
        if (fabs(result->data[i] - expected) > EPSILON) {
            return false;
        }
    }
    return true;
}

int main(void)
{
    bool all_valid = true;
    double primary_us = 0.0;

    printf("=== vec_add benchmark (median of %d runs) ===\n", REPEATS);
    printf("%-12s %12s %10s\n", "Size", "Time(us)", "Valid");

    double times[REPEATS];

    for (int s = 0; s < n_sizes; s++) {
        size_t dim = sizes[s];

        Vector a = vec_create(dim);
        Vector b = vec_create(dim);
        Vector result = vec_create(dim);

        if (!a.data || !b.data || !result.data) {
            printf("FAIL allocation %zu\n", dim);
            all_valid = false;
            continue;
        }

        fill_vector(&a, 42 + s * 7);
        fill_vector(&b, 99 + s * 13);

        /* Correctness check */
        int rc = vec_add(a, b, &result);
        bool valid = (rc == 0) && validate(&a, &b, &result);

        if (!valid) {
            printf("FAIL %zu: validation error (rc=%d)\n", dim, rc);
            all_valid = false;
            continue;
        }

        /* Warmup */
        vec_add(a, b, &result);
        vec_add(a, b, &result);
        vec_add(a, b, &result);

        /* Timed runs */
        for (int r = 0; r < REPEATS; r++) {
            struct timespec ts, te;
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
            vec_add(a, b, &result);
            clock_gettime(CLOCK_MONOTONIC_RAW, &te);

            double ns = (double)(te.tv_sec - ts.tv_sec) * 1e9
                       + (double)(te.tv_nsec - ts.tv_nsec);
            times[r] = ns / 1000.0; /* Convert to µs */
        }

        double median_us = median_double(times, REPEATS);

        if (s == n_sizes - 1)
            primary_us = median_us;

        printf("%12zu %12.3f %5s\n", dim, median_us, "OK");

        vec_free(&a);
        vec_free(&b);
        vec_free(&result);
    }

    printf("\n=== Correctness: %s ===\n", all_valid ? "PASS" : "FAIL");

    if (all_valid)
        printf("METRIC total_us=%.3f\n", primary_us);
    else
        printf("METRIC total_us=999999.000\n");

    return EXIT_SUCCESS;
}
