/*
 * @file test_mat_norm.c
 * @brief Unit tests for matrix norm operations.
 */

#include "test_harness.h"

TEST_CASE(test_norm_l2_identity)
{
        Matrix I = mat_identity(3);
        double norm = mat_norm_l2(&I);
        TEST_ASSERT(approx_equal(norm, sqrt(3.0)));
        mat_free(&I);
}

TEST_CASE(test_norm_l2_zero_matrix)
{
        Matrix A = mat_create(2, 2);
        /* All zeros by default */
        double norm = mat_norm_l2(&A);
        TEST_ASSERT(approx_equal(norm, 0.0));
        mat_free(&A);
}

TEST_CASE(test_norm_l2_simple)
{
        Matrix A = mat_create(2, 2);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        double norm = mat_norm_l2(&A);
        TEST_ASSERT(approx_equal(norm, sqrt(1.0 + 4.0 + 9.0 + 16.0)));
        mat_free(&A);
}

TEST_CASE(test_norm_l2_null_pointer)
{
        double norm = mat_norm_l2(NULL);
        TEST_ASSERT(isnan(norm));
}

/* Triangle inequality: ||A+B||_F <= ||A||_F + ||B||_F. */
TEST_CASE(test_norm_l2_triangle_inequality)
{
        Matrix A = mat_create(2, 3);
        Matrix B = mat_create(2, 3);
        init_matrix(&A, 2, 3, (double[]){1, -2, 3, 4, 5, -6});
        init_matrix(&B, 2, 3, (double[]){-2, 1, 4, 0, -3, 2});

        Matrix sum = mat_create(2, 3);
        TEST_ASSERT(mat_add(A, B, &sum) == 0);

        double na = mat_norm_l2(&A);
        double nb = mat_norm_l2(&B);
        double nsum = mat_norm_l2(&sum);

        TEST_ASSERT(nsum <= na + nb + 1e-9);

        mat_free(&A);
        mat_free(&B);
        mat_free(&sum);
}

/* Absolute homogeneity: ||alpha*A||_F == |alpha| * ||A||_F. */
TEST_CASE(test_norm_l2_scaling)
{
        Matrix A = mat_create(3, 2);
        init_matrix(&A, 3, 2, (double[]){1, -2, 3, -4, 5, -6});
        double na = mat_norm_l2(&A);
        Matrix sA = mat_create(3, 2);

        double alphas[] = {0.0, 1.0, -1.0, 2.5, -3.0, 1e-6};
        for (size_t k = 0; k < sizeof(alphas) / sizeof(alphas[0]); k++) {
                double a = alphas[k];
                TEST_ASSERT(mat_scale(A, a, &sA) == 0);
                double nsA = mat_norm_l2(&sA);
                TEST_ASSERT(rel_equal(nsA, fabs(a) * na,
                                      DEFAULT_RTOL, DEFAULT_ATOL));
        }

        mat_free(&A);
        mat_free(&sA);
}

/* Single non-zero entry: ||A||_F == |entry|. */
TEST_CASE(test_norm_l2_single_entry)
{
        Matrix A = mat_create(4, 4);
        A.data[7] = -3.5;
        TEST_ASSERT(approx_equal(mat_norm_l2(&A), 3.5));
        mat_free(&A);
}

/* Norm of NULL data field. */
TEST_CASE(test_norm_l2_null_data)
{
        Matrix A = {.rows = 2, .cols = 2, .data = NULL};
        TEST_ASSERT(isnan(mat_norm_l2(&A)));
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_norm tests ===\n\n");

        run_test(test_norm_l2_identity, "test_norm_l2_identity");
        run_test(test_norm_l2_zero_matrix, "test_norm_l2_zero_matrix");
        run_test(test_norm_l2_simple, "test_norm_l2_simple");
        run_test(test_norm_l2_null_pointer, "test_norm_l2_null_pointer");

        run_test(test_norm_l2_triangle_inequality,
                 "test_norm_l2_triangle_inequality");
        run_test(test_norm_l2_scaling, "test_norm_l2_scaling");
        run_test(test_norm_l2_single_entry, "test_norm_l2_single_entry");
        run_test(test_norm_l2_null_data, "test_norm_l2_null_data");

        fprintf(stdout, "\n=== All mat_norm tests passed ===\n\n");
        return EXIT_SUCCESS;
}
