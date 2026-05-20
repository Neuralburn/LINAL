/*
 * @file test_mat_vec.c
 * @brief Unit tests for matrix-vector operations.
 */

#include "test_harness.h"

TEST_CASE(test_mat_vec_mul_simple)
{
        Matrix m = mat_create(2, 3);
        Vector v = vec_create(3);
        Vector res = vec_create(2);

        /* m = [1, 2, 3]
         *     [4, 5, 6] */
        m.data[0] = 1.0; m.data[1] = 2.0; m.data[2] = 3.0;
        m.data[3] = 4.0; m.data[4] = 5.0; m.data[5] = 6.0;

        /* v = [7, 8, 9] */
        v.data[0] = 7.0; v.data[1] = 8.0; v.data[2] = 9.0;

        /* res = m * v = [1*7 + 2*8 + 3*9, 4*7 + 5*8 + 6*9]
         *              = [7 + 16 + 27, 28 + 40 + 54]
         *              = [50, 122] */

        TEST_ASSERT(mat_vec_mul(m, v, &res) == 0);
        TEST_ASSERT(approx_equal(res.data[0], 50.0));
        TEST_ASSERT(approx_equal(res.data[1], 122.0));

        mat_free(&m);
        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_vec_mat_mul_simple)
{
        Vector v = vec_create(2);
        Matrix m = mat_create(2, 3);
        Vector res = vec_create(3);

        /* v = [1, 2] */
        v.data[0] = 1.0; v.data[1] = 2.0;

        /* m = [1, 2, 3]
         *     [4, 5, 6] */
        m.data[0] = 1.0; m.data[1] = 2.0; m.data[2] = 3.0;
        m.data[3] = 4.0; m.data[4] = 5.0; m.data[5] = 6.0;

        /* res = v^T * m = [1*1 + 2*4, 1*2 + 2*5, 1*3 + 2*6]
         *                = [1 + 8, 2 + 10, 3 + 12]
         *                = [9, 12, 15] */

        TEST_ASSERT(vec_mat_mul(v, m, &res) == 0);
        TEST_ASSERT(approx_equal(res.data[0], 9.0));
        TEST_ASSERT(approx_equal(res.data[1], 12.0));
        TEST_ASSERT(approx_equal(res.data[2], 15.0));

        vec_free(&v);
        mat_free(&m);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_add_simple)
{
        Matrix m = mat_create(2, 2);
        Vector v = vec_create(2);
        Vector b = vec_create(2);
        Vector res = vec_create(2);

        /* m = [1, 2]
         *     [3, 4] */
        m.data[0] = 1.0; m.data[1] = 2.0;
        m.data[2] = 3.0; m.data[3] = 4.0;

        /* v = [5, 6] */
        v.data[0] = 5.0; v.data[1] = 6.0;

        /* b = [7, 8] */
        b.data[0] = 7.0; b.data[1] = 8.0;

        /* res = m * v + b = [1*5 + 2*6 + 7, 3*5 + 4*6 + 8]
         *                = [5 + 12 + 7, 15 + 24 + 8]
         *                = [24, 47] */

        TEST_ASSERT(mat_vec_add(m, v, &b, &res) == 0);
        TEST_ASSERT(approx_equal(res.data[0], 24.0));
        TEST_ASSERT(approx_equal(res.data[1], 47.0));

        mat_free(&m);
        vec_free(&v);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_mul_mismatch)
{
        Matrix m = mat_create(2, 2);
        Vector v = vec_create(3);
        Vector res = vec_create(2);

        TEST_ASSERT(mat_vec_mul(m, v, &res) == -1);

        mat_free(&m);
        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_mul_null_result)
{
        Matrix m = mat_create(2, 2);
        Vector v = vec_create(2);
        Vector res = {.size = 2, .data = NULL};

        m.data[0] = 1.0; m.data[1] = 2.0;
        m.data[2] = 3.0; m.data[3] = 4.0;
        v.data[0] = 5.0; v.data[1] = 6.0;

        TEST_ASSERT(mat_vec_mul(m, v, &res) == -1);

        mat_free(&m);
        vec_free(&v);
}

TEST_CASE(test_mat_vec_mul_null_matrix)
{
        Matrix m = {.rows = 2, .cols = 2, .data = NULL};
        Vector v = vec_create(2);
        Vector res = vec_create(2);

        v.data[0] = 5.0; v.data[1] = 6.0;

        TEST_ASSERT(mat_vec_mul(m, v, &res) == -1);

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_mul_null_vector)
{
        Matrix m = mat_create(2, 2);
        Vector v = {.size = 2, .data = NULL};
        Vector res = vec_create(2);

        m.data[0] = 1.0; m.data[1] = 2.0;
        m.data[2] = 3.0; m.data[3] = 4.0;

        TEST_ASSERT(mat_vec_mul(m, v, &res) == -1);

        mat_free(&m);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_mul_zero_size)
{
        /* Zero-size matrices/vectors have data == NULL, which is rejected. */
        Matrix m = mat_create(0, 0);
        Vector v = vec_create(0);
        Vector res = vec_create(0);

        TEST_ASSERT(mat_vec_mul(m, v, &res) == -1);

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_mul_1x1)
{
        Matrix m = mat_create(1, 1);
        Vector v = vec_create(1);
        Vector res = vec_create(1);

        m.data[0] = 3.0;
        v.data[0] = 4.0;

        TEST_ASSERT(mat_vec_mul(m, v, &res) == 0);
        TEST_ASSERT(approx_equal(res.data[0], 12.0));

        mat_free(&m);
        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_vec_mat_mul_mismatch)
{
        Vector v = vec_create(3);
        Matrix m = mat_create(2, 2);
        Vector res = vec_create(2);

        TEST_ASSERT(vec_mat_mul(v, m, &res) == -1);

        vec_free(&v);
        mat_free(&m);
        vec_free(&res);
}

TEST_CASE(test_vec_mat_mul_null_result)
{
        Vector v = vec_create(2);
        Matrix m = mat_create(2, 2);
        Vector res = {.size = 2, .data = NULL};

        v.data[0] = 1.0; v.data[1] = 2.0;
        m.data[0] = 3.0; m.data[1] = 4.0;
        m.data[2] = 5.0; m.data[3] = 6.0;

        TEST_ASSERT(vec_mat_mul(v, m, &res) == -1);

        vec_free(&v);
        mat_free(&m);
}

TEST_CASE(test_vec_mat_mul_null_matrix)
{
        Vector v = vec_create(2);
        Matrix m = {.rows = 2, .cols = 2, .data = NULL};
        Vector res = vec_create(2);

        v.data[0] = 1.0; v.data[1] = 2.0;

        TEST_ASSERT(vec_mat_mul(v, m, &res) == -1);

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_vec_mat_mul_null_vector)
{
        Vector v = {.size = 2, .data = NULL};
        Matrix m = mat_create(2, 2);
        Vector res = vec_create(2);

        m.data[0] = 3.0; m.data[1] = 4.0;
        m.data[2] = 5.0; m.data[3] = 6.0;

        TEST_ASSERT(vec_mat_mul(v, m, &res) == -1);

        mat_free(&m);
        vec_free(&res);
}

TEST_CASE(test_vec_mat_mul_zero_size)
{
        /* Zero-size matrices/vectors have data == NULL, which is rejected. */
        Vector v = vec_create(0);
        Matrix m = mat_create(0, 0);
        Vector res = vec_create(0);

        TEST_ASSERT(vec_mat_mul(v, m, &res) == -1);

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_add_mismatch)
{
        Matrix m = mat_create(2, 2);
        Vector v = vec_create(2);
        Vector b = vec_create(3);
        Vector res = vec_create(2);

        TEST_ASSERT(mat_vec_add(m, v, &b, &res) == -1);

        mat_free(&m);
        vec_free(&v);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_add_null_result)
{
        Matrix m = mat_create(2, 2);
        Vector v = vec_create(2);
        Vector b = vec_create(2);
        Vector res = {.size = 2, .data = NULL};

        m.data[0] = 1.0; m.data[1] = 2.0;
        m.data[2] = 3.0; m.data[3] = 4.0;
        v.data[0] = 5.0; v.data[1] = 6.0;
        b.data[0] = 7.0; b.data[1] = 8.0;

        TEST_ASSERT(mat_vec_add(m, v, &b, &res) == -1);

        mat_free(&m);
        vec_free(&v);
        vec_free(&b);
}

TEST_CASE(test_mat_vec_add_null_matrix)
{
        Matrix m = {.rows = 2, .cols = 2, .data = NULL};
        Vector v = vec_create(2);
        Vector b = vec_create(2);
        Vector res = vec_create(2);

        v.data[0] = 5.0; v.data[1] = 6.0;
        b.data[0] = 7.0; b.data[1] = 8.0;

        TEST_ASSERT(mat_vec_add(m, v, &b, &res) == -1);

        vec_free(&v);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_add_null_vector)
{
        Matrix m = mat_create(2, 2);
        Vector v = {.size = 2, .data = NULL};
        Vector b = vec_create(2);
        Vector res = vec_create(2);

        m.data[0] = 1.0; m.data[1] = 2.0;
        m.data[2] = 3.0; m.data[3] = 4.0;
        b.data[0] = 7.0; b.data[1] = 8.0;

        TEST_ASSERT(mat_vec_add(m, v, &b, &res) == -1);

        mat_free(&m);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_add_null_bias)
{
        Matrix m = mat_create(2, 2);
        Vector v = vec_create(2);
        Vector b = {.size = 2, .data = NULL};
        Vector res = vec_create(2);

        m.data[0] = 1.0; m.data[1] = 2.0;
        m.data[2] = 3.0; m.data[3] = 4.0;
        v.data[0] = 5.0; v.data[1] = 6.0;

        TEST_ASSERT(mat_vec_add(m, v, &b, &res) == -1);

        mat_free(&m);
        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_add_zero_size)
{
        /* Zero-size matrices/vectors have data == NULL, which is rejected. */
        Matrix m = mat_create(0, 0);
        Vector v = vec_create(0);
        Vector b = vec_create(0);
        Vector res = vec_create(0);

        TEST_ASSERT(mat_vec_add(m, v, &b, &res) == -1);

        vec_free(&v);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_add_alias_result_eq_b)
{
        Matrix m = mat_create(2, 2);
        Vector v = vec_create(2);
        Vector b = vec_create(2);
        Vector res = vec_create(2);

        m.data[0] = 1.0; m.data[1] = 2.0;
        m.data[2] = 3.0; m.data[3] = 4.0;
        v.data[0] = 5.0; v.data[1] = 6.0;
        b.data[0] = 7.0; b.data[1] = 8.0;

        /* Attempt to alias result with b's storage. */
        Vector aliased_res = {.size = 2, .data = b.data};
        TEST_ASSERT(mat_vec_add(m, v, &b, &aliased_res) == -1);

        /* b must be untouched after rejection. */
        TEST_ASSERT(approx_equal(b.data[0], 7.0));

        mat_free(&m);
        vec_free(&v);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_mul_alias)
{
        Matrix m = mat_create(2, 2);
        Vector v = vec_create(2);
        Vector res = vec_create(2);

        m.data[0] = 1.0; m.data[1] = 2.0;
        m.data[2] = 3.0; m.data[3] = 4.0;
        v.data[0] = 5.0; v.data[1] = 6.0;

        /* Attempt to alias result with v's storage. */
        Vector aliased_res = {.size = 2, .data = v.data};
        TEST_ASSERT(mat_vec_mul(m, v, &aliased_res) == -1);

        /* v must be untouched after rejection. */
        TEST_ASSERT(approx_equal(v.data[0], 5.0));

        mat_free(&m);
        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_mat_vec_mul_round_trip)
{
        Matrix m = mat_create(2, 3);
        Vector v = vec_create(3);
        Vector prod = vec_create(2);

        /* m = [1, 0, 0]
         *     [0, 1, 0] */
        m.data[0] = 1.0; m.data[1] = 0.0; m.data[2] = 0.0;
        m.data[3] = 0.0; m.data[4] = 1.0; m.data[5] = 0.0;

        /* v = [5, 6, 7] */
        v.data[0] = 5.0; v.data[1] = 6.0; v.data[2] = 7.0;

        /* prod = m * v = [5, 6] */
        TEST_ASSERT(mat_vec_mul(m, v, &prod) == 0);
        TEST_ASSERT(approx_equal(prod.data[0], 5.0));
        TEST_ASSERT(approx_equal(prod.data[1], 6.0));

        mat_free(&m);
        vec_free(&v);
        vec_free(&prod);
}

TEST_CASE(test_vec_mat_mul_round_trip)
{
        Vector v = vec_create(2);
        Matrix m = mat_create(2, 3);
        Vector prod = vec_create(3);

        /* v = [1, 2] */
        v.data[0] = 1.0; v.data[1] = 2.0;

        /* m = [1, 0, 0]
         *     [0, 1, 0] */
        m.data[0] = 1.0; m.data[1] = 0.0; m.data[2] = 0.0;
        m.data[3] = 0.0; m.data[4] = 1.0; m.data[5] = 0.0;

        /* prod = v^T * m = [1, 2, 0] */
        TEST_ASSERT(vec_mat_mul(v, m, &prod) == 0);
        TEST_ASSERT(approx_equal(prod.data[0], 1.0));
        TEST_ASSERT(approx_equal(prod.data[1], 2.0));
        TEST_ASSERT(approx_equal(prod.data[2], 0.0));

        vec_free(&v);
        mat_free(&m);
        vec_free(&prod);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_vec tests ===\n\n");

        run_test(test_mat_vec_mul_simple, "test_mat_vec_mul_simple");
        run_test(test_vec_mat_mul_simple, "test_vec_mat_mul_simple");
        run_test(test_mat_vec_add_simple, "test_mat_vec_add_simple");
        run_test(test_mat_vec_mul_mismatch, "test_mat_vec_mul_mismatch");
        run_test(test_mat_vec_mul_null_result, "test_mat_vec_mul_null_result");
        run_test(test_mat_vec_mul_null_matrix, "test_mat_vec_mul_null_matrix");
        run_test(test_mat_vec_mul_null_vector, "test_mat_vec_mul_null_vector");
        run_test(test_mat_vec_mul_zero_size, "test_mat_vec_mul_zero_size");
        run_test(test_mat_vec_mul_1x1, "test_mat_vec_mul_1x1");
        run_test(test_vec_mat_mul_mismatch, "test_vec_mat_mul_mismatch");
        run_test(test_vec_mat_mul_null_result, "test_vec_mat_mul_null_result");
        run_test(test_vec_mat_mul_null_matrix, "test_vec_mat_mul_null_matrix");
        run_test(test_vec_mat_mul_null_vector, "test_vec_mat_mul_null_vector");
        run_test(test_vec_mat_mul_zero_size, "test_vec_mat_mul_zero_size");
        run_test(test_mat_vec_add_mismatch, "test_mat_vec_add_mismatch");
        run_test(test_mat_vec_add_null_result, "test_mat_vec_add_null_result");
        run_test(test_mat_vec_add_null_matrix, "test_mat_vec_add_null_matrix");
        run_test(test_mat_vec_add_null_vector, "test_mat_vec_add_null_vector");
        run_test(test_mat_vec_add_null_bias, "test_mat_vec_add_null_bias");
        run_test(test_mat_vec_add_zero_size, "test_mat_vec_add_zero_size");
        run_test(test_mat_vec_add_alias_result_eq_b,
                 "test_mat_vec_add_alias_result_eq_b");
        run_test(test_mat_vec_mul_alias, "test_mat_vec_mul_alias");
        run_test(test_mat_vec_mul_round_trip, "test_mat_vec_mul_round_trip");
        run_test(test_vec_mat_mul_round_trip, "test_vec_mat_mul_round_trip");

        fprintf(stdout, "\n=== All mat_vec tests passed ===\n\n");
        return EXIT_SUCCESS;
}
