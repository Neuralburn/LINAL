/*
 * @file test_vec_create.c
 * @brief Unit tests for vector creation operations.
 */

#include "test_harness.h"

TEST_CASE(test_vec_create_zero)
{
        Vector v = vec_create(3);

        TEST_ASSERT(v.size == 3);
        TEST_ASSERT(v.data != NULL);

        for (size_t i = 0; i < 3; i++) {
                TEST_ASSERT(v.data[i] == 0.0);
        }

        vec_free(&v);
}

TEST_CASE(test_vec_create_nonzero)
{
        Vector v = vec_create(5);

        TEST_ASSERT(v.size == 5);
        TEST_ASSERT(v.data != NULL);

        vec_free(&v);
}

TEST_CASE(test_vec_create_zero_size)
{
        Vector v = vec_create(0);
        TEST_ASSERT(v.size == 0);
        TEST_ASSERT(v.data == NULL);
        vec_free(&v);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_create tests ===\n\n");

        run_test(test_vec_create_zero, "test_vec_create_zero");
        run_test(test_vec_create_nonzero, "test_vec_create_nonzero");
        run_test(test_vec_create_zero_size, "test_vec_create_zero_size");

        fprintf(stdout, "\n=== All vec_create tests passed ===\n\n");
        return EXIT_SUCCESS;
}
