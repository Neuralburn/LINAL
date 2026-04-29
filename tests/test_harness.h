/*
 * @file test_harness.h
 * @brief Common test harness for LINAL unit tests.
 *
 * Provides:
 *   - TEST_ASSERT / TEST_PASS / TEST_CASE macros
 *   - approx_equal (absolute) and rel_equal (scale-aware) comparators
 *   - mat_equal (absolute) and mat_relative_equal (scale-aware) matrix compare
 *   - Seeded xorshift64 PRNG for reproducible random matrices
 *   - mat_random / mat_random_diagdom / mat_random_spd builders
 *   - PROPERTY_TEST macro for fuzz-style property checks with seed reporting
 */

#ifndef LINAL_TEST_HARNESS_H
#define LINAL_TEST_HARNESS_H

#include "linal.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================ ASSERTION & TEST MACROS ================================= */

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

/* ================ TOLERANCE COMPARATORS =================================== */

/* Absolute epsilon — for hand-crafted small inputs whose answer is exact. */
#define EPSILON 1e-10

/* Default relative/absolute tolerances for randomized / large-N tests.
 * rtol picked so a stable double-precision algorithm clearly passes;
 * atol prevents division-by-near-zero blowup in the relative test. */
#define DEFAULT_RTOL 1e-9
#define DEFAULT_ATOL 1e-12

static inline int
approx_equal(double a, double b)
{
        return fabs(a - b) < EPSILON;
}

/* numpy.allclose-style: |a-b| <= atol + rtol * max(|a|,|b|). */
static inline int
rel_equal(double a, double b, double rtol, double atol)
{
        double abs_a = fabs(a);
        double abs_b = fabs(b);
        double scale = abs_a > abs_b ? abs_a : abs_b;
        return fabs(a - b) <= atol + rtol * scale;
}

static int __attribute__((unused))
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

static int __attribute__((unused))
mat_relative_equal(const Matrix a, const Matrix b, double rtol, double atol)
{
        if (a.rows != b.rows || a.cols != b.cols) {
                return -1;
        }
        for (size_t i = 0; i < a.rows * a.cols; i++) {
                if (!rel_equal(a.data[i], b.data[i], rtol, atol)) {
                        return -1;
                }
        }
        return 0;
}

/* ================ MATRIX INITIALIZATION =================================== */

static void __attribute__((unused))
init_matrix(Matrix *m, size_t rows, size_t cols, const double *values)
{
        TEST_ASSERT(m->data != NULL);
        for (size_t i = 0; i < rows * cols; i++) {
                m->data[i] = values[i];
        }
}

/* ================ SEEDED PRNG ============================================= */
/*
 * xorshift64* — small, fast, deterministic. Good enough for fuzz-style
 * property tests; not cryptographic.
 */
typedef struct {
        uint64_t state;
} test_rng;

static inline test_rng
rng_seed(uint64_t seed)
{
        /* Avoid zero state which would lock xorshift at zero. */
        test_rng r = {seed ? seed : 0xDEADBEEFCAFEBABEULL};
        return r;
}

static inline uint64_t
rng_next_u64(test_rng *r)
{
        uint64_t x = r->state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        r->state = x;
        return x * 0x2545F4914F6CDD1DULL;
}

/* Uniform double in [-1, 1). */
static inline double
rng_next_unit(test_rng *r)
{
        /* 53 bits of mantissa entropy → divide by 2^52 then offset. */
        uint64_t bits = rng_next_u64(r) >> 11; /* 53 bits */
        double u = (double)bits * (1.0 / 9007199254740992.0); /* [0,1) */
        return 2.0 * u - 1.0;
}

/* Uniform integer in [lo, hi). */
static inline size_t
rng_next_size(test_rng *r, size_t lo, size_t hi)
{
        if (hi <= lo) {
                return lo;
        }
        return lo + (size_t)(rng_next_u64(r) % (uint64_t)(hi - lo));
}

/* ================ RANDOM MATRIX BUILDERS ================================== */

/* Fill with uniform [-1, 1). */
static void __attribute__((unused))
mat_random_fill(Matrix *m, test_rng *r)
{
        size_t n = m->rows * m->cols;
        for (size_t i = 0; i < n; i++) {
                m->data[i] = rng_next_unit(r);
        }
}

static Matrix __attribute__((unused))
mat_random(size_t rows, size_t cols, test_rng *r)
{
        Matrix m = mat_create(rows, cols);
        mat_random_fill(&m, r);
        return m;
}

/*
 * Strictly diagonally dominant n×n matrix:
 *   for each row i, |A[i][i]| > sum_{j!=i} |A[i][j]|.
 * Guaranteed non-singular and well-conditioned.
 */
static Matrix __attribute__((unused))
mat_random_diagdom(size_t n, test_rng *r)
{
        Matrix m = mat_create(n, n);
        for (size_t i = 0; i < n; i++) {
                double row_sum = 0.0;
                for (size_t j = 0; j < n; j++) {
                        if (j == i) {
                                continue;
                        }
                        double v = rng_next_unit(r);
                        m.data[i * n + j] = v;
                        row_sum += fabs(v);
                }
                /* Diagonal: sign-randomized magnitude > row off-diag sum. */
                double sign = (rng_next_u64(r) & 1u) ? 1.0 : -1.0;
                m.data[i * n + i] = sign * (row_sum + 1.0);
        }
        return m;
}

/*
 * Symmetric positive definite n×n via A = X^T X + n*I.
 * Useful for regression-style tests.  Caller owns returned matrix.
 */
static Matrix __attribute__((unused))
mat_random_spd(size_t n, test_rng *r)
{
        Matrix X = mat_random(n, n, r);
        Matrix XT = mat_create(n, n);
        Matrix XtX = mat_create(n, n);

        TEST_ASSERT(mat_transpose(X, &XT) == 0);
        TEST_ASSERT(mat_mul(XT, X, &XtX) == 0);

        /* Add n*I to ensure strict positive definiteness. */
        for (size_t i = 0; i < n; i++) {
                XtX.data[i * n + i] += (double)n;
        }

        mat_free(&X);
        mat_free(&XT);
        return XtX;
}

/* ================ TEST RUNNER ============================================= */

static void __attribute__((unused))
run_test(void (*test_func)(void), const char *name)
{
        test_func();
        TEST_PASS(name);
}

/* ================ PROPERTY TEST HELPERS =================================== */

/*
 * Read a uint64 from env var. Returns fallback on missing/invalid input.
 * Used for LINAL_FUZZ_SEED / LINAL_FUZZ_ITERS overrides in property tests.
 */
static inline uint64_t
test_env_u64(const char *name, uint64_t fallback)
{
        const char *s = getenv(name);
        if (!s || !*s) {
                return fallback;
        }
        char *end = NULL;
        unsigned long long v = strtoull(s, &end, 10);
        if (end == s) {
                return fallback;
        }
        return (uint64_t)v;
}

/*
 * PROPERTY_TEST(name, default_iters)
 *
 * Inside the body, `rng` and `iter` are in scope. On any TEST_ASSERT failure
 * the seed prints to stderr (via the wrapping test harness) so the failure
 * is reproducible by exporting LINAL_FUZZ_SEED.
 */
#define PROPERTY_TEST(name, default_iters)                                     \
        static void name##_body(test_rng *rng, size_t iter);                   \
        TEST_CASE(name)                                                        \
        {                                                                      \
                uint64_t seed =                                                \
                    test_env_u64("LINAL_FUZZ_SEED", 0xC0FFEEULL ^ __LINE__);   \
                size_t iters = (size_t)test_env_u64("LINAL_FUZZ_ITERS",        \
                                                    (default_iters));          \
                fprintf(stdout, "  [seed=0x%016llx iters=%zu] ",               \
                        (unsigned long long)seed, iters);                      \
                test_rng rng = rng_seed(seed);                                 \
                for (size_t iter = 0; iter < iters; iter++) {                  \
                        name##_body(&rng, iter);                               \
                }                                                              \
        }                                                                      \
        static void name##_body(test_rng *rng, size_t iter)

#endif /* LINAL_TEST_HARNESS_H */
