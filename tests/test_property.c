/*
 * @file test_property.c
 * @brief Property / fuzz tests for LINAL.
 *
 * Each PROPERTY_TEST runs N iterations against random matrices generated
 * from a seeded PRNG.  On failure the seed and iter index are reported so
 * the failure can be reproduced via:
 *
 *   LINAL_FUZZ_SEED=0x... LINAL_FUZZ_ITERS=N ./test_property
 *
 * Tolerances: relative (DEFAULT_RTOL/ATOL) is the default — random shapes
 * accumulate FP roundoff and absolute EPSILON would yield false negatives.
 */

#include "test_harness.h"

/* Random dimensions kept small enough that an iteration is cheap.
 * Property tests are run repeatedly; per-iter cost dominates. */
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
        size_t r = rand_dim(rng), c = rand_dim(rng);
        Matrix A = mat_random(r, c, rng);
        Matrix B = mat_random(r, c, rng);
        Matrix AB = mat_create(r, c);
        Matrix BA = mat_create(r, c);

        TEST_ASSERT(mat_add(A, B, &AB) == 0);
        TEST_ASSERT(mat_add(B, A, &BA) == 0);
        TEST_ASSERT(mat_relative_equal(AB, BA, DEFAULT_RTOL, DEFAULT_ATOL) == 0);

        mat_free(&A); mat_free(&B);
        mat_free(&AB); mat_free(&BA);
}

PROPERTY_TEST(prop_add_associative, 200)
{
        (void)iter;
        size_t r = rand_dim(rng), c = rand_dim(rng);
        Matrix A = mat_random(r, c, rng);
        Matrix B = mat_random(r, c, rng);
        Matrix C = mat_random(r, c, rng);

        Matrix AB = mat_create(r, c);
        Matrix lhs = mat_create(r, c);
        Matrix BC = mat_create(r, c);
        Matrix rhs = mat_create(r, c);

        TEST_ASSERT(mat_add(A, B, &AB) == 0);
        TEST_ASSERT(mat_add(AB, C, &lhs) == 0);
        TEST_ASSERT(mat_add(B, C, &BC) == 0);
        TEST_ASSERT(mat_add(A, BC, &rhs) == 0);
        TEST_ASSERT(mat_relative_equal(lhs, rhs, DEFAULT_RTOL,
                                        DEFAULT_ATOL) == 0);

        mat_free(&A); mat_free(&B); mat_free(&C);
        mat_free(&AB); mat_free(&BC);
        mat_free(&lhs); mat_free(&rhs);
}

/* ============ SUB ======================================================= */

PROPERTY_TEST(prop_sub_then_add_round_trip, 200)
{
        (void)iter;
        size_t r = rand_dim(rng), c = rand_dim(rng);
        Matrix A = mat_random(r, c, rng);
        Matrix B = mat_random(r, c, rng);
        Matrix sum = mat_create(r, c);
        Matrix recovered = mat_create(r, c);

        TEST_ASSERT(mat_add(A, B, &sum) == 0);
        TEST_ASSERT(mat_sub(sum, B, &recovered) == 0);
        TEST_ASSERT(mat_relative_equal(A, recovered, DEFAULT_RTOL,
                                        DEFAULT_ATOL) == 0);

        mat_free(&A); mat_free(&B);
        mat_free(&sum); mat_free(&recovered);
}

/* ============ MUL ======================================================= */

PROPERTY_TEST(prop_mul_associative, 60)
{
        (void)iter;
        /* Three random matrices with compatible dims. */
        size_t m = rand_dim(rng);
        size_t k = rand_dim(rng);
        size_t n = rand_dim(rng);
        size_t p = rand_dim(rng);

        Matrix A = mat_random(m, k, rng);
        Matrix B = mat_random(k, n, rng);
        Matrix C = mat_random(n, p, rng);

        Matrix AB = mat_create(m, n);
        Matrix lhs = mat_create(m, p);
        Matrix BC = mat_create(k, p);
        Matrix rhs = mat_create(m, p);

        TEST_ASSERT(mat_mul(A, B, &AB) == 0);
        TEST_ASSERT(mat_mul(AB, C, &lhs) == 0);
        TEST_ASSERT(mat_mul(B, C, &BC) == 0);
        TEST_ASSERT(mat_mul(A, BC, &rhs) == 0);
        TEST_ASSERT(mat_relative_equal(lhs, rhs, DEFAULT_RTOL,
                                        DEFAULT_ATOL) == 0);

        mat_free(&A); mat_free(&B); mat_free(&C);
        mat_free(&AB); mat_free(&BC);
        mat_free(&lhs); mat_free(&rhs);
}

PROPERTY_TEST(prop_mul_distributive_left, 100)
{
        (void)iter;
        size_t m = rand_dim(rng);
        size_t k = rand_dim(rng);
        size_t n = rand_dim(rng);

        Matrix A = mat_random(m, k, rng);
        Matrix B = mat_random(k, n, rng);
        Matrix C = mat_random(k, n, rng);

        Matrix BplusC = mat_create(k, n);
        Matrix lhs = mat_create(m, n);
        Matrix AB = mat_create(m, n);
        Matrix AC = mat_create(m, n);
        Matrix rhs = mat_create(m, n);

        TEST_ASSERT(mat_add(B, C, &BplusC) == 0);
        TEST_ASSERT(mat_mul(A, BplusC, &lhs) == 0);
        TEST_ASSERT(mat_mul(A, B, &AB) == 0);
        TEST_ASSERT(mat_mul(A, C, &AC) == 0);
        TEST_ASSERT(mat_add(AB, AC, &rhs) == 0);
        TEST_ASSERT(mat_relative_equal(lhs, rhs, DEFAULT_RTOL,
                                        DEFAULT_ATOL) == 0);

        mat_free(&A); mat_free(&B); mat_free(&C);
        mat_free(&BplusC); mat_free(&lhs);
        mat_free(&AB); mat_free(&AC); mat_free(&rhs);
}

PROPERTY_TEST(prop_mul_identity, 200)
{
        (void)iter;
        size_t m = rand_dim(rng);
        size_t n = rand_dim(rng);

        Matrix A = mat_random(m, n, rng);
        Matrix Im = mat_identity(m);
        Matrix In = mat_identity(n);
        Matrix lhs = mat_create(m, n);
        Matrix rhs = mat_create(m, n);

        TEST_ASSERT(mat_mul(Im, A, &lhs) == 0);
        TEST_ASSERT(mat_mul(A, In, &rhs) == 0);
        TEST_ASSERT(mat_relative_equal(A, lhs, DEFAULT_RTOL,
                                        DEFAULT_ATOL) == 0);
        TEST_ASSERT(mat_relative_equal(A, rhs, DEFAULT_RTOL,
                                        DEFAULT_ATOL) == 0);

        mat_free(&A); mat_free(&Im); mat_free(&In);
        mat_free(&lhs); mat_free(&rhs);
}

PROPERTY_TEST(prop_mul_transpose_invariant, 100)
{
        (void)iter;
        size_t m = rand_dim(rng);
        size_t k = rand_dim(rng);
        size_t n = rand_dim(rng);

        Matrix A = mat_random(m, k, rng);
        Matrix B = mat_random(k, n, rng);

        Matrix AB = mat_create(m, n);
        Matrix ABT = mat_create(n, m);
        Matrix AT = mat_create(k, m);
        Matrix BT = mat_create(n, k);
        Matrix BTAT = mat_create(n, m);

        TEST_ASSERT(mat_mul(A, B, &AB) == 0);
        TEST_ASSERT(mat_transpose(AB, &ABT) == 0);
        TEST_ASSERT(mat_transpose(A, &AT) == 0);
        TEST_ASSERT(mat_transpose(B, &BT) == 0);
        TEST_ASSERT(mat_mul(BT, AT, &BTAT) == 0);
        TEST_ASSERT(mat_relative_equal(ABT, BTAT, DEFAULT_RTOL,
                                        DEFAULT_ATOL) == 0);

        mat_free(&A); mat_free(&B);
        mat_free(&AB); mat_free(&ABT);
        mat_free(&AT); mat_free(&BT); mat_free(&BTAT);
}

/* ============ SCALE ===================================================== */

PROPERTY_TEST(prop_scale_distributive, 200)
{
        (void)iter;
        size_t r = rand_dim(rng), c = rand_dim(rng);
        Matrix A = mat_random(r, c, rng);
        Matrix B = mat_random(r, c, rng);
        double alpha = rng_next_unit(rng) * 5.0;

        Matrix sum = mat_create(r, c);
        Matrix lhs = mat_create(r, c);
        Matrix sA = mat_create(r, c);
        Matrix sB = mat_create(r, c);
        Matrix rhs = mat_create(r, c);

        TEST_ASSERT(mat_add(A, B, &sum) == 0);
        TEST_ASSERT(mat_scale(sum, alpha, &lhs) == 0);
        TEST_ASSERT(mat_scale(A, alpha, &sA) == 0);
        TEST_ASSERT(mat_scale(B, alpha, &sB) == 0);
        TEST_ASSERT(mat_add(sA, sB, &rhs) == 0);
        TEST_ASSERT(mat_relative_equal(lhs, rhs, DEFAULT_RTOL,
                                        DEFAULT_ATOL) == 0);

        mat_free(&A); mat_free(&B);
        mat_free(&sum); mat_free(&lhs);
        mat_free(&sA); mat_free(&sB); mat_free(&rhs);
}

/* ============ TRANSPOSE ================================================== */

PROPERTY_TEST(prop_transpose_involution, 200)
{
        (void)iter;
        size_t r = rand_dim(rng), c = rand_dim(rng);
        Matrix A = mat_random(r, c, rng);
        Matrix T = mat_create(c, r);
        Matrix TT = mat_create(r, c);

        TEST_ASSERT(mat_transpose(A, &T) == 0);
        TEST_ASSERT(mat_transpose(T, &TT) == 0);
        TEST_ASSERT(mat_equal(A, TT) == 0); /* exact: pure data movement */

        mat_free(&A); mat_free(&T); mat_free(&TT);
}

/* ============ NORM ====================================================== */

PROPERTY_TEST(prop_norm_homogeneity, 200)
{
        (void)iter;
        size_t r = rand_dim(rng), c = rand_dim(rng);
        Matrix A = mat_random(r, c, rng);
        double alpha = rng_next_unit(rng) * 7.0;

        Matrix sA = mat_create(r, c);
        TEST_ASSERT(mat_scale(A, alpha, &sA) == 0);

        double na = mat_norm_l2(&A);
        double nsa = mat_norm_l2(&sA);
        TEST_ASSERT(rel_equal(nsa, fabs(alpha) * na,
                              DEFAULT_RTOL, DEFAULT_ATOL));

        mat_free(&A);
        mat_free(&sA);
}

PROPERTY_TEST(prop_norm_triangle, 200)
{
        (void)iter;
        size_t r = rand_dim(rng), c = rand_dim(rng);
        Matrix A = mat_random(r, c, rng);
        Matrix B = mat_random(r, c, rng);
        Matrix sum = mat_create(r, c);
        TEST_ASSERT(mat_add(A, B, &sum) == 0);

        double na = mat_norm_l2(&A);
        double nb = mat_norm_l2(&B);
        double nsum = mat_norm_l2(&sum);
        /* Slack for FP roundoff. */
        TEST_ASSERT(nsum <= na + nb + DEFAULT_ATOL +
                            DEFAULT_RTOL * (na + nb));

        mat_free(&A); mat_free(&B); mat_free(&sum);
}

/* ============ DOT ======================================================= */

PROPERTY_TEST(prop_dot_self_norm_squared, 200)
{
        (void)iter;
        size_t r = rand_dim(rng), c = rand_dim(rng);
        Matrix A = mat_random(r, c, rng);
        double dot = mat_dot(A, A);
        double n = mat_norm_l2(&A);
        TEST_ASSERT(rel_equal(dot, n * n, DEFAULT_RTOL, DEFAULT_ATOL));
        mat_free(&A);
}

PROPERTY_TEST(prop_dot_cauchy_schwarz, 200)
{
        (void)iter;
        size_t r = rand_dim(rng), c = rand_dim(rng);
        Matrix A = mat_random(r, c, rng);
        Matrix B = mat_random(r, c, rng);
        double dot = mat_dot(A, B);
        double na = mat_norm_l2(&A);
        double nb = mat_norm_l2(&B);
        TEST_ASSERT(fabs(dot) <= na * nb + DEFAULT_ATOL +
                                   DEFAULT_RTOL * na * nb);
        mat_free(&A); mat_free(&B);
}

/* ============ TRACE ===================================================== */

PROPERTY_TEST(prop_trace_cyclic, 100)
{
        (void)iter;
        size_t m = rand_dim(rng);
        size_t n = rand_dim(rng);
        Matrix A = mat_random(m, n, rng);
        Matrix B = mat_random(n, m, rng);

        Matrix AB = mat_create(m, m);
        Matrix BA = mat_create(n, n);
        TEST_ASSERT(mat_mul(A, B, &AB) == 0);
        TEST_ASSERT(mat_mul(B, A, &BA) == 0);

        TEST_ASSERT(rel_equal(mat_trace(&AB), mat_trace(&BA),
                              DEFAULT_RTOL, DEFAULT_ATOL));

        mat_free(&A); mat_free(&B);
        mat_free(&AB); mat_free(&BA);
}

/* ============ DET / INV (square, well-conditioned) ====================== */

PROPERTY_TEST(prop_det_product_rule, 60)
{
        (void)iter;
        size_t n = rng_next_size(rng, 1, 8);
        Matrix A = mat_random_diagdom(n, rng);
        Matrix B = mat_random_diagdom(n, rng);
        Matrix AB = mat_create(n, n);
        TEST_ASSERT(mat_mul(A, B, &AB) == 0);

        double dA = mat_det(&A);
        double dB = mat_det(&B);
        double dAB = mat_det(&AB);
        /* det grows multiplicatively; loosen rtol a touch for n up to 7. */
        TEST_ASSERT(rel_equal(dAB, dA * dB, 1e-7, 1e-10));

        mat_free(&A); mat_free(&B); mat_free(&AB);
}

PROPERTY_TEST(prop_inv_left_and_right, 60)
{
        (void)iter;
        size_t n = rng_next_size(rng, 1, 8);
        Matrix A = mat_random_diagdom(n, rng);
        Matrix invA = mat_create(n, n);
        TEST_ASSERT(mat_inv(A, &invA) == 0);

        Matrix Lprod = mat_create(n, n);
        Matrix Rprod = mat_create(n, n);
        TEST_ASSERT(mat_mul(A, invA, &Lprod) == 0);
        TEST_ASSERT(mat_mul(invA, A, &Rprod) == 0);

        Matrix I = mat_identity(n);
        TEST_ASSERT(mat_relative_equal(Lprod, I, 1e-7, 1e-10) == 0);
        TEST_ASSERT(mat_relative_equal(Rprod, I, 1e-7, 1e-10) == 0);

        mat_free(&A); mat_free(&invA);
        mat_free(&Lprod); mat_free(&Rprod);
        mat_free(&I);
}

PROPERTY_TEST(prop_inv_involution, 60)
{
        (void)iter;
        size_t n = rng_next_size(rng, 1, 8);
        Matrix A = mat_random_diagdom(n, rng);
        Matrix invA = mat_create(n, n);
        Matrix invInvA = mat_create(n, n);

        TEST_ASSERT(mat_inv(A, &invA) == 0);
        TEST_ASSERT(mat_inv(invA, &invInvA) == 0);
        TEST_ASSERT(mat_relative_equal(A, invInvA, 1e-7, 1e-10) == 0);

        mat_free(&A); mat_free(&invA); mat_free(&invInvA);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running property/fuzz tests ===\n\n");

        run_test(prop_add_commutative, "prop_add_commutative");
        run_test(prop_add_associative, "prop_add_associative");
        run_test(prop_sub_then_add_round_trip,
                 "prop_sub_then_add_round_trip");

        run_test(prop_mul_associative, "prop_mul_associative");
        run_test(prop_mul_distributive_left,
                 "prop_mul_distributive_left");
        run_test(prop_mul_identity, "prop_mul_identity");
        run_test(prop_mul_transpose_invariant,
                 "prop_mul_transpose_invariant");

        run_test(prop_scale_distributive, "prop_scale_distributive");
        run_test(prop_transpose_involution, "prop_transpose_involution");

        run_test(prop_norm_homogeneity, "prop_norm_homogeneity");
        run_test(prop_norm_triangle, "prop_norm_triangle");

        run_test(prop_dot_self_norm_squared, "prop_dot_self_norm_squared");
        run_test(prop_dot_cauchy_schwarz, "prop_dot_cauchy_schwarz");

        run_test(prop_trace_cyclic, "prop_trace_cyclic");

        run_test(prop_det_product_rule, "prop_det_product_rule");
        run_test(prop_inv_left_and_right, "prop_inv_left_and_right");
        run_test(prop_inv_involution, "prop_inv_involution");

        fprintf(stdout, "\n=== All property tests passed ===\n\n");
        return EXIT_SUCCESS;
}
