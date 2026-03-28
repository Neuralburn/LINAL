/*
 * @file test_mat_memory.c
 * @brief Unit tests for matrix memory management operations.
 */

#include "test_harness.h"

TEST_CASE(test_mat_free_null)
{
        mat_free(NULL);
}

TEST_CASE(test_mat_free_invalid)
{
        Matrix m = mat_create(2, 2);
        m.data = NULL;
        mat_free(&m);
}

TEST_CASE(test_mat_free_cleanup)
{
        Matrix m = mat_create(3, 3);
        m.data[0] = 1.0;

        mat_free(&m);

        TEST_ASSERT(m.data == NULL);
        TEST_ASSERT(m.rows == 0);
        TEST_ASSERT(m.cols == 0);
}

TEST_CASE(test_mat_copy_success)
{
        Matrix src = mat_create(2, 3);
        Matrix dest = mat_create(2, 3);

        init_matrix(&src, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        int result = mat_copy(src, &dest);

        TEST_ASSERT(result == 0);
        TEST_ASSERT(mat_equal(src, dest) == 0);

        mat_free(&src);
        mat_free(&dest);
}

TEST_CASE(test_mat_copy_dimension_mismatch)
{
        Matrix src = mat_create(2, 3);
        Matrix dest = mat_create(3, 2);

        init_matrix(&src, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        int result = mat_copy(src, &dest);

        TEST_ASSERT(result == -1);

        mat_free(&src);
        mat_free(&dest);
}

TEST_CASE(test_mat_copy_null_dest)
{
        Matrix src = mat_create(2, 2);
        Matrix dest = mat_create(2, 2);
        dest.data = NULL;

        init_matrix(&src, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result = mat_copy(src, &dest);

        TEST_ASSERT(result == -1);

        mat_free(&src);
        mat_free(&dest);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_memory tests ===\n\n");

        run_test(test_mat_free_null, "test_mat_free_null");
        run_test(test_mat_free_invalid, "test_mat_free_invalid");
        run_test(test_mat_free_cleanup, "test_mat_free_cleanup");
        run_test(test_mat_copy_success, "test_mat_copy_success");
        run_test(test_mat_copy_dimension_mismatch,
                 "test_mat_copy_dimension_mismatch");
        run_test(test_mat_copy_null_dest, "test_mat_copy_null_dest");

        fprintf(stdout, "\n=== All mat_memory tests passed ===\n\n");
        return EXIT_SUCCESS;
}
