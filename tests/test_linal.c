/*
 * @file test_linal.c
 * @brief Unit tests for LINAL library.
 */

#include "linal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(expr)                                                      \
        do {                                                                   \
                if (!(expr)) {                                                 \
                        fprintf(stderr, "FAIL  %s:%d  %s\n", __FILE__,         \
                                __LINE__, #expr);                              \
                        exit(EXIT_FAILURE);                                    \
                }                                                              \
        } while (0)

#define TEST_PASS(name) fprintf(stdout, "PASS  %s\n", (name))

#define TEST_CASE(name)                                                        \
        static void name(void);                                                \
        static void name(void)

#define EPSILON 1e-10

/**
 * @brief Compare two doubles with tolerance.
 * @param a First value
 * @param b Second value
 * @return true if values are within tolerance
 */
static inline int
approx_equal(double a, double b)
{
        return fabs(a - b) < EPSILON;
}

/**
 * @brief Compare two matrices for equality.
 * @param a First matrix
 * @param b Second matrix
 * @return 0 if equal, -1 if not
 */
static int
mat_equal(const Matrix a, const Matrix b)
{
        if (a.rows != b.rows || a.cols != b.cols) {
                return -1;
        }

        for (size_t i = 0; i < a.rows * a.cols; i++) {
                if (!approx_equal(a.data[i], b.data[i])) {
                        return -1;
                }
        }

        return 0;
}

/**
 * @brief Initialize matrix with given values.
 * @param m Matrix to initialize
 * @param rows Number of rows
 * @param cols Number of columns
 * @param values Array of values to set
 */
static void
init_matrix(Matrix *m, size_t rows, size_t cols, const double *values)
{
        TEST_ASSERT(m->data != NULL);
        for (size_t i = 0; i < rows * cols; i++) {
                m->data[i] = values[i];
        }
}

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
        Matrix m = mat_create(1000000, 1000000);
        TEST_ASSERT(m.data == NULL || m.rows == 0 || m.cols == 0);
        mat_free(&m);
}

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

TEST_CASE(test_mat_add_success)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == 0);
        TEST_ASSERT(result.rows == 2);
        TEST_ASSERT(result.cols == 2);

        TEST_ASSERT(approx_equal(result.data[0], 6.0));
        TEST_ASSERT(approx_equal(result.data[1], 8.0));
        TEST_ASSERT(approx_equal(result.data[2], 10.0));
        TEST_ASSERT(approx_equal(result.data[3], 12.0));

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_add_dimension_mismatch)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 3);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_add_null_result)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);
        result.data = NULL;

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_add_null_input_a)
{
        Matrix a = mat_create(2, 2);
        a.data = NULL;
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_add_null_input_b)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        b.data = NULL;
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_add_zero_matrices)
{
        Matrix a = mat_create(3, 3);
        Matrix b = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        int result_code = mat_add(a, b, &result);

        TEST_ASSERT(result_code == 0);
        for (size_t i = 0; i < 9; i++) {
                TEST_ASSERT(result.data[i] == 0.0);
        }

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_success_square)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == 0);

        /* [1 2] * [5 6] = [19 22] */
        /* [3 4]   [7 8]   [43 50] */
        TEST_ASSERT(approx_equal(result.data[0], 19.0));
        TEST_ASSERT(approx_equal(result.data[1], 22.0));
        TEST_ASSERT(approx_equal(result.data[2], 43.0));
        TEST_ASSERT(approx_equal(result.data[3], 50.0));

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_success_rectangular)
{
        Matrix a = mat_create(2, 3);
        Matrix b = mat_create(3, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
        init_matrix(&b, 3, 2, (double[]){7.0, 8.0, 9.0, 10.0, 11.0, 12.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == 0);

        TEST_ASSERT(approx_equal(result.data[0], 58.0));
        TEST_ASSERT(approx_equal(result.data[1], 64.0));
        TEST_ASSERT(approx_equal(result.data[2], 139.0));
        TEST_ASSERT(approx_equal(result.data[3], 154.0));

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_identity)
{
        Matrix a = mat_create(3, 3);
        Matrix b = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        /* Create identity matrix A */
        init_matrix(&a, 3, 3,
                    (double[]){1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});
        /* Create identity matrix B */
        init_matrix(&b, 3, 3,
                    (double[]){1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == 0);

        /* A * I = A, so result should be identity */
        for (size_t i = 0; i < 9; i++) {
                if (i % 3 == i / 3) {
                        TEST_ASSERT(approx_equal(result.data[i], 1.0));
                } else {
                        TEST_ASSERT(approx_equal(result.data[i], 0.0));
                }
        }

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_dimension_mismatch)
{
        Matrix a = mat_create(2, 3);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
        init_matrix(&b, 2, 2, (double[]){7.0, 8.0, 9.0, 10.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_empty_matrix)
{
        Matrix a = mat_create(0, 3);
        Matrix b = mat_create(3, 2);
        Matrix result = mat_create(0, 2);

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_null_result)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);
        result.data = NULL;

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_null_input_a)
{
        Matrix a = mat_create(2, 2);
        a.data = NULL;
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_null_input_b)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        b.data = NULL;
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_success)
{
        Matrix m = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 3, 3,
                    (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == 0);

        TEST_ASSERT(approx_equal(result.data[0], 2.0));
        TEST_ASSERT(approx_equal(result.data[1], 4.0));
        TEST_ASSERT(approx_equal(result.data[2], 6.0));
        TEST_ASSERT(approx_equal(result.data[3], 8.0));
        TEST_ASSERT(approx_equal(result.data[4], 10.0));
        TEST_ASSERT(approx_equal(result.data[5], 12.0));
        TEST_ASSERT(approx_equal(result.data[6], 14.0));
        TEST_ASSERT(approx_equal(result.data[7], 16.0));
        TEST_ASSERT(approx_equal(result.data[8], 18.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_zero)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 0.0, &result);

        TEST_ASSERT(result_code == 0);
        for (size_t i = 0; i < 4; i++) {
                TEST_ASSERT(result.data[i] == 0.0);
        }

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_negative)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, -1.5, &result);

        TEST_ASSERT(result_code == 0);
        TEST_ASSERT(approx_equal(result.data[0], -1.5));
        TEST_ASSERT(approx_equal(result.data[1], -3.0));
        TEST_ASSERT(approx_equal(result.data[2], -4.5));
        TEST_ASSERT(approx_equal(result.data[3], -6.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_null_input)
{
        Matrix m = mat_create(2, 2);
        m.data = NULL;
        Matrix result = mat_create(2, 2);

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_null_result)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);
        result.data = NULL;

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_null_result_ptr)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 2.0, NULL);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_dimension_mismatch)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_success_square)
{
        Matrix m = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 3, 3,
                    (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == 0);

        /* Transpose of [1 2 3] is [1 4 7] */
        /*              [4 5 6]   [2 5 8] */
        /*              [7 8 9]   [3 6 9] */
        TEST_ASSERT(approx_equal(result.data[0], 1.0));
        TEST_ASSERT(approx_equal(result.data[1], 4.0));
        TEST_ASSERT(approx_equal(result.data[2], 7.0));
        TEST_ASSERT(approx_equal(result.data[3], 2.0));
        TEST_ASSERT(approx_equal(result.data[4], 5.0));
        TEST_ASSERT(approx_equal(result.data[5], 8.0));
        TEST_ASSERT(approx_equal(result.data[6], 3.0));
        TEST_ASSERT(approx_equal(result.data[7], 6.0));
        TEST_ASSERT(approx_equal(result.data[8], 9.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_success_rectangular)
{
        Matrix m = mat_create(2, 3);
        Matrix result = mat_create(3, 2);

        init_matrix(&m, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == 0);

        /* Transpose of [1 2 3] is [1 4] */
        /*              [4 5 6]   [2 5] */
        /*                         [3 6] */
        TEST_ASSERT(approx_equal(result.data[0], 1.0));
        TEST_ASSERT(approx_equal(result.data[1], 4.0));
        TEST_ASSERT(approx_equal(result.data[2], 2.0));
        TEST_ASSERT(approx_equal(result.data[3], 5.0));
        TEST_ASSERT(approx_equal(result.data[4], 3.0));
        TEST_ASSERT(approx_equal(result.data[5], 6.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_null_input)
{
        Matrix m = mat_create(2, 2);
        m.data = NULL;
        Matrix result = mat_create(2, 2);

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_null_result)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);
        result.data = NULL;

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_null_result_ptr)
{
        Matrix m = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_transpose(m, NULL);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
}

TEST_CASE(test_mat_transpose_dimension_mismatch)
{
        Matrix m = mat_create(2, 3);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_identity)
{
        Matrix m = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 3, 3,
                    (double[]){1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});

        int result_code = mat_transpose(m, &result);

        TEST_ASSERT(result_code == 0);

        /* Transpose of identity is identity */
        TEST_ASSERT(mat_equal(m, result) == 0);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_transpose_double_transpose)
{
        Matrix m = mat_create(2, 3);
        Matrix t = mat_create(3, 2);
        Matrix tt = mat_create(2, 3);

        init_matrix(&m, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

        mat_transpose(m, &t);
        mat_transpose(t, &tt);

        TEST_ASSERT(mat_equal(m, tt) == 0);

        mat_free(&m);
        mat_free(&t);
        mat_free(&tt);
}

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

TEST_CASE(test_mat_chain_operations)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix c = mat_create(2, 2);
        Matrix d = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        mat_add(a, b, &c);
        mat_mul(c, c, &d);

        mat_free(&a);
        mat_free(&b);
        mat_free(&c);
        mat_free(&d);
}

static void
run_test(void (*test_func)(void), const char *name)
{
        test_func();
        TEST_PASS(name);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running linal unit tests ===\n\n");

        run_test(test_mat_create_zero, "test_mat_create_zero");
        run_test(test_mat_create_nonzero, "test_mat_create_nonzero");
        run_test(test_mat_create_zero_dimensions,
                 "test_mat_create_zero_dimensions");
        run_test(test_mat_create_allocation_failure,
                 "test_mat_create_allocation_failure");
        run_test(test_mat_free_null, "test_mat_free_null");
        run_test(test_mat_free_invalid, "test_mat_free_invalid");
        run_test(test_mat_free_cleanup, "test_mat_free_cleanup");
        run_test(test_mat_copy_success, "test_mat_copy_success");
        run_test(test_mat_copy_dimension_mismatch,
                 "test_mat_copy_dimension_mismatch");
        run_test(test_mat_copy_null_dest, "test_mat_copy_null_dest");
        run_test(test_mat_add_success, "test_mat_add_success");
        run_test(test_mat_add_dimension_mismatch,
                 "test_mat_add_dimension_mismatch");
        run_test(test_mat_add_null_result, "test_mat_add_null_result");
        run_test(test_mat_add_null_input_a, "test_mat_add_null_input_a");
        run_test(test_mat_add_null_input_b, "test_mat_add_null_input_b");
        run_test(test_mat_add_zero_matrices, "test_mat_add_zero_matrices");
        run_test(test_mat_mul_success_square, "test_mat_mul_success_square");
        run_test(test_mat_mul_success_rectangular,
                 "test_mat_mul_success_rectangular");
        run_test(test_mat_mul_identity, "test_mat_mul_identity");
        run_test(test_mat_mul_dimension_mismatch,
                 "test_mat_mul_dimension_mismatch");
        run_test(test_mat_mul_empty_matrix, "test_mat_mul_empty_matrix");
        run_test(test_mat_mul_null_result, "test_mat_mul_null_result");
        run_test(test_mat_mul_null_input_a, "test_mat_mul_null_input_a");
        run_test(test_mat_mul_null_input_b, "test_mat_mul_null_input_b");
        run_test(test_mat_scale_success, "test_mat_scale_success");
        run_test(test_mat_scale_zero, "test_mat_scale_zero");
        run_test(test_mat_scale_negative, "test_mat_scale_negative");
        run_test(test_mat_scale_null_input, "test_mat_scale_null_input");
        run_test(test_mat_scale_null_result, "test_mat_scale_null_result");
        run_test(test_mat_scale_null_result_ptr,
                 "test_mat_scale_null_result_ptr");
        run_test(test_mat_scale_dimension_mismatch,
                 "test_mat_scale_dimension_mismatch");
        run_test(test_mat_transpose_success_square,
                 "test_mat_transpose_success_square");
        run_test(test_mat_transpose_success_rectangular,
                 "test_mat_transpose_success_rectangular");
        run_test(test_mat_transpose_null_input, "test_mat_transpose_null_input");
        run_test(test_mat_transpose_null_result,
                 "test_mat_transpose_null_result");
        run_test(test_mat_transpose_null_result_ptr,
                 "test_mat_transpose_null_result_ptr");
        run_test(test_mat_transpose_dimension_mismatch,
                 "test_mat_transpose_dimension_mismatch");
        run_test(test_mat_transpose_identity, "test_mat_transpose_identity");
        run_test(test_mat_transpose_double_transpose,
                 "test_mat_transpose_double_transpose");
        run_test(test_mat_print, "test_mat_print");
        run_test(test_mat_print_no_label, "test_mat_print_no_label");
        run_test(test_mat_print_empty, "test_mat_print_empty");
        run_test(test_mat_chain_operations, "test_mat_chain_operations");

        fprintf(stdout, "\n=== All tests passed ===\n\n");
        return EXIT_SUCCESS;
}
