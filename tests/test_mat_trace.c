/*
 * @file test_mat_trace.c
 * @brief Unit tests for matrix trace.
 */

#include "test_harness.h"

TEST_CASE(test_trace_identity)
{
        Matrix I = mat_identity(5);
        double trace = mat_trace(&I);
        TEST_ASSERT(approx_equal(trace, 5.0));
        mat_free(&I);
}

TEST_CASE(test_trace_2x2)
{
        Matrix A = mat_create(2, 2);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;

        double trace = mat_trace(&A);
        TEST_ASSERT(approx_equal(trace, 5.0)); /* 1 + 4 */
        mat_free(&A);
}

TEST_CASE(test_trace_non_square)
{
        Matrix A = mat_create(2, 3);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 3.0;
        A.data[3] = 4.0;
        A.data[4] = 5.0;
        A.data[5] = 6.0;

        double trace = mat_trace(&A);
        TEST_ASSERT(approx_equal(trace, 6.0)); /* 1 + 5 */
        mat_free(&A);
}

TEST_CASE(test_trace_null_pointer)
{
        double trace = mat_trace(NULL);
        TEST_ASSERT(isnan(trace));
}

/* tr(A^T) == tr(A) for square A. */
TEST_CASE(test_trace_transpose_invariant)
{
        Matrix A = mat_create(4, 4);
        for (size_t i = 0; i < 16; i++) {
                A.data[i] = (double)i - 8.0;
        }
        Matrix AT = mat_create(4, 4);
        TEST_ASSERT(mat_transpose(A, &AT) == 0);
        TEST_ASSERT(approx_equal(mat_trace(&A), mat_trace(&AT)));
        mat_free(&A);
        mat_free(&AT);
}

/* Cyclic property: tr(AB) == tr(BA) for compatible shapes. */
TEST_CASE(test_trace_cyclic)
{
        /* A: 2x3, B: 3x2 — AB is 2x2, BA is 3x3, but traces are equal. */
        Matrix A = mat_create(2, 3);
        Matrix B = mat_create(3, 2);
        init_matrix(&A, 2, 3, (double[]){1, 2, 3, 4, 5, 6});
        init_matrix(&B, 3, 2, (double[]){7, 8, 9, 10, 11, 12});

        Matrix AB = mat_create(2, 2);
        Matrix BA = mat_create(3, 3);
        TEST_ASSERT(mat_mul(A, B, &AB) == 0);
        TEST_ASSERT(mat_mul(B, A, &BA) == 0);
        TEST_ASSERT(rel_equal(mat_trace(&AB), mat_trace(&BA),
                              DEFAULT_RTOL, DEFAULT_ATOL));

        mat_free(&A); mat_free(&B);
        mat_free(&AB); mat_free(&BA);
}

/* Linearity: tr(alpha*A + B) == alpha*tr(A) + tr(B). */
TEST_CASE(test_trace_linearity)
{
        Matrix A = mat_create(3, 3);
        Matrix B = mat_create(3, 3);
        init_matrix(&A, 3, 3, (double[]){1,2,3,4,5,6,7,8,9});
        init_matrix(&B, 3, 3, (double[]){-1,0,2, 5,3,-4, 8,-2,1});

        double alpha = 1.5;
        Matrix sA = mat_create(3, 3);
        Matrix sum = mat_create(3, 3);
        TEST_ASSERT(mat_scale(A, alpha, &sA) == 0);
        TEST_ASSERT(mat_add(sA, B, &sum) == 0);

        double lhs = mat_trace(&sum);
        double rhs = alpha * mat_trace(&A) + mat_trace(&B);
        TEST_ASSERT(rel_equal(lhs, rhs, DEFAULT_RTOL, DEFAULT_ATOL));

        mat_free(&A); mat_free(&B);
        mat_free(&sA); mat_free(&sum);
}

TEST_CASE(test_trace_null_data)
{
        Matrix A = {.rows = 2, .cols = 2, .data = NULL};
        TEST_ASSERT(isnan(mat_trace(&A)));
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_trace tests ===\n\n");

        run_test(test_trace_identity, "test_trace_identity");
        run_test(test_trace_2x2, "test_trace_2x2");
        run_test(test_trace_non_square, "test_trace_non_square");
        run_test(test_trace_null_pointer, "test_trace_null_pointer");

        run_test(test_trace_transpose_invariant,
                 "test_trace_transpose_invariant");
        run_test(test_trace_cyclic, "test_trace_cyclic");
        run_test(test_trace_linearity, "test_trace_linearity");
        run_test(test_trace_null_data, "test_trace_null_data");

        fprintf(stdout, "\n=== All mat_trace tests passed ===\n\n");
        return EXIT_SUCCESS;
}
