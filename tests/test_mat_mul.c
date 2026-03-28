/*
 * @file test_mat_mul.c
 * @brief Unit tests for matrix multiplication.
 */

#include "test_harness.h"

TEST_CASE(test_mat_mul_success_square)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == 0);

        /* [1 2] * [5 6] = [19 22] */
        /* [3 4]   [7 8]   [43 50] */
        TEST_ASSERT(approx_equal(result.data[0], 19.0));
        TEST_ASSERT(approx_equal(result.data[1], 22.0));
        TEST_ASSERT(approx_equal(result.data[2], 43.0));
        TEST_ASSERT(approx_equal(result.data[3], 50.0));

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_success_rectangular)
{
        Matrix a = mat_create(2, 3);
        Matrix b = mat_create(3, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
        init_matrix(&b, 3, 2, (double[]){7.0, 8.0, 9.0, 10.0, 11.0, 12.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == 0);

        TEST_ASSERT(approx_equal(result.data[0], 58.0));
        TEST_ASSERT(approx_equal(result.data[1], 64.0));
        TEST_ASSERT(approx_equal(result.data[2], 139.0));
        TEST_ASSERT(approx_equal(result.data[3], 154.0));

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_identity)
{
        Matrix a = mat_create(3, 3);
        Matrix b = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        /* Create identity matrix A */
        init_matrix(&a, 3, 3,
                    (double[]){1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});
        /* Create identity matrix B */
        init_matrix(&b, 3, 3,
                    (double[]){1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == 0);

        /* A * I = A, so result should be identity */
        for (size_t i = 0; i < 9; i++) {
                if (i % 3 == i / 3) {
                        TEST_ASSERT(approx_equal(result.data[i], 1.0));
                } else {
                        TEST_ASSERT(approx_equal(result.data[i], 0.0));
                }
        }

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_dimension_mismatch)
{
        Matrix a = mat_create(2, 3);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
        init_matrix(&b, 2, 2, (double[]){7.0, 8.0, 9.0, 10.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_empty_matrix)
{
        Matrix a = mat_create(0, 3);
        Matrix b = mat_create(3, 2);
        Matrix result = mat_create(0, 2);

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_null_result)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = {.rows = 2, .cols = 2, .data = NULL};

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
}

TEST_CASE(test_mat_mul_null_input_a)
{
        Matrix a = {.rows = 2, .cols = 2, .data = NULL};
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_null_input_b)
{
        Matrix a = mat_create(2, 2);
        Matrix b = {.rows = 2, .cols = 2, .data = NULL};
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&result);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_mul tests ===\n\n");

        run_test(test_mat_mul_success_square, "test_mat_mul_success_square");
        run_test(test_mat_mul_success_rectangular,
                 "test_mat_mul_success_rectangular");
        run_test(test_mat_mul_identity, "test_mat_mul_identity");
        run_test(test_mat_mul_dimension_mismatch,
                 "test_mat_mul_dimension_mismatch");
        run_test(test_mat_mul_empty_matrix, "test_mat_mul_empty_matrix");
        run_test(test_mat_mul_null_result, "test_mat_mul_null_result");
        run_test(test_mat_mul_null_input_a, "test_mat_mul_null_input_a");
        run_test(test_mat_mul_null_input_b, "test_mat_mul_null_input_b");

        fprintf(stdout, "\n=== All mat_mul tests passed ===\n\n");
        return EXIT_SUCCESS;
}
