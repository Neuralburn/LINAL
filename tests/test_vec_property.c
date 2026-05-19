/*
 * @file test_vec_property.c
 * @brief Property / fuzz tests for Vector operations.
 *
 * Each PROPERTY_TEST runs N iterations against random vectors generated from
 * a seeded PRNG. On failure the seed and iter index are reported so the
 * failure can be reproduced via:
 *
 *   LINAL_FUZZ_SEED=0x... LINAL_FUZZ_ITERS=N ./test_vec_property
 *
 * Tolerances: relative (DEFAULT_RTOL/ATOL) is the default — random shapes
 * accumulate FP roundoff and absolute EPSILON would yield false negatives.
 */

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "test_harness.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Random size kept small enough that an iteration is cheap. */
#define MIN_DIM 1
#define MAX_DIM 16

static size_t
rand_dim(test_rng *r)
{
        return rng_next_size(r, MIN_DIM, MAX_DIM + 1);
}

/* ============ ADD ======================================================= */

PROPERTY_TEST(prop_add_commutative, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);
        Vector ab = vec_create(n);
        Vector ba = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
        }

        TEST_ASSERT(vec_add(a, b, &ab) == 0);
        TEST_ASSERT(vec_add(b, a, &ba) == 0);

        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(ab.data[i], ba.data[i], DEFAULT_RTOL,
                                      DEFAULT_ATOL));
        }

        vec_free(&a);
        vec_free(&b);
        vec_free(&ab);
        vec_free(&ba);
}

PROPERTY_TEST(prop_add_associative, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);
        Vector c = vec_create(n);
        Vector ab = vec_create(n);
        Vector lhs = vec_create(n);
        Vector bc = vec_create(n);
        Vector rhs = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
                c.data[i] = rng_next_unit(rng);
        }

        TEST_ASSERT(vec_add(a, b, &ab) == 0);
        TEST_ASSERT(vec_add(ab, c, &lhs) == 0);
        TEST_ASSERT(vec_add(b, c, &bc) == 0);
        TEST_ASSERT(vec_add(a, bc, &rhs) == 0);

        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(lhs.data[i], rhs.data[i], DEFAULT_RTOL,
                                      DEFAULT_ATOL));
        }

        vec_free(&a);
        vec_free(&b);
        vec_free(&c);
        vec_free(&ab);
        vec_free(&bc);
        vec_free(&lhs);
        vec_free(&rhs);
}

/* ============ SUB ======================================================= */

PROPERTY_TEST(prop_sub_then_add_round_trip, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);
        Vector sum = vec_create(n);
        Vector recovered = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
        }

        TEST_ASSERT(vec_add(a, b, &sum) == 0);
        TEST_ASSERT(vec_sub(sum, b, &recovered) == 0);

        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(a.data[i], recovered.data[i],
                                      DEFAULT_RTOL, DEFAULT_ATOL));
        }

        vec_free(&a);
        vec_free(&b);
        vec_free(&sum);
        vec_free(&recovered);
}

/* ============ DOT ======================================================= */

PROPERTY_TEST(prop_dot_commutative, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
        }

        double dot_ab = vec_dot(a, b);
        double dot_ba = vec_dot(b, a);
        TEST_ASSERT(rel_equal(dot_ab, dot_ba, DEFAULT_RTOL, DEFAULT_ATOL));

        vec_free(&a);
        vec_free(&b);
}

PROPERTY_TEST(prop_dot_self_norm_squared, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
        }

        double dot = vec_dot(a, a);
        double nrm = vec_norm_l2(a);
        TEST_ASSERT(rel_equal(dot, nrm * nrm, DEFAULT_RTOL, DEFAULT_ATOL));

        vec_free(&a);
}

PROPERTY_TEST(prop_dot_cauchy_schwarz, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
        }

        double dot = vec_dot(a, b);
        double na = vec_norm_l2(a);
        double nb = vec_norm_l2(b);
        TEST_ASSERT(fabs(dot)
                    <= na * nb + DEFAULT_ATOL + DEFAULT_RTOL * na * nb);

        vec_free(&a);
        vec_free(&b);
}

PROPERTY_TEST(prop_dot_linear_left, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);
        Vector c = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
                c.data[i] = rng_next_unit(rng);
        }

        Vector ab = vec_create(n);
        TEST_ASSERT(vec_add(a, b, &ab) == 0);

        double lhs = vec_dot(ab, c);
        double dot_a = vec_dot(a, c);
        double dot_b = vec_dot(b, c);
        double rhs = dot_a + dot_b;

        TEST_ASSERT(rel_equal(lhs, rhs, DEFAULT_RTOL, DEFAULT_ATOL));

        vec_free(&a);
        vec_free(&b);
        vec_free(&c);
        vec_free(&ab);
}

/* ============ SCALE ===================================================== */

PROPERTY_TEST(prop_scale_distributive, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);
        double alpha = rng_next_unit(rng) * 5.0;

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
        }

        Vector sum = vec_create(n);
        Vector lhs = vec_create(n);
        Vector sA = vec_create(n);
        Vector sB = vec_create(n);
        Vector rhs = vec_create(n);

        TEST_ASSERT(vec_add(a, b, &sum) == 0);
        TEST_ASSERT(vec_scale(sum, alpha, &lhs) == 0);
        TEST_ASSERT(vec_scale(a, alpha, &sA) == 0);
        TEST_ASSERT(vec_scale(b, alpha, &sB) == 0);
        TEST_ASSERT(vec_add(sA, sB, &rhs) == 0);

        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(lhs.data[i], rhs.data[i], DEFAULT_RTOL,
                                      DEFAULT_ATOL));
        }

        vec_free(&a);
        vec_free(&b);
        vec_free(&sum);
        vec_free(&lhs);
        vec_free(&sA);
        vec_free(&sB);
        vec_free(&rhs);
}

PROPERTY_TEST(prop_scale_associative, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector v = vec_create(n);
        double alpha = rng_next_unit(rng) * 5.0;
        double beta = rng_next_unit(rng) * 5.0;

        for (size_t i = 0; i < n; i++) {
                v.data[i] = rng_next_unit(rng);
        }

        Vector sv = vec_create(n);
        Vector ssv = vec_create(n);
        Vector rhs = vec_create(n);

        TEST_ASSERT(vec_scale(v, alpha, &sv) == 0);
        TEST_ASSERT(vec_scale(sv, beta, &ssv) == 0);
        TEST_ASSERT(vec_scale(v, alpha * beta, &rhs) == 0);

        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(ssv.data[i], rhs.data[i], DEFAULT_RTOL,
                                      DEFAULT_ATOL));
        }

        vec_free(&v);
        vec_free(&sv);
        vec_free(&ssv);
        vec_free(&rhs);
}

/* ============ NORM ====================================================== */

PROPERTY_TEST(prop_norm_homogeneity, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector v = vec_create(n);
        double alpha = rng_next_unit(rng) * 7.0;

        for (size_t i = 0; i < n; i++) {
                v.data[i] = rng_next_unit(rng);
        }

        Vector sv = vec_create(n);
        TEST_ASSERT(vec_scale(v, alpha, &sv) == 0);

        double nsv = vec_norm_l2(sv);
        double nv = vec_norm_l2(v);
        TEST_ASSERT(
            rel_equal(nsv, fabs(alpha) * nv, DEFAULT_RTOL, DEFAULT_ATOL));

        vec_free(&v);
        vec_free(&sv);
}

PROPERTY_TEST(prop_norm_triangle, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);
        Vector sum = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
        }

        TEST_ASSERT(vec_add(a, b, &sum) == 0);

        double na = vec_norm_l2(a);
        double nb = vec_norm_l2(b);
        double nsum = vec_norm_l2(sum);
        TEST_ASSERT(nsum <= na + nb + DEFAULT_ATOL + DEFAULT_RTOL * (na + nb));

        vec_free(&a);
        vec_free(&b);
        vec_free(&sum);
}

/* ============ NORMALIZE ================================================= */

PROPERTY_TEST(prop_normalize_idempotent, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector v = vec_create(n);

        /* Avoid zero vector. */
        v.data[0] = rng_next_unit(rng);
        if (v.data[0] < 1e-10) {
                v.data[0] = 1.0;
        }
        for (size_t i = 1; i < n; i++) {
                v.data[i] = rng_next_unit(rng);
        }

        Vector n1 = vec_create(n);
        Vector n2 = vec_create(n);

        TEST_ASSERT(vec_normalize(v, &n1) == 0);
        TEST_ASSERT(vec_normalize(n1, &n2) == 0);

        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(n1.data[i], n2.data[i], DEFAULT_RTOL,
                                      DEFAULT_ATOL));
        }

        vec_free(&v);
        vec_free(&n1);
        vec_free(&n2);
}

PROPERTY_TEST(prop_normalize_preserves_direction, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector v = vec_create(n);

        /* Avoid zero vector. */
        v.data[0] = rng_next_unit(rng);
        if (v.data[0] < 1e-10) {
                v.data[0] = 1.0;
        }
        for (size_t i = 1; i < n; i++) {
                v.data[i] = rng_next_unit(rng);
        }

        Vector nv = vec_create(n);
        TEST_ASSERT(vec_normalize(v, &nv) == 0);

        /* norm(nv) should be 1.0 */
        double nv_norm = vec_norm_l2(nv);
        TEST_ASSERT(rel_equal(nv_norm, 1.0, DEFAULT_RTOL, DEFAULT_ATOL));

        /* v and nv should point in the same direction:
         *   dot(v, nv) ≈ ||v||   and   dot(v, nv) > 0 */
        double dot_v_nv = vec_dot(v, nv);
        double nrm_v = vec_norm_l2(v);
        TEST_ASSERT(dot_v_nv > 0.0);
        TEST_ASSERT(rel_equal(dot_v_nv, nrm_v, DEFAULT_RTOL, DEFAULT_ATOL));

        vec_free(&v);
        vec_free(&nv);
}

/* ============ DISTANCE ================================================== */

PROPERTY_TEST(prop_distance_triangle_inequality, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);
        Vector c = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
                c.data[i] = rng_next_unit(rng);
        }

        double d_ab = vec_distance(a, b);
        double d_bc = vec_distance(b, c);
        double d_ac = vec_distance(a, c);
        TEST_ASSERT(d_ac <= d_ab + d_bc + DEFAULT_ATOL
                                + DEFAULT_RTOL * (d_ab + d_bc));

        vec_free(&a);
        vec_free(&b);
        vec_free(&c);
}

/* ============ ANGLE ===================================================== */

PROPERTY_TEST(prop_angle_commutative, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);

        /* Avoid zero vectors. */
        a.data[0] = rng_next_unit(rng);
        if (a.data[0] < 1e-10) {
                a.data[0] = 1.0;
        }
        for (size_t i = 1; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
        }

        b.data[0] = rng_next_unit(rng);
        if (b.data[0] < 1e-10) {
                b.data[0] = 1.0;
        }
        for (size_t i = 1; i < n; i++) {
                b.data[i] = rng_next_unit(rng);
        }

        double ang_ab = vec_angle(a, b);
        double ang_ba = vec_angle(b, a);
        TEST_ASSERT(rel_equal(ang_ab, ang_ba, DEFAULT_RTOL, DEFAULT_ATOL));

        vec_free(&a);
        vec_free(&b);
}

PROPERTY_TEST(prop_angle_in_range, 200)
{
        (void)iter;
        size_t n = rand_dim(rng);
        Vector a = vec_create(n);
        Vector b = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(rng);
                b.data[i] = rng_next_unit(rng);
        }

        double angle = vec_angle(a, b);
        TEST_ASSERT(!isnan(angle));
        TEST_ASSERT(angle >= -DEFAULT_ATOL);
        TEST_ASSERT(angle <= M_PI + DEFAULT_ATOL);

        vec_free(&a);
        vec_free(&b);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_property tests ===\n\n");

        /* Add */
        run_test(prop_add_commutative, "prop_add_commutative");
        run_test(prop_add_associative, "prop_add_associative");

        /* Sub */
        run_test(prop_sub_then_add_round_trip, "prop_sub_then_add_round_trip");

        /* Dot */
        run_test(prop_dot_commutative, "prop_dot_commutative");
        run_test(prop_dot_self_norm_squared, "prop_dot_self_norm_squared");
        run_test(prop_dot_cauchy_schwarz, "prop_dot_cauchy_schwarz");
        run_test(prop_dot_linear_left, "prop_dot_linear_left");

        /* Scale */
        run_test(prop_scale_distributive, "prop_scale_distributive");
        run_test(prop_scale_associative, "prop_scale_associative");

        /* Norm */
        run_test(prop_norm_homogeneity, "prop_norm_homogeneity");
        run_test(prop_norm_triangle, "prop_norm_triangle");

        /* Normalize */
        run_test(prop_normalize_idempotent, "prop_normalize_idempotent");
        run_test(prop_normalize_preserves_direction,
                 "prop_normalize_preserves_direction");

        /* Distance */
        run_test(prop_distance_triangle_inequality,
                 "prop_distance_triangle_inequality");

        /* Angle */
        run_test(prop_angle_commutative, "prop_angle_commutative");
        run_test(prop_angle_in_range, "prop_angle_in_range");

        fprintf(stdout, "\n=== All vec_property tests passed ===\n\n");
        return EXIT_SUCCESS;
}
