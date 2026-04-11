/*
 * @file test_stress.c
 * @brief Stress tests for LINAL.
 */

#include "test_harness.h"

TEST_CASE(test_large_mul)
{
        size_t n = 50;
        Matrix a = mat_create(n, n);
        Matrix b = mat_create(n, n);
        Matrix res = mat_create(n, n);

        for (size_t i = 0; i < n * n; i++) {
                a.data[i] = (double)(i % 10);
                b.data[i] = (double)((i + 1) % 10);
        }

        int code = mat_mul(a, b, &res);
        TEST_ASSERT(code == 0);

        // Check some values roughly
        // For n=50, elements should be within reasonable range
        for (size_t i = 0; i < n; i++) {
                for (size_t j = 0; j < n; j++) {
                        TEST_ASSERT(res.data[i * n + j] >= 0);
                }
        }

        mat_free(&a);
        mat_free(&b);
        mat_free(&res);
}

TEST_CASE(test_large_det)
{
        size_t n = 20;
        Matrix I = mat_identity(n);
        double det = mat_det(&I);
        TEST_ASSERT(approx_equal(det, 1.0));

        Matrix A = mat_create(n, n);
        // Diagonal matrix
        for (size_t i = 0; i < n; i++) {
                A.data[i * n + i] = 2.0;
        }
        det = mat_det(&A);
        // 2^20
        TEST_ASSERT(approx_equal(det, pow(2.0, (double)n)));

        mat_free(&I);
        mat_free(&A);
}

TEST_CASE(test_large_inv)
{
        size_t n = 20;
        Matrix A = mat_create(n, n);
        for (size_t i = 0; i < n; i++) {
                A.data[i * n + i] = 2.0;
        }

        Matrix inv = mat_create(n, n);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == 0);

        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(approx_equal(inv.data[i * n + i], 0.5));
        }

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_numerical_stability)
{
        Matrix A = mat_create(2, 2);
        // Very small values
        A.data[0] = 1e-12;
        A.data[1] = 0.0;
        A.data[2] = 0.0;
        A.data[3] = 1e-12;

        Matrix inv = mat_create(2, 2);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == 0);
        TEST_ASSERT(approx_equal(inv.data[0], 1e12));

        mat_free(&A);
        mat_free(&inv);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running stress tests ===\n\n");

        run_test(test_large_mul, "test_large_mul");
        run_test(test_large_det, "test_large_det");
        run_test(test_large_inv, "test_large_inv");
        run_test(test_numerical_stability, "test_numerical_stability");

        fprintf(stdout, "\n=== All stress tests passed ===\n\n");
        return EXIT_SUCCESS;
}
