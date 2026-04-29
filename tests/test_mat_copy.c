/*
 * @file test_mat_copy.c
 * @brief Unit tests for mat_copy.
 */

#include "test_harness.h"

TEST_CASE(test_copy_success)
{
        Matrix src = mat_create(2, 3);
        Matrix dest = mat_create(2, 3);
        init_matrix(&src, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        TEST_ASSERT(mat_copy(src, &dest) == 0);
        TEST_ASSERT(mat_equal(src, dest) == 0);

        mat_free(&src);
        mat_free(&dest);
}

TEST_CASE(test_copy_is_deep)
{
        Matrix src = mat_create(2, 2);
        Matrix dest = mat_create(2, 2);
        init_matrix(&src, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        TEST_ASSERT(mat_copy(src, &dest) == 0);

        /* Mutate src; dest must be unaffected. */
        src.data[0] = 99.0;
        TEST_ASSERT(approx_equal(dest.data[0], 1.0));

        mat_free(&src);
        mat_free(&dest);
}

TEST_CASE(test_copy_alias_rejected)
{
        Matrix src = mat_create(2, 2);
        init_matrix(&src, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        /* Construct a Matrix that aliases src's storage. */
        Matrix aliased = {.rows = 2, .cols = 2, .data = src.data};
        TEST_ASSERT(mat_copy(src, &aliased) == -1);

        /* src untouched. */
        TEST_ASSERT(approx_equal(src.data[0], 1.0));
        TEST_ASSERT(approx_equal(src.data[3], 4.0));

        mat_free(&src);
}

TEST_CASE(test_copy_dim_mismatch)
{
        Matrix src = mat_create(2, 3);
        Matrix dest = mat_create(3, 2);
        init_matrix(&src, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        TEST_ASSERT(mat_copy(src, &dest) == -1);

        mat_free(&src);
        mat_free(&dest);
}

TEST_CASE(test_copy_null_src_data)
{
        Matrix src = {.rows = 2, .cols = 2, .data = NULL};
        Matrix dest = mat_create(2, 2);

        TEST_ASSERT(mat_copy(src, &dest) == -1);

        mat_free(&dest);
}

TEST_CASE(test_copy_null_dest_ptr)
{
        Matrix src = mat_create(2, 2);
        init_matrix(&src, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        TEST_ASSERT(mat_copy(src, NULL) == -1);

        mat_free(&src);
}

TEST_CASE(test_copy_null_dest_data)
{
        Matrix src = mat_create(2, 2);
        Matrix dest = {.rows = 2, .cols = 2, .data = NULL};
        init_matrix(&src, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        TEST_ASSERT(mat_copy(src, &dest) == -1);

        mat_free(&src);
}

TEST_CASE(test_copy_large)
{
        size_t r = 64, c = 96;
        Matrix src = mat_create(r, c);
        Matrix dest = mat_create(r, c);
        for (size_t i = 0; i < r * c; i++) {
                src.data[i] = (double)i * 0.5 - 100.0;
        }
        TEST_ASSERT(mat_copy(src, &dest) == 0);
        for (size_t i = 0; i < r * c; i++) {
                TEST_ASSERT(approx_equal(src.data[i], dest.data[i]));
        }
        mat_free(&src);
        mat_free(&dest);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_copy tests ===\n\n");

        run_test(test_copy_success, "test_copy_success");
        run_test(test_copy_is_deep, "test_copy_is_deep");
        run_test(test_copy_alias_rejected, "test_copy_alias_rejected");
        run_test(test_copy_dim_mismatch, "test_copy_dim_mismatch");
        run_test(test_copy_null_src_data, "test_copy_null_src_data");
        run_test(test_copy_null_dest_ptr, "test_copy_null_dest_ptr");
        run_test(test_copy_null_dest_data, "test_copy_null_dest_data");
        run_test(test_copy_large, "test_copy_large");

        fprintf(stdout, "\n=== All mat_copy tests passed ===\n\n");
        return EXIT_SUCCESS;
}
