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

/* ---------- Aliasing rejection (contract: result must not alias a or b) -- */

TEST_CASE(test_mat_add_alias_result_eq_a)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        /* Construct a Matrix that aliases a's storage. */
        Matrix aliased = {.rows = 2, .cols = 2, .data = a.data};
        TEST_ASSERT(mat_add(a, b, &aliased) == -1);

        /* Operand a must be untouched after rejection. */
        TEST_ASSERT(approx_equal(a.data[0], 1.0));
        TEST_ASSERT(approx_equal(a.data[3], 4.0));

        mat_free(&a);
        mat_free(&b);
}

TEST_CASE(test_mat_add_alias_result_eq_b)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        Matrix aliased = {.rows = 2, .cols = 2, .data = b.data};
        TEST_ASSERT(mat_add(a, b, &aliased) == -1);
        TEST_ASSERT(approx_equal(b.data[0], 5.0));

        mat_free(&a);
        mat_free(&b);
}

TEST_CASE(test_mat_sub_alias_result_eq_a)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        Matrix aliased = {.rows = 2, .cols = 2, .data = a.data};
        TEST_ASSERT(mat_sub(a, b, &aliased) == -1);
        TEST_ASSERT(approx_equal(a.data[0], 1.0));

        mat_free(&a);
        mat_free(&b);
}

TEST_CASE(test_mat_sub_alias_result_eq_b)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        Matrix aliased = {.rows = 2, .cols = 2, .data = b.data};
        TEST_ASSERT(mat_sub(a, b, &aliased) == -1);
        TEST_ASSERT(approx_equal(b.data[0], 5.0));

        mat_free(&a);
        mat_free(&b);
}

/* ---------- Edge shapes ------------------------------------------------- */

TEST_CASE(test_mat_add_1x1)
{
        Matrix a = mat_create(1, 1);
        Matrix b = mat_create(1, 1);
        Matrix r = mat_create(1, 1);
        a.data[0] = 3.5;
        b.data[0] = -1.5;
        TEST_ASSERT(mat_add(a, b, &r) == 0);
        TEST_ASSERT(approx_equal(r.data[0], 2.0));
        mat_free(&a);
        mat_free(&b);
        mat_free(&r);
}

TEST_CASE(test_mat_add_wide_1x100)
{
        Matrix a = mat_create(1, 100);
        Matrix b = mat_create(1, 100);
        Matrix r = mat_create(1, 100);
        for (size_t i = 0; i < 100; i++) {
                a.data[i] = (double)i;
                b.data[i] = (double)(100 - i);
        }
        TEST_ASSERT(mat_add(a, b, &r) == 0);
        for (size_t i = 0; i < 100; i++) {
                TEST_ASSERT(approx_equal(r.data[i], 100.0));
        }
        mat_free(&a);
        mat_free(&b);
        mat_free(&r);
}

TEST_CASE(test_mat_add_tall_100x1)
{
        Matrix a = mat_create(100, 1);
        Matrix b = mat_create(100, 1);
        Matrix r = mat_create(100, 1);
        for (size_t i = 0; i < 100; i++) {
                a.data[i] = 1.0;
                b.data[i] = -1.0;
        }
        TEST_ASSERT(mat_add(a, b, &r) == 0);
        for (size_t i = 0; i < 100; i++) {
                TEST_ASSERT(approx_equal(r.data[i], 0.0));
        }
        mat_free(&a);
        mat_free(&b);
        mat_free(&r);
}

TEST_CASE(test_mat_sub_self_is_zero)
{
        /* A - A = 0; uses two separate buffers because aliasing is rejected. */
        Matrix a = mat_create(4, 4);
        Matrix copy = mat_create(4, 4);
        Matrix r = mat_create(4, 4);
        for (size_t i = 0; i < 16; i++) {
                a.data[i] = (double)i - 7.5;
                copy.data[i] = a.data[i];
        }
        TEST_ASSERT(mat_sub(a, copy, &r) == 0);
        for (size_t i = 0; i < 16; i++) {
                TEST_ASSERT(r.data[i] == 0.0);
        }
        mat_free(&a);
        mat_free(&copy);
        mat_free(&r);
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

        run_test(test_mat_add_alias_result_eq_a,
                 "test_mat_add_alias_result_eq_a");
        run_test(test_mat_add_alias_result_eq_b,
                 "test_mat_add_alias_result_eq_b");
        run_test(test_mat_sub_alias_result_eq_a,
                 "test_mat_sub_alias_result_eq_a");
        run_test(test_mat_sub_alias_result_eq_b,
                 "test_mat_sub_alias_result_eq_b");
        run_test(test_mat_add_1x1, "test_mat_add_1x1");
        run_test(test_mat_add_wide_1x100, "test_mat_add_wide_1x100");
        run_test(test_mat_add_tall_100x1, "test_mat_add_tall_100x1");
        run_test(test_mat_sub_self_is_zero, "test_mat_sub_self_is_zero");

        fprintf(stdout, "\n=== All mat_arithmetic tests passed ===\n\n");
        return EXIT_SUCCESS;
}
