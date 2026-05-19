/*
 * @file test_vec_ml.c
 * @brief Unit tests for vector ML/primitive operations (vec_hadamard,
 * vec_abs).
 */

#include "test_harness.h"

/* ============= vec_hadamard tests ===================================== */

TEST_CASE(test_vec_hadamard_basic)
{
        Vector a = vec_create(4);
        a.data[0] = 1.0;
        a.data[1] = 2.0;
        a.data[2] = 3.0;
        a.data[3] = 4.0;

        Vector b = vec_create(4);
        b.data[0] = 5.0;
        b.data[1] = 6.0;
        b.data[2] = 7.0;
        b.data[3] = 8.0;

        Vector result = vec_create(4);
        TEST_ASSERT(vec_hadamard(a, b, &result) == 0);

        TEST_ASSERT(approx_equal(result.data[0], 5.0));
        TEST_ASSERT(approx_equal(result.data[1], 12.0));
        TEST_ASSERT(approx_equal(result.data[2], 21.0));
        TEST_ASSERT(approx_equal(result.data[3], 32.0));

        vec_free(&a);
        vec_free(&b);
        vec_free(&result);
}

TEST_CASE(test_vec_hadamard_zero_vectors)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);

        Vector result = vec_create(3);
        TEST_ASSERT(vec_hadamard(a, b, &result) == 0);

        for (size_t i = 0; i < 3; i++) {
                TEST_ASSERT(approx_equal(result.data[i], 0.0));
        }

        vec_free(&a);
        vec_free(&b);
        vec_free(&result);
}

TEST_CASE(test_vec_hadamard_1_element)
{
        Vector a = vec_create(1);
        a.data[0] = -3.0;

        Vector b = vec_create(1);
        b.data[0] = 4.0;

        Vector result = vec_create(1);
        TEST_ASSERT(vec_hadamard(a, b, &result) == 0);
        TEST_ASSERT(approx_equal(result.data[0], -12.0));

        vec_free(&a);
        vec_free(&b);
        vec_free(&result);
}

TEST_CASE(test_vec_hadamard_size_mismatch)
{
        Vector a = vec_create(3);
        Vector b = vec_create(4);

        Vector result = vec_create(3);
        TEST_ASSERT(vec_hadamard(a, b, &result) == -1);

        vec_free(&a);
        vec_free(&b);
        vec_free(&result);
}

TEST_CASE(test_vec_hadamard_null_result)
{
        Vector a = vec_create(3);
        Vector b = vec_create(3);

        TEST_ASSERT(vec_hadamard(a, b, NULL) == -1);

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_hadamard_null_data)
{
        Vector a = vec_create(3);
        Vector b = {0};

        Vector result = vec_create(3);
        TEST_ASSERT(vec_hadamard(a, b, &result) == -1);

        vec_free(&a);
        vec_free(&result);
}

TEST_CASE(test_vec_hadamard_distributive)
{
        /* a * (b + c) == a*b + a*c */
        Vector a = vec_create(3);
        a.data[0] = 2.0;
        a.data[1] = 3.0;
        a.data[2] = 4.0;

        Vector b = vec_create(3);
        b.data[0] = 1.0;
        b.data[1] = 2.0;
        b.data[2] = 3.0;

        Vector c = vec_create(3);
        c.data[0] = 4.0;
        c.data[1] = 5.0;
        c.data[2] = 6.0;

        Vector sum = vec_create(3);
        Vector lhs = vec_create(3);
        Vector rhs = vec_create(3);
        Vector rhs_ab = vec_create(3);
        Vector rhs_ac = vec_create(3);

        TEST_ASSERT(vec_add(b, c, &sum) == 0);
        TEST_ASSERT(vec_hadamard(a, sum, &lhs) == 0);
        TEST_ASSERT(vec_hadamard(a, b, &rhs_ab) == 0);
        TEST_ASSERT(vec_hadamard(a, c, &rhs_ac) == 0);
        TEST_ASSERT(vec_add(rhs_ab, rhs_ac, &rhs) == 0);

        for (size_t i = 0; i < 3; i++) {
                TEST_ASSERT(approx_equal(lhs.data[i], rhs.data[i]));
        }

        vec_free(&a);
        vec_free(&b);
        vec_free(&c);
        vec_free(&sum);
        vec_free(&lhs);
        vec_free(&rhs);
        vec_free(&rhs_ab);
        vec_free(&rhs_ac);
}

/* ============= vec_abs tests ========================================== */

TEST_CASE(test_vec_abs_mixed_signs)
{
        Vector v = vec_create(5);
        v.data[0] = -3.0;
        v.data[1] = 2.0;
        v.data[2] = -1.5;
        v.data[3] = 0.0;
        v.data[4] = -4.0;

        Vector result = vec_create(5);
        TEST_ASSERT(vec_abs(v, &result) == 0);

        TEST_ASSERT(approx_equal(result.data[0], 3.0));
        TEST_ASSERT(approx_equal(result.data[1], 2.0));
        TEST_ASSERT(approx_equal(result.data[2], 1.5));
        TEST_ASSERT(approx_equal(result.data[3], 0.0));
        TEST_ASSERT(approx_equal(result.data[4], 4.0));

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_abs_all_positive)
{
        Vector v = vec_create(3);
        v.data[0] = 1.0;
        v.data[1] = 2.0;
        v.data[2] = 3.0;

        Vector result = vec_create(3);
        TEST_ASSERT(vec_abs(v, &result) == 0);

        for (size_t i = 0; i < 3; i++) {
                TEST_ASSERT(approx_equal(result.data[i], v.data[i]));
        }

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_abs_all_negative)
{
        Vector v = vec_create(3);
        v.data[0] = -1.0;
        v.data[1] = -2.0;
        v.data[2] = -3.0;

        Vector result = vec_create(3);
        TEST_ASSERT(vec_abs(v, &result) == 0);

        TEST_ASSERT(approx_equal(result.data[0], 1.0));
        TEST_ASSERT(approx_equal(result.data[1], 2.0));
        TEST_ASSERT(approx_equal(result.data[2], 3.0));

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_abs_zero_vector)
{
        Vector v = vec_create(3);

        Vector result = vec_create(3);
        TEST_ASSERT(vec_abs(v, &result) == 0);

        for (size_t i = 0; i < 3; i++) {
                TEST_ASSERT(approx_equal(result.data[i], 0.0));
        }

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_abs_1_element)
{
        Vector v = vec_create(1);
        v.data[0] = -42.0;

        Vector result = vec_create(1);
        TEST_ASSERT(vec_abs(v, &result) == 0);
        TEST_ASSERT(approx_equal(result.data[0], 42.0));

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_abs_null_input)
{
        Vector v = {0};
        Vector result = vec_create(3);

        TEST_ASSERT(vec_abs(v, &result) == -1);

        vec_free(&result);
}

TEST_CASE(test_vec_abs_null_result)
{
        Vector v = vec_create(3);
        v.data[0] = 1.0;

        TEST_ASSERT(vec_abs(v, NULL) == -1);

        vec_free(&v);
}

TEST_CASE(test_vec_abs_size_mismatch)
{
        Vector v = vec_create(3);
        v.data[0] = 1.0;
        Vector result = vec_create(2);

        TEST_ASSERT(vec_abs(v, &result) == -1);

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_abs_idempotent)
{
        /* abs(abs(v)) == abs(v) */
        Vector v = vec_create(4);
        v.data[0] = -1.0;
        v.data[1] = 2.0;
        v.data[2] = -3.0;
        v.data[3] = 0.0;

        Vector r1 = vec_create(4);
        Vector r2 = vec_create(4);

        TEST_ASSERT(vec_abs(v, &r1) == 0);
        TEST_ASSERT(vec_abs(r1, &r2) == 0);

        for (size_t i = 0; i < 4; i++) {
                TEST_ASSERT(approx_equal(r1.data[i], r2.data[i]));
        }

        vec_free(&v);
        vec_free(&r1);
        vec_free(&r2);
}

TEST_CASE(test_vec_abs_norm_equals_dot)
{
        /* For any vector v: norm(v) == sqrt(dot(abs(v), abs(v))) */
        Vector v = vec_create(3);
        v.data[0] = -3.0;
        v.data[1] = 4.0;
        v.data[2] = -12.0;

        Vector abs_v = vec_create(3);
        TEST_ASSERT(vec_abs(v, &abs_v) == 0);

        double norm = vec_norm_l2(v);
        double dot = vec_dot(abs_v, abs_v);

        TEST_ASSERT(approx_equal(norm, sqrt(dot)));

        vec_free(&v);
        vec_free(&abs_v);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_ml tests ===\n\n");

        /* vec_hadamard */
        run_test(test_vec_hadamard_basic, "test_vec_hadamard_basic");
        run_test(test_vec_hadamard_zero_vectors,
                 "test_vec_hadamard_zero_vectors");
        run_test(test_vec_hadamard_1_element, "test_vec_hadamard_1_element");
        run_test(test_vec_hadamard_size_mismatch,
                 "test_vec_hadamard_size_mismatch");
        run_test(test_vec_hadamard_null_result,
                 "test_vec_hadamard_null_result");
        run_test(test_vec_hadamard_null_data, "test_vec_hadamard_null_data");
        run_test(test_vec_hadamard_distributive,
                 "test_vec_hadamard_distributive");

        /* vec_abs */
        run_test(test_vec_abs_mixed_signs, "test_vec_abs_mixed_signs");
        run_test(test_vec_abs_all_positive, "test_vec_abs_all_positive");
        run_test(test_vec_abs_all_negative, "test_vec_abs_all_negative");
        run_test(test_vec_abs_zero_vector, "test_vec_abs_zero_vector");
        run_test(test_vec_abs_1_element, "test_vec_abs_1_element");
        run_test(test_vec_abs_null_input, "test_vec_abs_null_input");
        run_test(test_vec_abs_null_result, "test_vec_abs_null_result");
        run_test(test_vec_abs_size_mismatch, "test_vec_abs_size_mismatch");
        run_test(test_vec_abs_idempotent, "test_vec_abs_idempotent");
        run_test(test_vec_abs_norm_equals_dot, "test_vec_abs_norm_equals_dot");

        fprintf(stdout, "\n=== All vec_ml tests passed ===\n\n");
        return EXIT_SUCCESS;
}
