/*
 * @file test_mat_scale.c
 * @brief Unit tests for matrix scaling.
 */

#include "test_harness.h"

TEST_CASE(test_mat_scale_success)
{
        Matrix m = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 3, 3,
                    (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == 0);

        TEST_ASSERT(approx_equal(result.data[0], 2.0));
        TEST_ASSERT(approx_equal(result.data[1], 4.0));
        TEST_ASSERT(approx_equal(result.data[2], 6.0));
        TEST_ASSERT(approx_equal(result.data[3], 8.0));
        TEST_ASSERT(approx_equal(result.data[4], 10.0));
        TEST_ASSERT(approx_equal(result.data[5], 12.0));
        TEST_ASSERT(approx_equal(result.data[6], 14.0));
        TEST_ASSERT(approx_equal(result.data[7], 16.0));
        TEST_ASSERT(approx_equal(result.data[8], 18.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_zero)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 0.0, &result);

        TEST_ASSERT(result_code == 0);
        for (size_t i = 0; i < 4; i++) {
                TEST_ASSERT(result.data[i] == 0.0);
        }

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_negative)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, -1.5, &result);

        TEST_ASSERT(result_code == 0);
        TEST_ASSERT(approx_equal(result.data[0], -1.5));
        TEST_ASSERT(approx_equal(result.data[1], -3.0));
        TEST_ASSERT(approx_equal(result.data[2], -4.5));
        TEST_ASSERT(approx_equal(result.data[3], -6.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_null_input)
{
        Matrix m = mat_create(2, 2);
        m.data = NULL;
        Matrix result = mat_create(2, 2);

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_null_result)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);
        result.data = NULL;

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_null_result_ptr)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 2.0, NULL);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_dimension_mismatch)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_scale tests ===\n\n");

        run_test(test_mat_scale_success, "test_mat_scale_success");
        run_test(test_mat_scale_zero, "test_mat_scale_zero");
        run_test(test_mat_scale_negative, "test_mat_scale_negative");
        run_test(test_mat_scale_null_input, "test_mat_scale_null_input");
        run_test(test_mat_scale_null_result, "test_mat_scale_null_result");
        run_test(test_mat_scale_null_result_ptr,
                 "test_mat_scale_null_result_ptr");
        run_test(test_mat_scale_dimension_mismatch,
                 "test_mat_scale_dimension_mismatch");

        fprintf(stdout, "\n=== All mat_scale tests passed ===\n\n");
        return EXIT_SUCCESS;
}
