/*
 * @file test_vec_scale.c
 * @brief Unit tests for vector scaling.
 */

#include "test_harness.h"

TEST_CASE(test_vec_scale_simple)
{
        Vector v = vec_create(3);
        Vector res = vec_create(3);

        v.data[0] = 1.0;
        v.data[1] = 2.0;
        v.data[2] = 3.0;

        TEST_ASSERT(vec_scale(v, 2.5, &res) == 0);
        TEST_ASSERT(approx_equal(res.data[0], 2.5));
        TEST_ASSERT(approx_equal(res.data[1], 5.0));
        TEST_ASSERT(approx_equal(res.data[2], 7.5));

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_vec_scale_mismatch)
{
        Vector v = vec_create(3);
        Vector res = vec_create(2);

        TEST_ASSERT(vec_scale(v, 2.0, &res) == -1);

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_vec_scale_null_result)
{
        Vector v = vec_create(3);
        Vector res = {.size = 3, .data = NULL};

        TEST_ASSERT(vec_scale(v, 2.0, &res) == -1);

        vec_free(&v);
}

TEST_CASE(test_vec_scale_null_input)
{
        Vector v = {.size = 3, .data = NULL};
        Vector res = vec_create(3);

        TEST_ASSERT(vec_scale(v, 2.0, &res) == -1);

        vec_free(&res);
}

TEST_CASE(test_vec_scale_identity)
{
        Vector v = vec_create(3);
        Vector res = vec_create(3);

        v.data[0] = 1.0;
        v.data[1] = -2.0;
        v.data[2] = 3.5;

        TEST_ASSERT(vec_scale(v, 1.0, &res) == 0);
        for (size_t i = 0; i < 3; i++) {
                TEST_ASSERT(approx_equal(res.data[i], v.data[i]));
        }

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_vec_scale_neg_one)
{
        Vector v = vec_create(3);
        Vector res = vec_create(3);

        v.data[0] = 1.0;
        v.data[1] = 2.0;
        v.data[2] = 3.0;

        TEST_ASSERT(vec_scale(v, -1.0, &res) == 0);
        TEST_ASSERT(approx_equal(res.data[0], -1.0));
        TEST_ASSERT(approx_equal(res.data[1], -2.0));
        TEST_ASSERT(approx_equal(res.data[2], -3.0));

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_vec_scale_distributive)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);
        Vector sum = vec_create(3);
        Vector scaled_sum = vec_create(3);
        Vector as = vec_create(3);
        Vector bs = vec_create(3);

        a.data[0] = 1.0;
        a.data[1] = 2.0;
        a.data[2] = 3.0;
        b.data[0] = 4.0;
        b.data[1] = 5.0;
        b.data[2] = 6.0;

        TEST_ASSERT(vec_add(a, b, &sum) == 0);
        TEST_ASSERT(vec_scale(sum, 2.0, &scaled_sum) == 0);
        TEST_ASSERT(vec_scale(a, 2.0, &as) == 0);
        TEST_ASSERT(vec_scale(b, 2.0, &bs) == 0);

        for (size_t i = 0; i < 3; i++) {
                TEST_ASSERT(
                    approx_equal(scaled_sum.data[i], as.data[i] + bs.data[i]));
        }

        vec_free(&a);
        vec_free(&b);
        vec_free(&sum);
        vec_free(&scaled_sum);
        vec_free(&as);
        vec_free(&bs);
}

/* ---------- Aliasing rejection ------------------------------------------- */

TEST_CASE(test_vec_scale_alias_result_eq_input)
{
        Vector v = vec_create(3);
        Vector res = vec_create(3);

        v.data[0] = 1.0;
        v.data[1] = 2.0;
        v.data[2] = 3.0;

        Vector aliased = {.size = 3, .data = v.data};
        TEST_ASSERT(vec_scale(v, 2.0, &aliased) == -1);
        TEST_ASSERT(approx_equal(v.data[0], 1.0));

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_vec_scale_zero_vector)
{
        Vector v = vec_create(3);
        Vector res = vec_create(3);

        TEST_ASSERT(vec_scale(v, 5.0, &res) == 0);
        for (size_t i = 0; i < 3; i++) {
                TEST_ASSERT(res.data[i] == 0.0);
        }

        vec_free(&v);
        vec_free(&res);
}

TEST_CASE(test_vec_scale_1_element)
{
        Vector v = vec_create(1);
        Vector res = vec_create(1);

        v.data[0] = 42.0;

        TEST_ASSERT(vec_scale(v, 0.0, &res) == 0);
        TEST_ASSERT(approx_equal(res.data[0], 0.0));

        vec_free(&v);
        vec_free(&res);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_scale tests ===\n\n");

        run_test(test_vec_scale_simple, "test_vec_scale_simple");
        run_test(test_vec_scale_mismatch, "test_vec_scale_mismatch");
        run_test(test_vec_scale_null_result, "test_vec_scale_null_result");
        run_test(test_vec_scale_null_input, "test_vec_scale_null_input");
        run_test(test_vec_scale_identity, "test_vec_scale_identity");
        run_test(test_vec_scale_neg_one, "test_vec_scale_neg_one");
        run_test(test_vec_scale_distributive, "test_vec_scale_distributive");
        run_test(test_vec_scale_alias_result_eq_input,
                 "test_vec_scale_alias_result_eq_input");
        run_test(test_vec_scale_zero_vector, "test_vec_scale_zero_vector");
        run_test(test_vec_scale_1_element, "test_vec_scale_1_element");

        fprintf(stdout, "\n=== All vec_scale tests passed ===\n\n");
        return EXIT_SUCCESS;
}
