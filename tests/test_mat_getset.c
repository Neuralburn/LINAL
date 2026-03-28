/*
 * @file test_mat_getset.c
 * @brief Unit tests for matrix get/set operations.
 */

#include "test_harness.h"

TEST_CASE(test_mat_get_success)
{
        Matrix m = mat_create(3, 3);

        init_matrix(&m, 3, 3,
                    (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});

        TEST_ASSERT(approx_equal(mat_get(m, 0, 0), 1.0));
        TEST_ASSERT(approx_equal(mat_get(m, 0, 1), 2.0));
        TEST_ASSERT(approx_equal(mat_get(m, 1, 1), 5.0));
        TEST_ASSERT(approx_equal(mat_get(m, 2, 2), 9.0));

        mat_free(&m);
}

TEST_CASE(test_mat_get_out_of_bounds_row)
{
        Matrix m = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        double val = mat_get(m, 5, 0);
        TEST_ASSERT(isnan(val));

        val = mat_get(m, 2, 0);
        TEST_ASSERT(isnan(val));

        mat_free(&m);
}

TEST_CASE(test_mat_get_out_of_bounds_col)
{
        Matrix m = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        double val = mat_get(m, 0, 5);
        TEST_ASSERT(isnan(val));

        val = mat_get(m, 0, 2);
        TEST_ASSERT(isnan(val));

        mat_free(&m);
}

TEST_CASE(test_mat_get_null_data)
{
        Matrix m = mat_create(2, 2);
        m.data = NULL;

        double val = mat_get(m, 0, 0);
        TEST_ASSERT(isnan(val));

        mat_free(&m);
}

TEST_CASE(test_mat_set_success)
{
        Matrix m = mat_create(3, 3);

        TEST_ASSERT(mat_set(&m, 0, 0, 1.0) == 0);
        TEST_ASSERT(mat_set(&m, 0, 1, 2.0) == 0);
        TEST_ASSERT(mat_set(&m, 1, 1, 5.0) == 0);
        TEST_ASSERT(mat_set(&m, 2, 2, 9.0) == 0);

        TEST_ASSERT(approx_equal(m.data[0], 1.0));
        TEST_ASSERT(approx_equal(m.data[1], 2.0));
        TEST_ASSERT(approx_equal(m.data[4], 5.0));
        TEST_ASSERT(approx_equal(m.data[8], 9.0));

        mat_free(&m);
}

TEST_CASE(test_mat_set_out_of_bounds_row)
{
        Matrix m = mat_create(2, 2);

        int result = mat_set(&m, 5, 0, 1.0);
        TEST_ASSERT(result == -1);

        result = mat_set(&m, 2, 0, 1.0);
        TEST_ASSERT(result == -1);

        mat_free(&m);
}

TEST_CASE(test_mat_set_out_of_bounds_col)
{
        Matrix m = mat_create(2, 2);

        int result = mat_set(&m, 0, 5, 1.0);
        TEST_ASSERT(result == -1);

        result = mat_set(&m, 0, 2, 1.0);
        TEST_ASSERT(result == -1);

        mat_free(&m);
}

TEST_CASE(test_mat_set_null_matrix)
{
        int result = mat_set(NULL, 0, 0, 1.0);
        TEST_ASSERT(result == -1);
}

TEST_CASE(test_mat_set_null_data)
{
        Matrix m = mat_create(2, 2);
        m.data = NULL;

        int result = mat_set(&m, 0, 0, 1.0);
        TEST_ASSERT(result == -1);

        mat_free(&m);
}

TEST_CASE(test_mat_get_set_roundtrip)
{
        Matrix m = mat_create(3, 4);

        for (size_t i = 0; i < 3; i++) {
                for (size_t j = 0; j < 4; j++) {
                        double val = (double)(i * 10 + j);
                        TEST_ASSERT(mat_set(&m, i, j, val) == 0);
                        TEST_ASSERT(approx_equal(mat_get(m, i, j), val));
                }
        }

        mat_free(&m);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_getset tests ===\n\n");

        run_test(test_mat_get_success, "test_mat_get_success");
        run_test(test_mat_get_out_of_bounds_row,
                 "test_mat_get_out_of_bounds_row");
        run_test(test_mat_get_out_of_bounds_col,
                 "test_mat_get_out_of_bounds_col");
        run_test(test_mat_get_null_data, "test_mat_get_null_data");
        run_test(test_mat_set_success, "test_mat_set_success");
        run_test(test_mat_set_out_of_bounds_row,
                 "test_mat_set_out_of_bounds_row");
        run_test(test_mat_set_out_of_bounds_col,
                 "test_mat_set_out_of_bounds_col");
        run_test(test_mat_set_null_matrix, "test_mat_set_null_matrix");
        run_test(test_mat_set_null_data, "test_mat_set_null_data");
        run_test(test_mat_get_set_roundtrip, "test_mat_get_set_roundtrip");

        fprintf(stdout, "\n=== All mat_getset tests passed ===\n\n");
        return EXIT_SUCCESS;
}
