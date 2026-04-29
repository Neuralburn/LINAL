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

/* Cauchy-Schwarz: |<A,B>| <= ||A||_F * ||B||_F. */
TEST_CASE(test_dot_cauchy_schwarz)
{
        Matrix A = mat_create(3, 4);
        Matrix B = mat_create(3, 4);
        for (size_t i = 0; i < 12; i++) {
                A.data[i] = (double)((i * 7 + 3) % 11) - 5.0;
                B.data[i] = (double)((i * 13 + 1) % 9) - 4.0;
        }
        double dot = mat_dot(A, B);
        double na = mat_norm_l2(&A);
        double nb = mat_norm_l2(&B);

        /* Use a small slack for FP roundoff in norm/dot accumulation. */
        TEST_ASSERT(fabs(dot) <= na * nb + 1e-9);
        mat_free(&A);
        mat_free(&B);
}

/* Linearity in first argument: <alpha*A + B, C> == alpha*<A,C> + <B,C>. */
TEST_CASE(test_dot_linearity)
{
        Matrix A = mat_create(2, 3);
        Matrix B = mat_create(2, 3);
        Matrix C = mat_create(2, 3);
        init_matrix(&A, 2, 3, (double[]){1, 2, 3, 4, 5, 6});
        init_matrix(&B, 2, 3, (double[]){-1, 0, 1, 2, -2, 3});
        init_matrix(&C, 2, 3, (double[]){2, 1, 0, -1, 3, 4});

        double alpha = 1.7;
        Matrix sA = mat_create(2, 3);
        Matrix sum = mat_create(2, 3);
        TEST_ASSERT(mat_scale(A, alpha, &sA) == 0);
        TEST_ASSERT(mat_add(sA, B, &sum) == 0);

        double lhs = mat_dot(sum, C);
        double rhs = alpha * mat_dot(A, C) + mat_dot(B, C);
        TEST_ASSERT(rel_equal(lhs, rhs, DEFAULT_RTOL, DEFAULT_ATOL));

        mat_free(&A); mat_free(&B); mat_free(&C);
        mat_free(&sA); mat_free(&sum);
}

/* Positive definiteness: <A,A> >= 0, == 0 iff A == 0. */
TEST_CASE(test_dot_positive_definite)
{
        Matrix Z = mat_create(3, 3);
        TEST_ASSERT(approx_equal(mat_dot(Z, Z), 0.0));
        mat_free(&Z);

        Matrix A = mat_create(2, 2);
        init_matrix(&A, 2, 2, (double[]){1.0, -2.0, 3.0, -4.0});
        TEST_ASSERT(mat_dot(A, A) > 0.0);
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

        run_test(test_dot_cauchy_schwarz, "test_dot_cauchy_schwarz");
        run_test(test_dot_linearity, "test_dot_linearity");
        run_test(test_dot_positive_definite, "test_dot_positive_definite");

        fprintf(stdout, "\n=== All mat_dot tests passed ===\n\n");
        return EXIT_SUCCESS;
}
