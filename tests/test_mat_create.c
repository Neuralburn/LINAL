/*
 * @file test_mat_create.c
 * @brief Unit tests for matrix creation operations.
 */

#include "test_harness.h"

TEST_CASE(test_mat_create_zero)
{
        Matrix m = mat_create(3, 3);

        TEST_ASSERT(m.rows == 3);
        TEST_ASSERT(m.cols == 3);
        TEST_ASSERT(m.data != NULL);

        for (size_t i = 0; i < 9; i++) {
                TEST_ASSERT(m.data[i] == 0.0);
        }

        mat_free(&m);
}

TEST_CASE(test_mat_create_nonzero)
{
        Matrix m = mat_create(2, 4);

        TEST_ASSERT(m.rows == 2);
        TEST_ASSERT(m.cols == 4);
        TEST_ASSERT(m.data != NULL);

        mat_free(&m);
}

TEST_CASE(test_mat_create_zero_dimensions)
{
        Matrix m = mat_create(0, 5);
        TEST_ASSERT(m.rows == 0);
        TEST_ASSERT(m.cols == 5);
        TEST_ASSERT(m.data == NULL);

        m = mat_create(3, 0);
        TEST_ASSERT(m.rows == 3);
        TEST_ASSERT(m.cols == 0);
        TEST_ASSERT(m.data == NULL);

        mat_free(&m);
}

TEST_CASE(test_mat_create_allocation_failure)
{
        Matrix m = mat_create(100000, 100000);
        TEST_ASSERT(m.data == NULL || m.rows == 0 || m.cols == 0);
        mat_free(&m);
}

TEST_CASE(test_mat_identity_1x1)
{
        Matrix I = mat_identity(1);
        TEST_ASSERT(I.rows == 1);
        TEST_ASSERT(I.cols == 1);
        TEST_ASSERT(approx_equal(I.data[0], 1.0));
        mat_free(&I);
}

TEST_CASE(test_mat_identity_2x2)
{
        Matrix I = mat_identity(2);
        TEST_ASSERT(I.rows == 2);
        TEST_ASSERT(I.cols == 2);
        TEST_ASSERT(approx_equal(I.data[0], 1.0));
        TEST_ASSERT(approx_equal(I.data[1], 0.0));
        TEST_ASSERT(approx_equal(I.data[2], 0.0));
        TEST_ASSERT(approx_equal(I.data[3], 1.0));
        mat_free(&I);
}

TEST_CASE(test_mat_identity_3x3)
{
        Matrix I = mat_identity(3);
        TEST_ASSERT(I.rows == 3);
        TEST_ASSERT(I.cols == 3);
        TEST_ASSERT(approx_equal(I.data[0], 1.0));
        TEST_ASSERT(approx_equal(I.data[4], 1.0));
        TEST_ASSERT(approx_equal(I.data[8], 1.0));
        TEST_ASSERT(approx_equal(I.data[1], 0.0));
        TEST_ASSERT(approx_equal(I.data[2], 0.0));
        TEST_ASSERT(approx_equal(I.data[3], 0.0));
        TEST_ASSERT(approx_equal(I.data[5], 0.0));
        TEST_ASSERT(approx_equal(I.data[6], 0.0));
        TEST_ASSERT(approx_equal(I.data[7], 0.0));
        mat_free(&I);
}

TEST_CASE(test_mat_identity_zero_size)
{
        Matrix I = mat_identity(0);
        TEST_ASSERT(I.rows == 0);
        TEST_ASSERT(I.cols == 0);
        TEST_ASSERT(I.data == NULL);
        mat_free(&I);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_create tests ===\n\n");

        run_test(test_mat_create_zero, "test_mat_create_zero");
        run_test(test_mat_create_nonzero, "test_mat_create_nonzero");
        run_test(test_mat_create_zero_dimensions,
                 "test_mat_create_zero_dimensions");
        run_test(test_mat_create_allocation_failure,
                 "test_mat_create_allocation_failure");
        run_test(test_mat_identity_1x1, "test_mat_identity_1x1");
        run_test(test_mat_identity_2x2, "test_mat_identity_2x2");
        run_test(test_mat_identity_3x3, "test_mat_identity_3x3");
        run_test(test_mat_identity_zero_size, "test_mat_identity_zero_size");

        fprintf(stdout, "\n=== All mat_create tests passed ===\n\n");
        return EXIT_SUCCESS;
}
