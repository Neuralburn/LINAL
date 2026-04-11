/*
 * @file test_mat_det.c
 * @brief Unit tests for matrix determinant.
 */

#include "test_harness.h"

TEST_CASE(test_det_1x1)
{
        Matrix A = mat_create(1, 1);
        A.data[0] = 5.0;

        double det = mat_det(&A);
        TEST_ASSERT(approx_equal(det, 5.0));
        mat_free(&A);
}

TEST_CASE(test_det_2x2)
{
        Matrix A = mat_create(2, 2);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        double det = mat_det(&A);
        TEST_ASSERT(approx_equal(det, -2.0)); /* 1*4 - 2*3 = -2 */
        mat_free(&A);
}

TEST_CASE(test_det_3x3)
{
        Matrix A = mat_create(3, 3);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 0.0;
        A.data[4] = 1.0;
        A.data[5] = 4.0;
        A.data[6] = 5.0;
        A.data[7] = 6.0;
        A.data[8] = 0.0;

        double det = mat_det(&A);
        TEST_ASSERT(approx_equal(
            det,
            1.0)); /* det = 1*(0-24) - 2*(0-20) + 3*(0-5) = -24+40-15 = 1 */
        mat_free(&A);
}

TEST_CASE(test_det_3x3_non_singular)
{
        Matrix A = mat_create(3, 3);
        A.data[0] = 6.0;
        A.data[1] = 1.0;
        A.data[2] = 1.0;
        A.data[3] = 4.0;
        A.data[4] = -2.0;
        A.data[5] = 5.0;
        A.data[6] = 2.0;
        A.data[7] = 8.0;
        A.data[8] = 7.0;

        double det = mat_det(&A);
        TEST_ASSERT(approx_equal(det, -306.0));
        mat_free(&A);
}

TEST_CASE(test_det_identity)
{
        Matrix I = mat_identity(4);
        double det = mat_det(&I);
        TEST_ASSERT(approx_equal(det, 1.0));
        mat_free(&I);
}

TEST_CASE(test_det_diagonal)
{
        Matrix D = mat_create(3, 3);
        D.data[0] = 2.0;
        D.data[4] = 3.0;
        D.data[8] = 4.0;

        double det = mat_det(&D);
        TEST_ASSERT(approx_equal(det, 24.0)); /* 2 * 3 * 4 */
        mat_free(&D);
}

TEST_CASE(test_det_zero_matrix)
{
        Matrix A = mat_create(2, 2);
        /* All zeros by default */
        double det = mat_det(&A);
        TEST_ASSERT(approx_equal(det, 0.0));
        mat_free(&A);
}

TEST_CASE(test_det_non_square_returns_zero)
{
        Matrix A = mat_create(2, 3);
        double det = mat_det(&A);
        TEST_ASSERT(approx_equal(det, 0.0));
        mat_free(&A);
}

TEST_CASE(test_det_null_matrix)
{
        double det = mat_det(NULL);
        TEST_ASSERT(approx_equal(det, 0.0));
}

TEST_CASE(test_det_null_data)
{
        Matrix A = {0};
        A.rows = 2;
        A.cols = 2;

        double det = mat_det(&A);
        TEST_ASSERT(approx_equal(det, 0.0));
}

TEST_CASE(test_det_large_matrix)
{
        size_t n = 20;
        Matrix I = mat_identity(n);
        double det = mat_det(&I);
        TEST_ASSERT(approx_equal(det, 1.0));
        mat_free(&I);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_det tests ===\n\n");

        run_test(test_det_1x1, "test_det_1x1");
        run_test(test_det_2x2, "test_det_2x2");
        run_test(test_det_3x3, "test_det_3x3");
        run_test(test_det_3x3_non_singular, "test_det_3x3_non_singular");
        run_test(test_det_identity, "test_det_identity");
        run_test(test_det_diagonal, "test_det_diagonal");
        run_test(test_det_zero_matrix, "test_det_zero_matrix");
        run_test(test_det_non_square_returns_zero,
                 "test_det_non_square_returns_zero");
        run_test(test_det_null_matrix, "test_det_null_matrix");
        run_test(test_det_null_data, "test_det_null_data");
        run_test(test_det_large_matrix, "test_det_large_matrix");

        fprintf(stdout, "\n=== All mat_det tests passed ===\n\n");
        return EXIT_SUCCESS;
}
