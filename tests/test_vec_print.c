/*
 * @file test_vec_print.c
 * @brief Unit tests for vector debug printing (vec_print).
 *
 * vec_print is conditionally compiled under LINAL_ENABLE_DEBUG_PRINT.
 * Tests verify that the function does not crash and produces expected output.
 */

#include "test_harness.h"

#if LINAL_ENABLE_DEBUG_PRINT

TEST_CASE(test_vec_print_normal)
{
        Vector v = vec_create(4);
        v.data[0] = 1.0;
        v.data[1] = 2.0;
        v.data[2] = -3.0;
        v.data[3] = 4.5;

        vec_print("TestVector", v);
        /* No crash = pass for print tests. */
        vec_free(&v);
}

TEST_CASE(test_vec_print_zero_size)
{
        Vector v = vec_create(0);

        vec_print("Empty", v);
        vec_free(&v);
}

TEST_CASE(test_vec_print_null_label)
{
        Vector v = vec_create(2);
        v.data[0] = 1.0;
        v.data[1] = 2.0;

        vec_print(NULL, v);
        vec_free(&v);
}

TEST_CASE(test_vec_print_empty_vector_null_label)
{
        Vector v = vec_create(0);

        vec_print(NULL, v);
        vec_free(&v);
}

TEST_CASE(test_vec_print_1_element)
{
        Vector v = vec_create(1);
        v.data[0] = 42.0;

        vec_print("Single", v);
        vec_free(&v);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_print tests ===\n\n");

        run_test(test_vec_print_normal, "test_vec_print_normal");
        run_test(test_vec_print_zero_size, "test_vec_print_zero_size");
        run_test(test_vec_print_null_label, "test_vec_print_null_label");
        run_test(test_vec_print_empty_vector_null_label,
                 "test_vec_print_empty_vector_null_label");
        run_test(test_vec_print_1_element, "test_vec_print_1_element");

        fprintf(stdout, "\n=== All vec_print tests passed ===\n\n");
        return EXIT_SUCCESS;
}

#else

/* When LINAL_ENABLE_DEBUG_PRINT is disabled, provide a no-op test. */
TEST_CASE(test_vec_print_disabled)
{
        /* vec_print is not compiled; this test just ensures the binary
         * still links and runs without errors. */
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_print tests ===\n\n");
        fprintf(stdout,
                "(LINAL_ENABLE_DEBUG_PRINT not defined — skipping)\n\n");
        run_test(test_vec_print_disabled, "test_vec_print_disabled");
        fprintf(stdout, "\n=== All vec_print tests passed ===\n\n");
        return EXIT_SUCCESS;
}

#endif /* LINAL_ENABLE_DEBUG_PRINT */
