/*
 * @file test_vec_access.c
 * @brief Unit tests for vector element access (vec_get / vec_set).
 */

#include "test_harness.h"

TEST_CASE(test_vec_get_valid_index)
{
        Vector v = vec_create(5);
        v.data[0] = 1.5;
        v.data[2] = -3.7;
        v.data[4] = 42.0;

        TEST_ASSERT(approx_equal(vec_get(v, 0), 1.5));
        TEST_ASSERT(approx_equal(vec_get(v, 2), -3.7));
        TEST_ASSERT(approx_equal(vec_get(v, 4), 42.0));

        vec_free(&v);
}

TEST_CASE(test_vec_get_out_of_bounds)
{
        Vector v = vec_create(3);
        v.data[0] = 1.0;

        TEST_ASSERT(isnan(vec_get(v, 3)));
        TEST_ASSERT(isnan(vec_get(v, 100)));

        vec_free(&v);
}

TEST_CASE(test_vec_get_zero_size)
{
        Vector v = vec_create(0);

        TEST_ASSERT(isnan(vec_get(v, 0)));

        vec_free(&v);
}

TEST_CASE(test_vec_get_null_vector)
{
        Vector v = {0};

        TEST_ASSERT(isnan(vec_get(v, 0)));
}

TEST_CASE(test_vec_set_valid_index)
{
        Vector v = vec_create(5);

        TEST_ASSERT(vec_set(&v, 0, 10.0) == 0);
        TEST_ASSERT(approx_equal(v.data[0], 10.0));

        TEST_ASSERT(vec_set(&v, 4, -7.5) == 0);
        TEST_ASSERT(approx_equal(v.data[4], -7.5));

        vec_free(&v);
}

TEST_CASE(test_vec_set_out_of_bounds)
{
        Vector v = vec_create(3);

        TEST_ASSERT(vec_set(&v, 3, 1.0) == -1);
        TEST_ASSERT(vec_set(&v, 100, 1.0) == -1);

        vec_free(&v);
}

TEST_CASE(test_vec_set_zero_size)
{
        Vector v = vec_create(0);

        TEST_ASSERT(vec_set(&v, 0, 1.0) == -1);

        vec_free(&v);
}

TEST_CASE(test_vec_set_null_pointer)
{
        TEST_ASSERT(vec_set(NULL, 0, 1.0) == -1);
}

TEST_CASE(test_vec_set_null_data)
{
        Vector v = {0};

        TEST_ASSERT(vec_set(&v, 0, 1.0) == -1);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_access tests ===\n\n");

        run_test(test_vec_get_valid_index, "test_vec_get_valid_index");
        run_test(test_vec_get_out_of_bounds, "test_vec_get_out_of_bounds");
        run_test(test_vec_get_zero_size, "test_vec_get_zero_size");
        run_test(test_vec_get_null_vector, "test_vec_get_null_vector");
        run_test(test_vec_set_valid_index, "test_vec_set_valid_index");
        run_test(test_vec_set_out_of_bounds, "test_vec_set_out_of_bounds");
        run_test(test_vec_set_zero_size, "test_vec_set_zero_size");
        run_test(test_vec_set_null_pointer, "test_vec_set_null_pointer");
        run_test(test_vec_set_null_data, "test_vec_set_null_data");

        fprintf(stdout, "\n=== All vec_access tests passed ===\n\n");
        return EXIT_SUCCESS;
}
