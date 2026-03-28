/*
 * @file test_mat_dot.c
 * @brief Unit tests for matrix dot product.
 */

#include "test_harness.h"

TEST_CASE(test_dot_2x2_basic)
{
        Matrix A = mat_create(2, 2);
        Matrix B = mat_create(2, 2);

        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        B.data[0] = 5.0;
        B.data[1] = 6.0;
        B.data[2] = 7.0;
        B.data[3] = 8.0;

        double dot = mat_dot(A, B);
        TEST_ASSERT(approx_equal(dot, 70.0)); /* 1*5 + 2*6 + 3*7 + 4*8 */
        mat_free(&A);
        mat_free(&B);
}

TEST_CASE(test_dot_2x3_non_square)
{
        Matrix A = mat_create(2, 3);
        Matrix B = mat_create(2, 3);

        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;
        A.data[4] = 5.0;
        A.data[5] = 6.0;

        B.data[0] = 1.0;
        B.data[1] = 1.0;
        B.data[2] = 1.0;
        B.data[3] = 1.0;
        B.data[4] = 1.0;
        B.data[5] = 1.0;

        double dot = mat_dot(A, B);
        TEST_ASSERT(approx_equal(dot, 21.0)); /* 1+2+3+4+5+6 */
        mat_free(&A);
        mat_free(&B);
}

TEST_CASE(test_dot_1x1_scalar)
{
        Matrix A = mat_create(1, 1);
        Matrix B = mat_create(1, 1);

        A.data[0] = 5.0;
        B.data[0] = 3.0;

        double dot = mat_dot(A, B);
        TEST_ASSERT(approx_equal(dot, 15.0)); /* 5*3 */
        mat_free(&A);
        mat_free(&B);
}

TEST_CASE(test_dot_self_squared_frobenius_norm)
{
        Matrix A = mat_create(2, 2);

        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        double dot = mat_dot(A, A);
        TEST_ASSERT(approx_equal(dot, 30.0)); /* 1+4+9+16 */

        double norm = mat_norm_l2(&A);
        TEST_ASSERT(approx_equal(dot, norm * norm));

        mat_free(&A);
}

TEST_CASE(test_dot_dimension_mismatch)
{
        Matrix A = mat_create(2, 2);
        Matrix B = mat_create(2, 3);

        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        B.data[0] = 1.0;
        B.data[1] = 2.0;
        B.data[2] = 3.0;
        B.data[3] = 4.0;
        B.data[4] = 5.0;
        B.data[5] = 6.0;

        double dot = mat_dot(A, B);
        TEST_ASSERT(isnan(dot));
        mat_free(&A);
        mat_free(&B);
}

TEST_CASE(test_dot_zero_matrix)
{
        Matrix A = mat_create(2, 2);
        Matrix B = mat_create(2, 2);

        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        double dot = mat_dot(A, B);
        TEST_ASSERT(approx_equal(dot, 0.0));
        mat_free(&A);
        mat_free(&B);
}

TEST_CASE(test_dot_identity_matrix)
{
        Matrix A = mat_create(3, 3);
        Matrix I = mat_identity(3);

        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;
        A.data[4] = 5.0;
        A.data[5] = 6.0;
        A.data[6] = 7.0;
        A.data[7] = 8.0;
        A.data[8] = 9.0;

        double dot = mat_dot(A, I);
        TEST_ASSERT(
            approx_equal(dot, 15.0)); /* 1*1 + 5*1 + 9*1 (diagonal only) */
        mat_free(&A);
        mat_free(&I);
}

TEST_CASE(test_dot_commutative)
{
        Matrix A = mat_create(2, 2);
        Matrix B = mat_create(2, 2);

        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        B.data[0] = 5.0;
        B.data[1] = 6.0;
        B.data[2] = 7.0;
        B.data[3] = 8.0;

        double dot_ab = mat_dot(A, B);
        double dot_ba = mat_dot(B, A);
        TEST_ASSERT(approx_equal(dot_ab, dot_ba));
        mat_free(&A);
        mat_free(&B);
}

TEST_CASE(test_dot_null_data_a)
{
        Matrix A = {NULL, 2, 2};
        Matrix B = mat_create(2, 2);

        double dot = mat_dot(A, B);
        TEST_ASSERT(isnan(dot));
        mat_free(&B);
}

TEST_CASE(test_dot_null_data_b)
{
        Matrix A = mat_create(2, 2);
        Matrix B = {NULL, 2, 2};

        double dot = mat_dot(A, B);
        TEST_ASSERT(isnan(dot));
        mat_free(&A);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_dot tests ===\n\n");

        run_test(test_dot_2x2_basic, "test_dot_2x2_basic");
        run_test(test_dot_2x3_non_square, "test_dot_2x3_non_square");
        run_test(test_dot_1x1_scalar, "test_dot_1x1_scalar");
        run_test(test_dot_self_squared_frobenius_norm,
                 "test_dot_self_squared_frobenius_norm");
        run_test(test_dot_dimension_mismatch, "test_dot_dimension_mismatch");
        run_test(test_dot_zero_matrix, "test_dot_zero_matrix");
        run_test(test_dot_identity_matrix, "test_dot_identity_matrix");
        run_test(test_dot_commutative, "test_dot_commutative");
        run_test(test_dot_null_data_a, "test_dot_null_data_a");
        run_test(test_dot_null_data_b, "test_dot_null_data_b");

        fprintf(stdout, "\n=== All mat_dot tests passed ===\n\n");
        return EXIT_SUCCESS;
}
