/*
 * @file test_vec_geom.c
 * @brief Unit tests for vector geometric operations (vec_normalize,
 * vec_distance, vec_angle).
 */

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "test_harness.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ============= vec_normalize tests ==================================== */

TEST_CASE(test_vec_normalize_basic)
{
        Vector v = vec_create(3);
        v.data[0] = 3.0;
        v.data[1] = 4.0;
        v.data[2] = 0.0;

        Vector result = vec_create(3);
        TEST_ASSERT(vec_normalize(v, &result) == 0);

        /* 3-4-5 triangle: norm is 5, so normalized = (0.6, 0.8, 0.0) */
        TEST_ASSERT(approx_equal(result.data[0], 0.6));
        TEST_ASSERT(approx_equal(result.data[1], 0.8));
        TEST_ASSERT(approx_equal(result.data[2], 0.0));

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_normalize_unit_vector)
{
        /* Already unit length — should be identity. */
        Vector v = vec_create(2);
        v.data[0] = 1.0 / sqrt(2.0);
        v.data[1] = 1.0 / sqrt(2.0);

        Vector result = vec_create(2);
        TEST_ASSERT(vec_normalize(v, &result) == 0);
        TEST_ASSERT(approx_equal(result.data[0], v.data[0]));
        TEST_ASSERT(approx_equal(result.data[1], v.data[1]));

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_normalize_zero_vector)
{
        Vector v = vec_create(3); /* all zeros */
        Vector result = vec_create(3);

        TEST_ASSERT(vec_normalize(v, &result) == -1);

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_normalize_null_result)
{
        Vector v = vec_create(3);
        v.data[0] = 1.0;

        TEST_ASSERT(vec_normalize(v, NULL) == -1);

        vec_free(&v);
}

TEST_CASE(test_vec_normalize_size_mismatch)
{
        Vector v = vec_create(3);
        v.data[0] = 1.0;
        Vector result = vec_create(2);

        TEST_ASSERT(vec_normalize(v, &result) == -1);

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_normalize_1_element)
{
        Vector v = vec_create(1);
        v.data[0] = -7.0;

        Vector result = vec_create(1);
        TEST_ASSERT(vec_normalize(v, &result) == 0);
        /* -7 / 7 = -1.0 */
        TEST_ASSERT(approx_equal(result.data[0], -1.0));

        vec_free(&v);
        vec_free(&result);
}

TEST_CASE(test_vec_normalize_round_trip)
{
        /* normalize(v) * norm(v) should equal v. */
        Vector v = vec_create(4);
        v.data[0] = 1.0;
        v.data[1] = -2.0;
        v.data[2] = 2.0;
        v.data[3] = 1.0;

        double norm = vec_norm_l2(v);
        Vector result = vec_create(4);
        TEST_ASSERT(vec_normalize(v, &result) == 0);

        for (size_t i = 0; i < v.size; i++) {
                TEST_ASSERT(approx_equal(v.data[i], result.data[i] * norm));
        }

        vec_free(&v);
        vec_free(&result);
}

/* ============= vec_distance tests ===================================== */

TEST_CASE(test_vec_distance_basic)
{
        Vector a = vec_create(3);
        a.data[0] = 0.0;
        a.data[1] = 0.0;
        a.data[2] = 0.0;

        Vector b = vec_create(3);
        b.data[0] = 3.0;
        b.data[1] = 4.0;
        b.data[2] = 0.0;

        /* distance = 5.0 */
        TEST_ASSERT(approx_equal(vec_distance(a, b), 5.0));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_distance_identical)
{
        Vector a = vec_create(3);
        a.data[0] = 1.0;
        a.data[1] = 2.0;
        a.data[2] = 3.0;

        Vector b = vec_create(3);
        b.data[0] = 1.0;
        b.data[1] = 2.0;
        b.data[2] = 3.0;

        TEST_ASSERT(approx_equal(vec_distance(a, b), 0.0));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_distance_zero_vectors)
{
        Vector a = vec_create(0);
        Vector b = vec_create(0);

        TEST_ASSERT(approx_equal(vec_distance(a, b), 0.0));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_distance_size_mismatch)
{
        Vector a = vec_create(3);
        Vector b = vec_create(4);

        TEST_ASSERT(isnan(vec_distance(a, b)));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_distance_1_element)
{
        Vector a = vec_create(1);
        a.data[0] = 3.0;

        Vector b = vec_create(1);
        b.data[0] = 7.0;

        TEST_ASSERT(approx_equal(vec_distance(a, b), 4.0));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_distance_triangle_inequality)
{
        /* ||a-c|| <= ||a-b|| + ||b-c|| */
        Vector a = vec_create(2);
        a.data[0] = 0.0;
        a.data[1] = 0.0;

        Vector b = vec_create(2);
        b.data[0] = 3.0;
        b.data[1] = 4.0;

        Vector c = vec_create(2);
        c.data[0] = 6.0;
        c.data[1] = 8.0;

        double d_ab = vec_distance(a, b);
        double d_bc = vec_distance(b, c);
        double d_ac = vec_distance(a, c);

        TEST_ASSERT(d_ac <= d_ab + d_bc + EPSILON);

        vec_free(&a);
        vec_free(&b);
        vec_free(&c);
}

TEST_CASE(test_vec_distance_round_trip)
{
        /* distance(a, b) == norm(a - b) */
        Vector a = vec_create(4);
        a.data[0] = 1.0;
        a.data[1] = -2.0;
        a.data[2] = 3.0;
        a.data[3] = -4.0;

        Vector b = vec_create(4);
        b.data[0] = 5.0;
        b.data[1] = 6.0;
        b.data[2] = -7.0;
        b.data[3] = 8.0;

        Vector diff = vec_create(4);
        TEST_ASSERT(vec_sub(a, b, &diff) == 0);

        double dist = vec_distance(a, b);
        double norm = vec_norm_l2(diff);

        TEST_ASSERT(approx_equal(dist, norm));

        vec_free(&a);
        vec_free(&b);
        vec_free(&diff);
}

/* ============= vec_angle tests ======================================== */

TEST_CASE(test_vec_angle_orthogonal)
{
        Vector a = vec_create(2);
        a.data[0] = 1.0;
        a.data[1] = 0.0;

        Vector b = vec_create(2);
        b.data[0] = 0.0;
        b.data[1] = 1.0;

        /* angle = pi/2 */
        TEST_ASSERT(approx_equal(vec_angle(a, b), M_PI / 2.0));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_angle_parallel)
{
        Vector a = vec_create(2);
        a.data[0] = 3.0;
        a.data[1] = 4.0;

        Vector b = vec_create(2);
        b.data[0] = 6.0;
        b.data[1] = 8.0;

        /* angle = 0 (parallel, same direction) */
        TEST_ASSERT(approx_equal(vec_angle(a, b), 0.0));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_angle_antiparallel)
{
        Vector a = vec_create(2);
        a.data[0] = 1.0;
        a.data[1] = 0.0;

        Vector b = vec_create(2);
        b.data[0] = -1.0;
        b.data[1] = 0.0;

        /* angle = pi */
        TEST_ASSERT(approx_equal(vec_angle(a, b), M_PI));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_angle_zero_vector)
{
        Vector a = vec_create(2);
        Vector b = vec_create(2);
        b.data[0] = 1.0;
        b.data[1] = 0.0;

        TEST_ASSERT(isnan(vec_angle(a, b)));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_angle_size_mismatch)
{
        Vector a = vec_create(2);
        Vector b = vec_create(3);

        TEST_ASSERT(isnan(vec_angle(a, b)));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_angle_commutative)
{
        Vector a = vec_create(3);
        a.data[0] = 1.0;
        a.data[1] = 2.0;
        a.data[2] = 3.0;

        Vector b = vec_create(3);
        b.data[0] = 4.0;
        b.data[1] = 5.0;
        b.data[2] = 6.0;

        TEST_ASSERT(approx_equal(vec_angle(a, b), vec_angle(b, a)));

        vec_free(&a);
        vec_free(&b);
}

TEST_CASE(test_vec_angle_1_element)
{
        Vector a = vec_create(1);
        a.data[0] = 5.0;

        Vector b = vec_create(1);
        b.data[0] = -3.0;

        /* angle = pi (antiparallel) */
        TEST_ASSERT(approx_equal(vec_angle(a, b), M_PI));

        vec_free(&a);
        vec_free(&b);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_geom tests ===\n\n");

        /* vec_normalize */
        run_test(test_vec_normalize_basic, "test_vec_normalize_basic");
        run_test(test_vec_normalize_unit_vector,
                 "test_vec_normalize_unit_vector");
        run_test(test_vec_normalize_zero_vector,
                 "test_vec_normalize_zero_vector");
        run_test(test_vec_normalize_null_result,
                 "test_vec_normalize_null_result");
        run_test(test_vec_normalize_size_mismatch,
                 "test_vec_normalize_size_mismatch");
        run_test(test_vec_normalize_1_element, "test_vec_normalize_1_element");
        run_test(test_vec_normalize_round_trip,
                 "test_vec_normalize_round_trip");

        /* vec_distance */
        run_test(test_vec_distance_basic, "test_vec_distance_basic");
        run_test(test_vec_distance_identical, "test_vec_distance_identical");
        run_test(test_vec_distance_zero_vectors,
                 "test_vec_distance_zero_vectors");
        run_test(test_vec_distance_size_mismatch,
                 "test_vec_distance_size_mismatch");
        run_test(test_vec_distance_1_element, "test_vec_distance_1_element");
        run_test(test_vec_distance_triangle_inequality,
                 "test_vec_distance_triangle_inequality");
        run_test(test_vec_distance_round_trip, "test_vec_distance_round_trip");

        /* vec_angle */
        run_test(test_vec_angle_orthogonal, "test_vec_angle_orthogonal");
        run_test(test_vec_angle_parallel, "test_vec_angle_parallel");
        run_test(test_vec_angle_antiparallel, "test_vec_angle_antiparallel");
        run_test(test_vec_angle_zero_vector, "test_vec_angle_zero_vector");
        run_test(test_vec_angle_size_mismatch, "test_vec_angle_size_mismatch");
        run_test(test_vec_angle_commutative, "test_vec_angle_commutative");
        run_test(test_vec_angle_1_element, "test_vec_angle_1_element");

        fprintf(stdout, "\n=== All vec_geom tests passed ===\n\n");
        return EXIT_SUCCESS;
}
