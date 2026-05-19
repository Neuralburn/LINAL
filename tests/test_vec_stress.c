/*
 * @file test_vec_stress.c
 * @brief Stress tests for Vector operations.
 *
 * Exercises large vectors and numerical edge cases where bugs in optimized
 * paths show up but tiny hand-picked tests do not.
 */

#include "test_harness.h"

/* ============ LARGE VECTOR OPERATIONS =================================== */

TEST_CASE(test_large_vec_ops)
{
        const size_t n = 1000;

        test_rng rng = rng_seed(0xCAFEBABEULL);
        Vector a = vec_create(n);
        Vector b = vec_create(n);
        Vector scale_a = vec_create(n);
        Vector sum = vec_create(n);
        Vector sub = vec_create(n);
        Vector scaled = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(&rng);
                b.data[i] = rng_next_unit(&rng);
        }

        /* vec_add */
        int code = vec_add(a, b, &sum);
        TEST_ASSERT(code == 0);
        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(sum.data[i], a.data[i] + b.data[i],
                                      DEFAULT_RTOL, DEFAULT_ATOL));
        }

        /* vec_sub */
        code = vec_sub(a, b, &sub);
        TEST_ASSERT(code == 0);
        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(sub.data[i], a.data[i] - b.data[i],
                                      DEFAULT_RTOL, DEFAULT_ATOL));
        }

        /* vec_scale */
        double alpha = 2.5;
        code = vec_scale(a, alpha, &scaled);
        TEST_ASSERT(code == 0);
        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(scaled.data[i], a.data[i] * alpha,
                                      DEFAULT_RTOL, DEFAULT_ATOL));
        }

        /* vec_dot — verify against manual accumulation */
        double dot = vec_dot(a, b);
        double manual_dot = 0.0;
        for (size_t i = 0; i < n; i++) {
                manual_dot += a.data[i] * b.data[i];
        }
        TEST_ASSERT(rel_equal(dot, manual_dot, DEFAULT_RTOL, DEFAULT_ATOL));

        /* vec_norm_l2 — verify against manual computation */
        double norm = vec_norm_l2(a);
        double manual_norm = 0.0;
        for (size_t i = 0; i < n; i++) {
                manual_norm += a.data[i] * a.data[i];
        }
        manual_norm = sqrt(manual_norm);
        TEST_ASSERT(rel_equal(norm, manual_norm, DEFAULT_RTOL, DEFAULT_ATOL));

        vec_free(&a);
        vec_free(&b);
        vec_free(&scale_a);
        vec_free(&sum);
        vec_free(&sub);
        vec_free(&scaled);
}

/* ============ LARGE VECTOR NORMALIZE ==================================== */

TEST_CASE(test_large_vec_normalize)
{
        const size_t n = 500;

        test_rng rng = rng_seed(0xDEADBEEFULL);
        Vector v = vec_create(n);
        Vector nv = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                v.data[i] = rng_next_unit(&rng);
        }

        int code = vec_normalize(v, &nv);
        TEST_ASSERT(code == 0);

        /* norm(nv) should be 1.0 */
        double nv_norm = vec_norm_l2(nv);
        TEST_ASSERT(rel_equal(nv_norm, 1.0, DEFAULT_RTOL, DEFAULT_ATOL));

        /* Verify v == nv * norm(v) */
        double v_norm = vec_norm_l2(v);
        Vector recovered = vec_create(n);
        code = vec_scale(nv, v_norm, &recovered);
        TEST_ASSERT(code == 0);

        for (size_t i = 0; i < n; i++) {
                TEST_ASSERT(rel_equal(recovered.data[i], v.data[i],
                                      DEFAULT_RTOL, DEFAULT_ATOL));
        }

        vec_free(&v);
        vec_free(&nv);
        vec_free(&recovered);
}

/* ============ NUMERICAL EXTREMES ======================================== */

TEST_CASE(test_numerical_extremes)
{
        /* Test with very large values (near overflow) */
        Vector large = vec_create(3);
        large.data[0] = 1e200;
        large.data[1] = 1e200;
        large.data[2] = 1e200;

        double large_norm = vec_norm_l2(large);
        /* norm should be INF or very large */
        TEST_ASSERT(large_norm >= 1e200);

        double large_dot = vec_dot(large, large);
        /* 1e200^2 * 3 overflows double; expect INF or very large */
        TEST_ASSERT(isinf(large_dot) || large_dot >= 1e300);

        vec_free(&large);

        /* Test with very small values (near underflow) */
        Vector tiny = vec_create(3);
        tiny.data[0] = 1e-200;
        tiny.data[1] = 1e-200;
        tiny.data[2] = 1e-200;

        double tiny_norm = vec_norm_l2(tiny);
        /* norm should be very small but finite */
        TEST_ASSERT(tiny_norm >= 0.0);
        TEST_ASSERT(tiny_norm <= 1e-100);

        vec_free(&tiny);

        /* Test with Inf */
        Vector inf = vec_create(2);
        inf.data[0] = INFINITY;
        inf.data[1] = 1.0;

        double inf_norm = vec_norm_l2(inf);
        TEST_ASSERT(isinf(inf_norm));

        double inf_dot = vec_dot(inf, inf);
        TEST_ASSERT(isinf(inf_dot));

        vec_free(&inf);

        /* Test with NaN */
        Vector nan = vec_create(2);
        nan.data[0] = NAN;
        nan.data[1] = 1.0;

        double nan_norm = vec_norm_l2(nan);
        TEST_ASSERT(isnan(nan_norm));

        double nan_dot = vec_dot(nan, nan);
        TEST_ASSERT(isnan(nan_dot));

        vec_free(&nan);

        /* Test vec_distance with extreme values */
        Vector a = vec_create(3);
        a.data[0] = 1e200;
        a.data[1] = 0.0;
        a.data[2] = 0.0;

        Vector b = vec_create(3);
        b.data[0] = 2e200;
        b.data[1] = 0.0;
        b.data[2] = 0.0;

        double dist = vec_distance(a, b);
        TEST_ASSERT(isinf(dist));

        vec_free(&a);
        vec_free(&b);
}

/* ============ LARGE VECTOR DISTANCE ===================================== */

TEST_CASE(test_large_vec_distance)
{
        const size_t n = 1000;

        test_rng rng = rng_seed(0x1234ABCDULL);
        Vector a = vec_create(n);
        Vector b = vec_create(n);
        Vector c = vec_create(n);

        for (size_t i = 0; i < n; i++) {
                a.data[i] = rng_next_unit(&rng);
                b.data[i] = rng_next_unit(&rng);
                c.data[i] = rng_next_unit(&rng);
        }

        /* Triangle inequality at scale */
        double d_ab = vec_distance(a, b);
        double d_bc = vec_distance(b, c);
        double d_ac = vec_distance(a, c);
        TEST_ASSERT(d_ac <= d_ab + d_bc + DEFAULT_ATOL
                                + DEFAULT_RTOL * (d_ab + d_bc));

        /* Verify distance(a, a) == 0 */
        double d_aa = vec_distance(a, a);
        TEST_ASSERT(rel_equal(d_aa, 0.0, DEFAULT_RTOL, DEFAULT_ATOL));

        /* Verify distance(a, b) == norm(a - b) */
        Vector diff = vec_create(n);
        TEST_ASSERT(vec_sub(a, b, &diff) == 0);
        double norm_diff = vec_norm_l2(diff);
        TEST_ASSERT(rel_equal(d_ab, norm_diff, DEFAULT_RTOL, DEFAULT_ATOL));

        /* Verify distance is symmetric */
        double d_ba = vec_distance(b, a);
        TEST_ASSERT(rel_equal(d_ab, d_ba, DEFAULT_RTOL, DEFAULT_ATOL));

        vec_free(&a);
        vec_free(&b);
        vec_free(&c);
        vec_free(&diff);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_stress tests ===\n\n");

        run_test(test_large_vec_ops, "test_large_vec_ops");
        run_test(test_large_vec_normalize, "test_large_vec_normalize");
        run_test(test_numerical_extremes, "test_numerical_extremes");
        run_test(test_large_vec_distance, "test_large_vec_distance");

        fprintf(stdout, "\n=== All vec_stress tests passed ===\n\n");
        return EXIT_SUCCESS;
}
