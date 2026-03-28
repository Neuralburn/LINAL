/*
 * @file test_mat_arithmetic.c
 * @brief Unit tests for matrix arithmetic operations (add, sub).
 */

#include "test_harness.h"

TEST_CASE(test_mat_add_success)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == 0);
        TEST_ASSERT(result.rows == 2);
        TEST_ASSERT(result.cols == 2);

        TEST_ASSERT(approx_equal(result.data[0], 6.0));
        TEST_ASSERT(approx_equal(result.data[1], 8.0));
        TEST_ASSERT(approx_equal(result.data[2], 10.0));
        TEST_ASSERT(approx_equal(result.data[3], 12.0));

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_add_dimension_mismatch)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 3);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_add_null_result)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = {.rows = 2, .cols = 2, .data = NULL};

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
}

TEST_CASE(test_mat_add_null_input_a)
{
        Matrix a = {.rows = 2, .cols = 2, .data = NULL};
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_add_null_input_b)
{
        Matrix a = mat_create(2, 2);
        Matrix b = {.rows = 2, .cols = 2, .data = NULL};
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&result);
}

TEST_CASE(test_mat_add_zero_matrices)
{
        Matrix a = mat_create(3, 3);
        Matrix b = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == 0);
        for (size_t i = 0; i < 9; i++) {
                TEST_ASSERT(result.data[i] == 0.0);
        }

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_sub_success)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_sub(a, b, &result);

        TEST_ASSERT(result_code == 0);
        TEST_ASSERT(result.rows == 2);
        TEST_ASSERT(result.cols == 2);

        TEST_ASSERT(approx_equal(result.data[0], -4.0));
        TEST_ASSERT(approx_equal(result.data[1], -4.0));
        TEST_ASSERT(approx_equal(result.data[2], -4.0));
        TEST_ASSERT(approx_equal(result.data[3], -4.0));

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_sub_dimension_mismatch)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 3);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        int result_code = mat_sub(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_sub_null_result)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = {.rows = 2, .cols = 2, .data = NULL};

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_sub(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
}

TEST_CASE(test_mat_sub_null_input_a)
{
        Matrix a = {.rows = 2, .cols = 2, .data = NULL};
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_sub(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_sub_null_input_b)
{
        Matrix a = mat_create(2, 2);
        Matrix b = {.rows = 2, .cols = 2, .data = NULL};
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_sub(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&result);
}

TEST_CASE(test_mat_sub_zero_matrices)
{
        Matrix a = mat_create(3, 3);
        Matrix b = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        int result_code = mat_sub(a, b, &result);

        TEST_ASSERT(result_code == 0);
        for (size_t i = 0; i < 9; i++) {
                TEST_ASSERT(result.data[i] == 0.0);
        }

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_sub_add_inverse)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix sum = mat_create(2, 2);
        Matrix diff = mat_create(2, 2);
        Matrix zero = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        mat_add(a, b, &sum);
        mat_sub(sum, b, &diff);

        TEST_ASSERT(mat_equal(a, diff) == 0);

        mat_free(&a);
        mat_free(&b);
        mat_free(&sum);
        mat_free(&diff);
        mat_free(&zero);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_arithmetic tests ===\n\n");

        run_test(test_mat_add_success, "test_mat_add_success");
        run_test(test_mat_add_dimension_mismatch,
                 "test_mat_add_dimension_mismatch");
        run_test(test_mat_add_null_result, "test_mat_add_null_result");
        run_test(test_mat_add_null_input_a, "test_mat_add_null_input_a");
        run_test(test_mat_add_null_input_b, "test_mat_add_null_input_b");
        run_test(test_mat_add_zero_matrices, "test_mat_add_zero_matrices");
        run_test(test_mat_sub_success, "test_mat_sub_success");
        run_test(test_mat_sub_dimension_mismatch,
                 "test_mat_sub_dimension_mismatch");
        run_test(test_mat_sub_null_result, "test_mat_sub_null_result");
        run_test(test_mat_sub_null_input_a, "test_mat_sub_null_input_a");
        run_test(test_mat_sub_null_input_b, "test_mat_sub_null_input_b");
        run_test(test_mat_sub_zero_matrices, "test_mat_sub_zero_matrices");
        run_test(test_mat_sub_add_inverse, "test_mat_sub_add_inverse");

        fprintf(stdout, "\n=== All mat_arithmetic tests passed ===\n\n");
        return EXIT_SUCCESS;
}
