/*
 * @file test_vec_copy.c
 * @brief Unit tests for vector deep copy (vec_copy).
 */

#include "test_harness.h"

TEST_CASE(test_vec_copy_basic)
{
        Vector src = vec_create(5);
        src.data[0] = 1.0;
        src.data[1] = 2.0;
        src.data[2] = 3.0;
        src.data[3] = 4.0;
        src.data[4] = 5.0;

        Vector dest = vec_create(5);
        TEST_ASSERT(vec_copy(src, &dest) == 0);
        TEST_ASSERT(dest.size == src.size);

        for (size_t i = 0; i < src.size; i++) {
                TEST_ASSERT(approx_equal(dest.data[i], src.data[i]));
        }

        /* Modify src — dest should be independent. */
        src.data[0] = 999.0;
        TEST_ASSERT(approx_equal(dest.data[0], 1.0));

        vec_free(&src);
        vec_free(&dest);
}

TEST_CASE(test_vec_copy_null_dest)
{
        Vector src = vec_create(3);
        src.data[0] = 1.0;

        TEST_ASSERT(vec_copy(src, NULL) == -1);

        vec_free(&src);
}

TEST_CASE(test_vec_copy_null_src_data)
{
        Vector src = {0};
        Vector dest = vec_create(3);

        TEST_ASSERT(vec_copy(src, &dest) == -1);

        vec_free(&dest);
}

TEST_CASE(test_vec_copy_dest_pre_allocated)
{
        Vector src = vec_create(3);
        src.data[0] = 1.0;
        src.data[1] = 2.0;
        src.data[2] = 3.0;

        Vector dest = vec_create(3);
        dest.data[0] = 999.0; /* pre-filled */

        TEST_ASSERT(vec_copy(src, &dest) == 0);
        TEST_ASSERT(approx_equal(dest.data[0], 1.0));

        vec_free(&src);
        vec_free(&dest);
}

TEST_CASE(test_vec_copy_size_mismatch)
{
        Vector src = vec_create(5);
        Vector dest = vec_create(3);

        TEST_ASSERT(vec_copy(src, &dest) == -1);

        vec_free(&src);
        vec_free(&dest);
}

TEST_CASE(test_vec_copy_self_copy)
{
        Vector v = vec_create(3);
        v.data[0] = 1.0;
        v.data[1] = 2.0;
        v.data[2] = 3.0;

        /* Self-copy: src.data == dest->data, should be rejected */
        TEST_ASSERT(vec_copy(v, &v) == -1);

        vec_free(&v);
}

TEST_CASE(test_vec_copy_zero_size)
{
        Vector src = vec_create(0);
        Vector dest = vec_create(0);

        TEST_ASSERT(vec_copy(src, &dest) == 0);
        TEST_ASSERT(dest.size == 0);
        TEST_ASSERT(dest.data == NULL);

        vec_free(&src);
        vec_free(&dest);
}

TEST_CASE(test_vec_copy_1_element)
{
        Vector src = vec_create(1);
        src.data[0] = 42.0;

        Vector dest = vec_create(1);
        TEST_ASSERT(vec_copy(src, &dest) == 0);
        TEST_ASSERT(approx_equal(dest.data[0], 42.0));

        vec_free(&src);
        vec_free(&dest);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_copy tests ===\n\n");

        run_test(test_vec_copy_basic, "test_vec_copy_basic");
        run_test(test_vec_copy_null_dest, "test_vec_copy_null_dest");
        run_test(test_vec_copy_null_src_data, "test_vec_copy_null_src_data");
        run_test(test_vec_copy_dest_pre_allocated,
                 "test_vec_copy_dest_pre_allocated");
        run_test(test_vec_copy_size_mismatch, "test_vec_copy_size_mismatch");
        run_test(test_vec_copy_self_copy, "test_vec_copy_self_copy");
        run_test(test_vec_copy_zero_size, "test_vec_copy_zero_size");
        run_test(test_vec_copy_1_element, "test_vec_copy_1_element");

        fprintf(stdout, "\n=== All vec_copy tests passed ===\n\n");
        return EXIT_SUCCESS;
}
