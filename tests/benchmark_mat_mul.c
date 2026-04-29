/*
 * @file benchmark_mat_mul.c
 * @brief Benchmark and correctness validation for mat_mul across sizes.
 */

#include "linal.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPEATS 5
#define EPSILON 1e-6

/* Reference naive mat_mul (i-j-k order) used for correctness validation */
static void
ref_mul(const double *a, size_t a_cols, const double *b, size_t b_cols,
        double *result, size_t r_rows, size_t r_cols, size_t k_dim)
{
    memset(result, 0, r_rows * r_cols * sizeof(double));
    for (size_t i = 0; i < r_rows; i++) {
        for (size_t j = 0; j < r_cols; j++) {
            double sum = 0.0;
            for (size_t kk = 0; kk < k_dim; kk++) {
                sum += a[i * a_cols + kk] * b[kk * b_cols + j];
            }
            result[i * r_cols + j] = sum;
        }
    }
}

static bool
mat_equal(const double *a, const double *b, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (fabs(a[i] - b[i]) > EPSILON) return false;
    }
    return true;
}

static double
clock_ms(struct timespec *start, struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) * 1000.0
           + (end->tv_nsec - start->tv_nsec) / 1e6;
}

/* Fill matrix with deterministic pseudo-random values */
static void fill_matrix(Matrix *m, int seed)
{
    srand(seed);
    for (size_t i = 0; i < m->rows * m->cols; i++) {
        m->data[i] = ((double)(rand() % 100)) / 10.0 - 5.0;
    }
}

int main(void)
{
    int errors = 0;
    struct timespec ts_start, ts_end;
    double times[REPEATS];
    bool all_ok = true;

    /* Test sizes: square and rectangular */
    size_t sizes[] = {4, 8, 16, 32, 64, 128, 256};
    int n_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int s = 0; s < n_sizes; s++) {
        size_t n = sizes[s];

        /* Square matrices */
        Matrix a = mat_create(n, n);
        Matrix b = mat_create(n, n);
        Matrix result = mat_create(n, n);
        Matrix ref = mat_create(n, n);

        fill_matrix(&a, 42 + s * 10);
        fill_matrix(&b, 99 + s * 7);

        /* Correctness: compare against reference */
        ref_mul(a.data, a.cols, b.data, b.cols, ref.data, ref.rows, ref.cols,
                a.cols);

        mat_mul(a, b, &result);

        size_t total = n * n;
        if (!mat_equal(result.data, ref.data, total)) {
            fprintf(stderr, "FAIL correctness square %zu x %zu\n", n, n);
            errors++;
            all_ok = false;
        } else {
            /* Benchmark: warm-up run */
            mat_mul(a, b, &result);

            /* Timed runs */
            for (int r = 0; r < REPEATS; r++) {
                clock_gettime(CLOCK_MONOTONIC, &ts_start);
                mat_mul(a, b, &result);
                clock_gettime(CLOCK_MONOTONIC, &ts_end);
                times[r] = clock_ms(&ts_start, &ts_end);

                /* Re-check correctness each run */
                if (!mat_equal(result.data, ref.data, total)) {
                    fprintf(stderr, "FAIL correctness square %zu x %zu (run %d)\n",
                            n, n, r);
                    errors++;
                    all_ok = false;
                }
            }

            /* Sort times and take median */
            for (int i = 0; i < REPEATS - 1; i++) {
                for (int j = i + 1; j < REPEATS; j++) {
                    if (times[j] < times[i]) {
                        double tmp = times[i];
                        times[i] = times[j];
                        times[j] = tmp;
                    }
                }
            }
            double median = times[REPEATS / 2];

            printf("MATMUL %zu,%zu,square,%.3f\n", n, n, median);
        }

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
        mat_free(&ref);

        /* Rectangular: (n x n/2) * (n/2 x n) */
        if (n >= 4) {
            size_t half = n / 2;
            Matrix a2 = mat_create(n, half);
            Matrix b2 = mat_create(half, n);
            Matrix result2 = mat_create(n, n);
            Matrix ref2 = mat_create(n, n);

            fill_matrix(&a2, 55 + s * 13);
            fill_matrix(&b2, 77 + s * 11);

            ref_mul(a2.data, a2.cols, b2.data, b2.cols, ref2.data, ref2.rows,
                    ref2.cols, a2.cols);

            mat_mul(a2, b2, &result2);

            if (!mat_equal(result2.data, ref2.data, total)) {
                fprintf(stderr, "FAIL correctness rect %zu x %zu * %zu x %zu\n",
                        n, half, half, n);
                errors++;
                all_ok = false;
            } else {
                /* Warm-up */
                mat_mul(a2, b2, &result2);

                for (int r = 0; r < REPEATS; r++) {
                    clock_gettime(CLOCK_MONOTONIC, &ts_start);
                    mat_mul(a2, b2, &result2);
                    clock_gettime(CLOCK_MONOTONIC, &ts_end);
                    times[r] = clock_ms(&ts_start, &ts_end);
                }

                for (int i = 0; i < REPEATS - 1; i++) {
                    for (int j = i + 1; j < REPEATS; j++) {
                        if (times[j] < times[i]) {
                            double tmp = times[i];
                            times[i] = times[j];
                            times[j] = tmp;
                        }
                    }
                }
                double median = times[REPEATS / 2];

                printf("MATMUL %zu,%zu,rect,%.3f\n", n, half, median);
            }

            mat_free(&a2);
            mat_free(&b2);
            mat_free(&result2);
            mat_free(&ref2);
        }
    }

    if (!all_ok) {
        fprintf(stderr, "\n=== VALIDATION FAILED: %d errors ===\n", errors);
        return EXIT_FAILURE;
    } else {
        printf("VALID OK\n");
        return EXIT_SUCCESS;
    }
}
