/*
 * @file test_vec_add.c
 * @brief Unit tests for vector addition.
 */

#include "test_harness.h"

TEST_CASE(test_vec_add_simple)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);
        Vector res = vec_create(3);

        a.data[0] = 1.0;
        a.data[1] = 2.0;
        a.data[2] = 3.0;
        b.data[0] = 4.0;
        b.data[1] = 5.0;
        b.data[2] = 6.0;

        TEST_ASSERT(vec_add(a, b, &res) == 0);
        TEST_ASSERT(approx_equal(res.data[0], 5.0));
        TEST_ASSERT(approx_equal(res.data[1], 7.0));
        TEST_ASSERT(approx_equal(res.data[2], 9.0));

        vec_free(&a);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_vec_add_mismatch)
{
        Vector a = vec_create(3);
        Vector b = vec_create(2);
        Vector res = vec_create(3);

        TEST_ASSERT(vec_add(a, b, &res) == -1);

        vec_free(&a);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_vec_add_null_result)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);
        Vector res = {.size = 3, .data = NULL};

        TEST_ASSERT(vec_add(a, b, &res) == -1);

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_add_null_input_a)
{
        Vector a = {.size = 3, .data = NULL};
        Vector b = vec_create(3);
        Vector res = vec_create(3);

        b.data[0] = 1.0;
        b.data[1] = 2.0;
        b.data[2] = 3.0;

        TEST_ASSERT(vec_add(a, b, &res) == -1);

        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_vec_add_null_input_b)
{
        Vector a = vec_create(3);
        Vector b = {.size = 3, .data = NULL};
        Vector res = vec_create(3);

        TEST_ASSERT(vec_add(a, b, &res) == -1);

        vec_free(&a);
        vec_free(&res);
}

TEST_CASE(test_vec_add_zero_vectors)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);
        Vector res = vec_create(3);

        TEST_ASSERT(vec_add(a, b, &res) == 0);
        for (size_t i = 0; i < 3; i++) {
                TEST_ASSERT(res.data[i] == 0.0);
        }

        vec_free(&a);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_vec_add_1_element)
{
        Vector a = vec_create(1);
        Vector b = vec_create(1);
        Vector res = vec_create(1);

        a.data[0] = 3.5;
        b.data[0] = -1.5;

        TEST_ASSERT(vec_add(a, b, &res) == 0);
        TEST_ASSERT(approx_equal(res.data[0], 2.0));

        vec_free(&a);
        vec_free(&b);
        vec_free(&res);
}

/* ---------- Aliasing rejection (contract: result must not alias a or b) -- */

TEST_CASE(test_vec_add_alias_result_eq_a)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);
        Vector res = vec_create(3);

        a.data[0] = 1.0;
        a.data[1] = 2.0;
        a.data[2] = 3.0;
        b.data[0] = 4.0;
        b.data[1] = 5.0;
        b.data[2] = 6.0;

        /* Construct a Vector that aliases a's storage. */
        Vector aliased = {.size = 3, .data = a.data};
        TEST_ASSERT(vec_add(a, b, &aliased) == -1);

        /* Operand a must be untouched after rejection. */
        TEST_ASSERT(approx_equal(a.data[0], 1.0));
        TEST_ASSERT(approx_equal(a.data[2], 3.0));

        vec_free(&a);
        vec_free(&b);
        vec_free(&res);
}

TEST_CASE(test_vec_add_alias_result_eq_b)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);
        Vector res = vec_create(3);

        a.data[0] = 1.0;
        a.data[1] = 2.0;
        a.data[2] = 3.0;
        b.data[0] = 4.0;
        b.data[1] = 5.0;
        b.data[2] = 6.0;

        Vector aliased = {.size = 3, .data = b.data};
        TEST_ASSERT(vec_add(a, b, &aliased) == -1);
        TEST_ASSERT(approx_equal(b.data[0], 4.0));

        vec_free(&a);
        vec_free(&b);
        vec_free(&res);
}

/* ---------- Round-trip properties --------------------------------------- */

TEST_CASE(test_vec_add_sub_inverse)
{
        Vector a = vec_create(4);
        Vector b = vec_create(4);
        Vector sum = vec_create(4);
        Vector diff = vec_create(4);

        a.data[0] = 1.0;
        a.data[1] = 2.0;
        a.data[2] = 3.0;
        a.data[3] = 4.0;
        b.data[0] = 10.0;
        b.data[1] = 20.0;
        b.data[2] = 30.0;
        b.data[3] = 40.0;

        TEST_ASSERT(vec_add(a, b, &sum) == 0);
        TEST_ASSERT(vec_sub(sum, b, &diff) == 0);

        for (size_t i = 0; i < 4; i++) {
                TEST_ASSERT(approx_equal(diff.data[i], a.data[i]));
        }

        vec_free(&a);
        vec_free(&b);
        vec_free(&sum);
        vec_free(&diff);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_add tests ===\n\n");

        run_test(test_vec_add_simple, "test_vec_add_simple");
        run_test(test_vec_add_mismatch, "test_vec_add_mismatch");
        run_test(test_vec_add_null_result, "test_vec_add_null_result");
        run_test(test_vec_add_null_input_a, "test_vec_add_null_input_a");
        run_test(test_vec_add_null_input_b, "test_vec_add_null_input_b");
        run_test(test_vec_add_zero_vectors, "test_vec_add_zero_vectors");
        run_test(test_vec_add_1_element, "test_vec_add_1_element");
        run_test(test_vec_add_alias_result_eq_a,
                 "test_vec_add_alias_result_eq_a");
        run_test(test_vec_add_alias_result_eq_b,
                 "test_vec_add_alias_result_eq_b");
        run_test(test_vec_add_sub_inverse, "test_vec_add_sub_inverse");

        fprintf(stdout, "\n=== All vec_add tests passed ===\n\n");
        return EXIT_SUCCESS;
}
