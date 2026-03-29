/*
 * @file test_harness.h
 * @brief Common test harness for LINAL unit tests.
 */

#ifndef LINAL_TEST_HARNESS_H
#define LINAL_TEST_HARNESS_H

#include "linal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Test assertion macro */
#define TEST_ASSERT(expr)                                                      \
        do {                                                                   \
                if (!(expr)) {                                                 \
                        fprintf(stderr, "FAIL  %s:%d  %s\n", __FILE__,         \
                                __LINE__, #expr);                              \
                        exit(EXIT_FAILURE);                                    \
                }                                                              \
        } while (0)

/* Test pass output */
#define TEST_PASS(name) fprintf(stdout, "PASS  %s\n", (name))

/* Test case macro */
#define TEST_CASE(name)                                                        \
        static void name(void);                                                \
        static void name(void)

/* Epsilon for floating point comparisons */
#define EPSILON 1e-10

/* Compare two doubles with tolerance */
static inline int
approx_equal(double a, double b)
{
        return fabs(a - b) < EPSILON;
}

/* Compare two matrices for equality */
static int __attribute__((unused))
mat_equal(const Matrix a, const Matrix b)
{
        if (a.rows != b.rows || a.cols != b.cols) {
                return -1;
        }

        for (size_t i = 0; i < a.rows * a.cols; i++) {
                if (!approx_equal(a.data[i], b.data[i])) {
                        return -1;
                }
        }

        return 0;
}

/* Initialize matrix with given values */
static void __attribute__((unused))
init_matrix(Matrix *m, size_t rows, size_t cols, const double *values)
{
        TEST_ASSERT(m->data != NULL);
        for (size_t i = 0; i < rows * cols; i++) {
                m->data[i] = values[i];
        }
}

/* Run a single test */
static void
run_test(void (*test_func)(void), const char *name)
{
        test_func();
        TEST_PASS(name);
}

#endif /* LINAL_TEST_HARNESS_H */
