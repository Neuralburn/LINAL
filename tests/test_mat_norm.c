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

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_norm tests ===\n\n");

        run_test(test_norm_l2_identity, "test_norm_l2_identity");
        run_test(test_norm_l2_zero_matrix, "test_norm_l2_zero_matrix");
        run_test(test_norm_l2_simple, "test_norm_l2_simple");
        run_test(test_norm_l2_null_pointer, "test_norm_l2_null_pointer");

        fprintf(stdout, "\n=== All mat_norm tests passed ===\n\n");
        return EXIT_SUCCESS;
}
