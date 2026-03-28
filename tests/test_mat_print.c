/*
 * @file test_mat_print.c
 * @brief Unit tests for matrix printing.
 */

#include "test_harness.h"

TEST_CASE(test_mat_print)
{
        Matrix m = mat_create(2, 3);

        init_matrix(&m, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        mat_print("Test matrix:", m);

        mat_free(&m);
}

TEST_CASE(test_mat_print_no_label)
{
        Matrix m = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        mat_print(NULL, m);

        mat_free(&m);
}

TEST_CASE(test_mat_print_empty)
{
        Matrix m = mat_create(0, 0);

        mat_print("Empty matrix:", m);

        mat_free(&m);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_print tests ===\n\n");

        run_test(test_mat_print, "test_mat_print");
        run_test(test_mat_print_no_label, "test_mat_print_no_label");
        run_test(test_mat_print_empty, "test_mat_print_empty");

        fprintf(stdout, "\n=== All mat_print tests passed ===\n\n");
        return EXIT_SUCCESS;
}
