/*
 * @file test_mat_inv.c
 * @brief Unit tests for matrix inversion.
 */

#include "test_harness.h"

TEST_CASE(test_inv_1x1)
{
        Matrix A = mat_create(1, 1);
        A.data[0] = 2.0;

        Matrix inv = mat_create(1, 1);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == 0);
        TEST_ASSERT(approx_equal(inv.data[0], 0.5));

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_inv_2x2)
{
        Matrix A = mat_create(2, 2);
        A.data[0] = 4.0;
        A.data[1] = 7.0;
        A.data[2] = 2.0;
        A.data[3] = 6.0;

        Matrix inv = mat_create(2, 2);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == 0);

        /* Inverse of [4 7] is [0.6  -0.7] */
        /*            [2 6]    [-0.2  0.4] */
        TEST_ASSERT(approx_equal(inv.data[0], 0.6));
        TEST_ASSERT(approx_equal(inv.data[1], -0.7));
        TEST_ASSERT(approx_equal(inv.data[2], -0.2));
        TEST_ASSERT(approx_equal(inv.data[3], 0.4));

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_inv_3x3)
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

        Matrix inv = mat_create(3, 3);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == 0);

        /* Verify A * A^-1 = I */
        Matrix result = mat_create(3, 3);
        mat_mul(A, inv, &result);

        for (size_t i = 0; i < 3; i++) {
                for (size_t j = 0; j < 3; j++) {
                        if (i == j) {
                                TEST_ASSERT(
                                    approx_equal(result.data[i * 3 + j], 1.0));
                        } else {
                                TEST_ASSERT(
                                    approx_equal(result.data[i * 3 + j], 0.0));
                        }
                }
        }

        mat_free(&A);
        mat_free(&inv);
        mat_free(&result);
}

TEST_CASE(test_inv_identity)
{
        Matrix I = mat_identity(3);
        Matrix inv = mat_create(3, 3);

        int code = mat_inv(I, &inv);
        TEST_ASSERT(code == 0);

        /* Inverse of identity is identity */
        for (size_t i = 0; i < 9; i++) {
                TEST_ASSERT(approx_equal(I.data[i], inv.data[i]));
        }

        mat_free(&I);
        mat_free(&inv);
}

TEST_CASE(test_inv_diagonal)
{
        Matrix D = mat_create(3, 3);
        D.data[0] = 2.0;
        D.data[4] = 3.0;
        D.data[8] = 4.0;

        Matrix inv = mat_create(3, 3);
        int code = mat_inv(D, &inv);
        TEST_ASSERT(code == 0);

        /* Inverse of diagonal is reciprocal of diagonal elements */
        TEST_ASSERT(approx_equal(inv.data[0], 0.5));
        TEST_ASSERT(approx_equal(inv.data[4], 1.0 / 3.0));
        TEST_ASSERT(approx_equal(inv.data[8], 0.25));
        TEST_ASSERT(approx_equal(inv.data[1], 0.0));
        TEST_ASSERT(approx_equal(inv.data[2], 0.0));
        TEST_ASSERT(approx_equal(inv.data[3], 0.0));
        TEST_ASSERT(approx_equal(inv.data[5], 0.0));
        TEST_ASSERT(approx_equal(inv.data[6], 0.0));
        TEST_ASSERT(approx_equal(inv.data[7], 0.0));

        mat_free(&D);
        mat_free(&inv);
}

TEST_CASE(test_inv_singular_returns_error)
{
        Matrix A = mat_create(2, 2);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 2.0;
        A.data[3] = 4.0; /* Second row is 2x first row */

        Matrix inv = mat_create(2, 2);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == -1);

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_inv_zero_matrix_returns_error)
{
        Matrix A = mat_create(2, 2);
        /* All zeros - singular */

        Matrix inv = mat_create(2, 2);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == -1);

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_inv_non_square_returns_error)
{
        Matrix A = mat_create(2, 3);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;
        A.data[4] = 5.0;
        A.data[5] = 6.0;

        Matrix inv = mat_create(3, 2);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == -1);

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_inv_null_input)
{
        Matrix A = mat_create(2, 2);
        A.data = NULL;

        Matrix inv = mat_create(2, 2);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == -1);

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_inv_null_result)
{
        Matrix A = mat_create(2, 2);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        Matrix inv = mat_create(2, 2);
        inv.data = NULL;

        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == -1);

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_inv_null_result_ptr)
{
        Matrix A = mat_create(2, 2);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        int code = mat_inv(A, NULL);
        TEST_ASSERT(code == -1);

        mat_free(&A);
}

TEST_CASE(test_inv_dimension_mismatch)
{
        Matrix A = mat_create(2, 2);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        Matrix inv = mat_create(3, 3);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == -1);

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_inv_inverse_property_left)
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

        Matrix inv = mat_create(3, 3);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == 0);

        /* Verify A * A^-1 = I */
        Matrix result = mat_create(3, 3);
        mat_mul(A, inv, &result);

        for (size_t i = 0; i < 9; i++) {
                size_t row = i / 3;
                size_t col = i % 3;
                if (row == col) {
                        TEST_ASSERT(approx_equal(result.data[i], 1.0));
                } else {
                        TEST_ASSERT(approx_equal(result.data[i], 0.0));
                }
        }

        mat_free(&A);
        mat_free(&inv);
        mat_free(&result);
}

TEST_CASE(test_inv_inverse_property_right)
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

        Matrix inv = mat_create(3, 3);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == 0);

        /* Verify A^-1 * A = I */
        Matrix result = mat_create(3, 3);
        mat_mul(inv, A, &result);

        for (size_t i = 0; i < 9; i++) {
                size_t row = i / 3;
                size_t col = i % 3;
                if (row == col) {
                        TEST_ASSERT(approx_equal(result.data[i], 1.0));
                } else {
                        TEST_ASSERT(approx_equal(result.data[i], 0.0));
                }
        }

        mat_free(&A);
        mat_free(&inv);
        mat_free(&result);
}

TEST_CASE(test_inv_double_inverse)
{
        Matrix A = mat_create(2, 2);
        A.data[0] = 3.0;
        A.data[1] = 5.0;
        A.data[2] = 2.0;
        A.data[3] = 4.0;

        Matrix inv = mat_create(2, 2);
        mat_inv(A, &inv);

        Matrix inv_inv = mat_create(2, 2);
        int code = mat_inv(inv, &inv_inv);
        TEST_ASSERT(code == 0);

        /* (A^-1)^-1 = A */
        for (size_t i = 0; i < 4; i++) {
                TEST_ASSERT(approx_equal(A.data[i], inv_inv.data[i]));
        }

        mat_free(&A);
        mat_free(&inv);
        mat_free(&inv_inv);
}

TEST_CASE(test_inv_4x4)
{
        Matrix A = mat_create(4, 4);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;
        A.data[4] = 5.0;
        A.data[5] = 6.0;
        A.data[6] = 7.0;
        A.data[7] = 8.0;
        A.data[8] = 9.0;
        A.data[9] = 10.0;
        A.data[10] = 11.0;
        A.data[11] = 12.0;
        A.data[12] = 13.0;
        A.data[13] = 14.0;
        A.data[14] = 15.0;
        A.data[15] = 16.0;

        Matrix inv = mat_create(4, 4);
        int code = mat_inv(A, &inv);
        /* This matrix is singular (rows are linearly dependent) */
        TEST_ASSERT(code == -1);

        mat_free(&A);
        mat_free(&inv);
}

TEST_CASE(test_inv_4x4_non_singular)
{
        Matrix A = mat_create(4, 4);
        /* Upper triangular non-singular 4x4 matrix (det = 24) */
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;
        A.data[4] = 0.0;
        A.data[5] = 2.0;
        A.data[6] = 3.0;
        A.data[7] = 4.0;
        A.data[8] = 0.0;
        A.data[9] = 0.0;
        A.data[10] = 3.0;
        A.data[11] = 4.0;
        A.data[12] = 0.0;
        A.data[13] = 0.0;
        A.data[14] = 0.0;
        A.data[15] = 4.0;

        Matrix inv = mat_create(4, 4);
        int code = mat_inv(A, &inv);
        TEST_ASSERT(code == 0);

        /* Verify A * A^-1 = I */
        Matrix result = mat_create(4, 4);
        mat_mul(A, inv, &result);

        for (size_t i = 0; i < 16; i++) {
                size_t row = i / 4;
                size_t col = i % 4;
                if (row == col) {
                        TEST_ASSERT(approx_equal(result.data[i], 1.0));
                } else {
                        TEST_ASSERT(approx_equal(result.data[i], 0.0));
                }
        }

        mat_free(&A);
        mat_free(&inv);
        mat_free(&result);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_inv tests ===\n\n");

        run_test(test_inv_1x1, "test_inv_1x1");
        run_test(test_inv_2x2, "test_inv_2x2");
        run_test(test_inv_3x3, "test_inv_3x3");
        run_test(test_inv_identity, "test_inv_identity");
        run_test(test_inv_diagonal, "test_inv_diagonal");
        run_test(test_inv_singular_returns_error,
                 "test_inv_singular_returns_error");
        run_test(test_inv_zero_matrix_returns_error,
                 "test_inv_zero_matrix_returns_error");
        run_test(test_inv_non_square_returns_error,
                 "test_inv_non_square_returns_error");
        run_test(test_inv_null_input, "test_inv_null_input");
        run_test(test_inv_null_result, "test_inv_null_result");
        run_test(test_inv_null_result_ptr, "test_inv_null_result_ptr");
        run_test(test_inv_dimension_mismatch, "test_inv_dimension_mismatch");
        run_test(test_inv_inverse_property_left,
                 "test_inv_inverse_property_left");
        run_test(test_inv_inverse_property_right,
                 "test_inv_inverse_property_right");
        run_test(test_inv_double_inverse, "test_inv_double_inverse");
        run_test(test_inv_4x4, "test_inv_4x4");
        run_test(test_inv_4x4_non_singular, "test_inv_4x4_non_singular");

        fprintf(stdout, "\n=== All mat_inv tests passed ===\n\n");
        return EXIT_SUCCESS;
}
