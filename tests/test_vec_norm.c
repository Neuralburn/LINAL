/*
 * @file test_vec_norm.c
 * @brief Unit tests for vector L2 norm.
 */

#include "test_harness.h"

TEST_CASE(test_vec_norm_simple)
{
        Vector v = vec_create(3);
        v.data[0] = 3.0;
        v.data[1] = 4.0;
        v.data[2] = 0.0;

        TEST_ASSERT(approx_equal(vec_norm_l2(v), 5.0));

        vec_free(&v);
}

TEST_CASE(test_vec_norm_zero)
{
        Vector v = vec_create(3);
        // data is already 0.0

        TEST_ASSERT(approx_equal(vec_norm_l2(v), 0.0));

        vec_free(&v);
}

TEST_CASE(test_vec_norm_null)
{
        Vector v = {0}; // v.data is NULL

        TEST_ASSERT(isnan(vec_norm_l2(v)));
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_norm tests ===\n\n");

        run_test(test_vec_norm_simple, "test_vec_norm_simple");
        run_test(test_vec_norm_zero, "test_vec_norm_zero");
        run_test(test_vec_norm_null, "test_vec_norm_null");

        fprintf(stdout, "\n=== All vec_norm tests passed ===\n\n");
        return EXIT_SUCCESS;
}
