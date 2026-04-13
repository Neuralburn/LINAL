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

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_trace tests ===\n\n");

        run_test(test_trace_identity, "test_trace_identity");
        run_test(test_trace_2x2, "test_trace_2x2");
        run_test(test_trace_non_square, "test_trace_non_square");
        run_test(test_trace_null_pointer, "test_trace_null_pointer");

        fprintf(stdout, "\n=== All mat_trace tests passed ===\n\n");
        return EXIT_SUCCESS;
}
