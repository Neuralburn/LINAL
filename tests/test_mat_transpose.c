/*
 * @file test_mat_transpose.c
 * @brief Unit tests for matrix transpose.
 */

#include "test_harness.h"

TEST_CASE(test_mat_transpose_success_square)
{
        Matrix m = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 3, 3,
                    (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == 0);

        /* Transpose of [1 2 3] is [1 4 7] */
        /*              [4 5 6]   [2 5 8] */
        /*              [7 8 9]   [3 6 9] */
        TEST_ASSERT(approx_equal(result.data[0], 1.0));
        TEST_ASSERT(approx_equal(result.data[1], 4.0));
        TEST_ASSERT(approx_equal(result.data[2], 7.0));
        TEST_ASSERT(approx_equal(result.data[3], 2.0));
        TEST_ASSERT(approx_equal(result.data[4], 5.0));
        TEST_ASSERT(approx_equal(result.data[5], 8.0));
        TEST_ASSERT(approx_equal(result.data[6], 3.0));
        TEST_ASSERT(approx_equal(result.data[7], 6.0));
        TEST_ASSERT(approx_equal(result.data[8], 9.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_success_rectangular)
{
        Matrix m = mat_create(2, 3);
        Matrix result = mat_create(3, 2);

        init_matrix(&m, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == 0);

        /* Transpose of [1 2 3] is [1 4] */
        /*              [4 5 6]   [2 5] */
        /*                         [3 6] */
        TEST_ASSERT(approx_equal(result.data[0], 1.0));
        TEST_ASSERT(approx_equal(result.data[1], 4.0));
        TEST_ASSERT(approx_equal(result.data[2], 2.0));
        TEST_ASSERT(approx_equal(result.data[3], 5.0));
        TEST_ASSERT(approx_equal(result.data[4], 3.0));
        TEST_ASSERT(approx_equal(result.data[5], 6.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_null_input)
{
        Matrix m = {.rows = 2, .cols = 2, .data = NULL};
        Matrix result = mat_create(2, 2);

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&result);
}

TEST_CASE(test_mat_transpose_null_result)
{
        Matrix m = mat_create(2, 2);
        Matrix result = {.rows = 2, .cols = 2, .data = NULL};

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
}

TEST_CASE(test_mat_transpose_null_result_ptr)
{
        Matrix m = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_transpose(m, NULL);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
}

TEST_CASE(test_mat_transpose_dimension_mismatch)
{
        Matrix m = mat_create(2, 3);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_identity)
{
        Matrix m = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 3, 3,
                    (double[]){1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == 0);

        /* Transpose of identity is identity */
        TEST_ASSERT(mat_equal(m, result) == 0);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_double_transpose)
{
        Matrix m = mat_create(2, 3);
        Matrix t = mat_create(3, 2);
        Matrix tt = mat_create(2, 3);

        init_matrix(&m, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        mat_transpose(m, &t);
        mat_transpose(t, &tt);

        TEST_ASSERT(mat_equal(m, tt) == 0);

        mat_free(&m);
        mat_free(&t);
        mat_free(&tt);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_transpose tests ===\n\n");

        run_test(test_mat_transpose_success_square,
                 "test_mat_transpose_success_square");
        run_test(test_mat_transpose_success_rectangular,
                 "test_mat_transpose_success_rectangular");
        run_test(test_mat_transpose_null_input,
                 "test_mat_transpose_null_input");
        run_test(test_mat_transpose_null_result,
                 "test_mat_transpose_null_result");
        run_test(test_mat_transpose_null_result_ptr,
                 "test_mat_transpose_null_result_ptr");
        run_test(test_mat_transpose_dimension_mismatch,
                 "test_mat_transpose_dimension_mismatch");
        run_test(test_mat_transpose_identity, "test_mat_transpose_identity");
        run_test(test_mat_transpose_double_transpose,
                 "test_mat_transpose_double_transpose");

        fprintf(stdout, "\n=== All mat_transpose tests passed ===\n\n");
        return EXIT_SUCCESS;
}
