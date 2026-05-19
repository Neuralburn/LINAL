/*
 * @file test_vec_dot.c
 * @brief Unit tests for vector dot product.
 */

#include "test_harness.h"

TEST_CASE(test_vec_dot_simple)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);

        a.data[0] = 1.0;
        a.data[1] = 2.0;
        a.data[2] = 3.0;
        b.data[0] = 4.0;
        b.data[1] = 5.0;
        b.data[2] = 6.0;

        TEST_ASSERT(approx_equal(vec_dot(a, b), 32.0));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_dot_mismatch)
{
        Vector a = vec_create(3);
        Vector b = vec_create(2);

        TEST_ASSERT(isnan(vec_dot(a, b)));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_dot_zero_vectors)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);

        TEST_ASSERT(approx_equal(vec_dot(a, b), 0.0));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_dot_1_element)
{
        Vector a = vec_create(1);
        Vector b = vec_create(1);

        a.data[0] = 7.0;
        b.data[0] = -3.0;

        TEST_ASSERT(approx_equal(vec_dot(a, b), -21.0));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_dot_self_eq_norm_squared)
{
        Vector v = vec_create(3);

        v.data[0] = 3.0;
        v.data[1] = 4.0;
        v.data[2] = 0.0;

        TEST_ASSERT(approx_equal(vec_dot(v, v), 25.0));
        TEST_ASSERT(
            approx_equal(vec_dot(v, v), vec_norm_l2(v) * vec_norm_l2(v)));

        vec_free(&v);
}

TEST_CASE(test_vec_dot_commutative)
{
        Vector a = vec_create(4);
        Vector b = vec_create(4);

        a.data[0] = 1.0;
        a.data[1] = -2.0;
        a.data[2] = 3.0;
        a.data[3] = -4.0;
        b.data[0] = 5.0;
        b.data[1] = 6.0;
        b.data[2] = -7.0;
        b.data[3] = 8.0;

        TEST_ASSERT(approx_equal(vec_dot(a, b), vec_dot(b, a)));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_dot_null_data_a)
{
        Vector a = {.size = 3, .data = NULL};
        Vector b = vec_create(3);

        TEST_ASSERT(isnan(vec_dot(a, b)));

        vec_free(&b);
}

TEST_CASE(test_vec_dot_null_data_b)
{
        Vector a = vec_create(3);
        Vector b = {.size = 3, .data = NULL};

        TEST_ASSERT(isnan(vec_dot(a, b)));

        vec_free(&a);
}

TEST_CASE(test_vec_dot_zero_size)
{
        /* Zero-size vectors have NULL data; vec_dot returns NAN for NULL. */
        Vector a = vec_create(0);
        Vector b = vec_create(0);

        TEST_ASSERT(isnan(vec_dot(a, b)));
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_dot tests ===\n\n");

        run_test(test_vec_dot_simple, "test_vec_dot_simple");
        run_test(test_vec_dot_mismatch, "test_vec_dot_mismatch");
        run_test(test_vec_dot_zero_vectors, "test_vec_dot_zero_vectors");
        run_test(test_vec_dot_1_element, "test_vec_dot_1_element");
        run_test(test_vec_dot_self_eq_norm_squared,
                 "test_vec_dot_self_eq_norm_squared");
        run_test(test_vec_dot_commutative, "test_vec_dot_commutative");
        run_test(test_vec_dot_null_data_a, "test_vec_dot_null_data_a");
        run_test(test_vec_dot_null_data_b, "test_vec_dot_null_data_b");
        run_test(test_vec_dot_zero_size, "test_vec_dot_zero_size");

        fprintf(stdout, "\n=== All vec_dot tests passed ===\n\n");
        return EXIT_SUCCESS;
}
