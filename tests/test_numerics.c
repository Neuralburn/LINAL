/*
 * @file test_numerics.c
 * @brief Numerical edge cases: special values, ill conditioning, mixed
 *        magnitudes.
 *
 * These tests document and lock in the *current* numerical behavior of
 * LINAL.  A regression in pivoting, scaling, or any of the optimized
 * kernels will move these results outside the asserted bounds.
 */

#include "test_harness.h"

#include <float.h>

/* ============ NaN / Inf propagation ===================================== */

TEST_CASE(test_nan_propagates_through_add)
{
        Matrix A = mat_create(2, 2);
        Matrix B = mat_create(2, 2);
        Matrix R = mat_create(2, 2);
        init_matrix(&A, 2, 2, (double[]){1.0, NAN, 3.0, 4.0});
        init_matrix(&B, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        TEST_ASSERT(mat_add(A, B, &R) == 0);
        TEST_ASSERT(isnan(R.data[1]));
        /* Other entries finite. */
        TEST_ASSERT(isfinite(R.data[0]) && isfinite(R.data[2])
                    && isfinite(R.data[3]));
        mat_free(&A); mat_free(&B); mat_free(&R);
}

TEST_CASE(test_inf_propagates_through_mul)
{
        Matrix A = mat_create(2, 2);
        Matrix B = mat_create(2, 2);
        Matrix R = mat_create(2, 2);
        init_matrix(&A, 2, 2, (double[]){INFINITY, 0.0, 0.0, 1.0});
        init_matrix(&B, 2, 2, (double[]){1.0, 0.0, 0.0, 1.0});
        TEST_ASSERT(mat_mul(A, B, &R) == 0);
        TEST_ASSERT(isinf(R.data[0]));
        TEST_ASSERT(approx_equal(R.data[3], 1.0));
        mat_free(&A); mat_free(&B); mat_free(&R);
}

TEST_CASE(test_inf_minus_inf_is_nan)
{
        Matrix A = mat_create(1, 1);
        Matrix B = mat_create(1, 1);
        Matrix R = mat_create(1, 1);
        A.data[0] = INFINITY;
        B.data[0] = INFINITY;
        TEST_ASSERT(mat_sub(A, B, &R) == 0);
        TEST_ASSERT(isnan(R.data[0]));
        mat_free(&A); mat_free(&B); mat_free(&R);
}

TEST_CASE(test_norm_of_nan_matrix_is_nan)
{
        Matrix A = mat_create(2, 2);
        init_matrix(&A, 2, 2, (double[]){1.0, NAN, 3.0, 4.0});
        TEST_ASSERT(isnan(mat_norm_l2(&A)));
        mat_free(&A);
}

TEST_CASE(test_dot_with_nan_is_nan)
{
        Matrix A = mat_create(2, 2);
        Matrix B = mat_create(2, 2);
        init_matrix(&A, 2, 2, (double[]){1.0, NAN, 3.0, 4.0});
        init_matrix(&B, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        TEST_ASSERT(isnan(mat_dot(A, B)));
        mat_free(&A); mat_free(&B);
}

TEST_CASE(test_scale_by_nan)
{
        Matrix A = mat_create(2, 2);
        Matrix R = mat_create(2, 2);
        init_matrix(&A, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        TEST_ASSERT(mat_scale(A, NAN, &R) == 0);
        for (size_t i = 0; i < 4; i++) {
                TEST_ASSERT(isnan(R.data[i]));
        }
        mat_free(&A); mat_free(&R);
}

/* ============ Plus/minus zero ========================================== */

TEST_CASE(test_signed_zero_through_add)
{
        Matrix A = mat_create(1, 1);
        Matrix B = mat_create(1, 1);
        Matrix R = mat_create(1, 1);
        A.data[0] = 0.0;
        B.data[0] = -0.0;
        TEST_ASSERT(mat_add(A, B, &R) == 0);
        /* IEEE: +0 + -0 = +0. */
        TEST_ASSERT(R.data[0] == 0.0);
        TEST_ASSERT(!signbit(R.data[0]));
        mat_free(&A); mat_free(&B); mat_free(&R);
}

/* ============ Denormals =============================================== */

TEST_CASE(test_denormal_addition)
{
        Matrix A = mat_create(1, 1);
        Matrix B = mat_create(1, 1);
        Matrix R = mat_create(1, 1);
        A.data[0] = DBL_MIN / 2.0;       /* denormal */
        B.data[0] = DBL_MIN / 2.0;
        TEST_ASSERT(mat_add(A, B, &R) == 0);
        TEST_ASSERT(approx_equal(R.data[0], DBL_MIN));
        mat_free(&A); mat_free(&B); mat_free(&R);
}

/* ============ Catastrophic cancellation ================================ */

TEST_CASE(test_mixed_magnitude_cancellation)
{
        /* (1e15 + 1) - 1e15 — classic cancellation; 1.0 should survive. */
        Matrix A = mat_create(1, 1);
        Matrix B = mat_create(1, 1);
        Matrix C = mat_create(1, 1);
        Matrix tmp = mat_create(1, 1);
        Matrix R = mat_create(1, 1);

        A.data[0] = 1e15;
        B.data[0] = 1.0;
        C.data[0] = 1e15;

        TEST_ASSERT(mat_add(A, B, &tmp) == 0);
        TEST_ASSERT(mat_sub(tmp, C, &R) == 0);
        TEST_ASSERT(approx_equal(R.data[0], 1.0));

        mat_free(&A); mat_free(&B); mat_free(&C);
        mat_free(&tmp); mat_free(&R);
}

/* ============ Hilbert matrix (ill-conditioned) ========================= */

static Matrix
hilbert(size_t n)
{
        Matrix H = mat_create(n, n);
        for (size_t i = 0; i < n; i++) {
                for (size_t j = 0; j < n; j++) {
                        H.data[i * n + j] = 1.0 / ((double)(i + j) + 1.0);
                }
        }
        return H;
}

/*
 * Hilbert matrices have catastrophic condition number that doubles roughly
 * every n. Locking the rough error band tells us if pivoting regresses.
 *
 * cond(H_4) ≈ 1.55e4   → ~4 digits
 * cond(H_6) ≈ 1.5e7    → ~9 digits left
 * cond(H_8) ≈ 1.5e10   → ~6 digits left
 *
 * We only assert generous upper bounds — tighter bounds would fail under
 * any harmless reordering of FP ops.
 */
TEST_CASE(test_hilbert_4_inversion)
{
        Matrix H = hilbert(4);
        Matrix invH = mat_create(4, 4);
        TEST_ASSERT(mat_inv(H, &invH) == 0);

        Matrix prod = mat_create(4, 4);
        Matrix I = mat_identity(4);
        TEST_ASSERT(mat_mul(H, invH, &prod) == 0);
        /* H_4 inverse should round-trip well — error well under 1. */
        TEST_ASSERT(mat_relative_equal(prod, I, 1e-8, 1e-10) == 0);
        mat_free(&H); mat_free(&invH); mat_free(&prod); mat_free(&I);
}

TEST_CASE(test_hilbert_6_inversion)
{
        Matrix H = hilbert(6);
        Matrix invH = mat_create(6, 6);
        TEST_ASSERT(mat_inv(H, &invH) == 0);

        Matrix prod = mat_create(6, 6);
        Matrix I = mat_identity(6);
        TEST_ASSERT(mat_mul(H, invH, &prod) == 0);
        /* Looser: cond(H_6) ~1e7, so ~7 digits lost from machine precision. */
        TEST_ASSERT(mat_relative_equal(prod, I, 1e-6, 1e-8) == 0);
        mat_free(&H); mat_free(&invH); mat_free(&prod); mat_free(&I);
}

TEST_CASE(test_hilbert_8_inversion)
{
        Matrix H = hilbert(8);
        Matrix invH = mat_create(8, 8);
        TEST_ASSERT(mat_inv(H, &invH) == 0);
        /* Just confirm the inversion succeeded and produced a finite result.
         * cond(H_8) is large enough that tighter tolerances are unstable. */
        for (size_t i = 0; i < 64; i++) {
                TEST_ASSERT(isfinite(invH.data[i]));
        }
        mat_free(&H); mat_free(&invH);
}

/* ============ Near-singular determinant =============================== */

TEST_CASE(test_near_singular_det_does_not_blow_up)
{
        /* Two near-parallel rows.  det should be small but finite. */
        Matrix A = mat_create(2, 2);
        A.data[0] = 1.0;
        A.data[1] = 2.0;
        A.data[2] = 1.0 + 1e-12;
        A.data[3] = 2.0 + 2e-12;
        double d = mat_det(&A);
        TEST_ASSERT(isfinite(d));
        TEST_ASSERT(fabs(d) < 1e-10);
        mat_free(&A);
}

TEST_CASE(test_singular_inv_returns_error)
{
        /* Near-zero pivot path. */
        Matrix A = mat_create(3, 3);
        init_matrix(&A, 3, 3,
                    (double[]){1.0, 2.0, 3.0,
                               2.0, 4.0, 6.0,        /* exactly 2*row0 */
                               7.0, 8.0, 9.0});
        Matrix invA = mat_create(3, 3);
        TEST_ASSERT(mat_inv(A, &invA) == -1);
        mat_free(&A);
        mat_free(&invA);
}

/* ============ Very large / very small uniform values =================== */

TEST_CASE(test_large_value_norm)
{
        Matrix A = mat_create(2, 2);
        for (size_t i = 0; i < 4; i++) {
                A.data[i] = 1e150;
        }
        double n = mat_norm_l2(&A);
        /* sqrt(4 * (1e150)^2) = 2e150.  Should not overflow. */
        TEST_ASSERT(isfinite(n));
        TEST_ASSERT(rel_equal(n, 2e150, DEFAULT_RTOL, 0.0));
        mat_free(&A);
}

TEST_CASE(test_small_value_norm)
{
        Matrix A = mat_create(2, 2);
        for (size_t i = 0; i < 4; i++) {
                A.data[i] = 1e-150;
        }
        double n = mat_norm_l2(&A);
        TEST_ASSERT(isfinite(n));
        TEST_ASSERT(rel_equal(n, 2e-150, DEFAULT_RTOL, 0.0));
        mat_free(&A);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running numerical edge case tests ===\n\n");

        run_test(test_nan_propagates_through_add,
                 "test_nan_propagates_through_add");
        run_test(test_inf_propagates_through_mul,
                 "test_inf_propagates_through_mul");
        run_test(test_inf_minus_inf_is_nan, "test_inf_minus_inf_is_nan");
        run_test(test_norm_of_nan_matrix_is_nan,
                 "test_norm_of_nan_matrix_is_nan");
        run_test(test_dot_with_nan_is_nan, "test_dot_with_nan_is_nan");
        run_test(test_scale_by_nan, "test_scale_by_nan");

        run_test(test_signed_zero_through_add,
                 "test_signed_zero_through_add");
        run_test(test_denormal_addition, "test_denormal_addition");
        run_test(test_mixed_magnitude_cancellation,
                 "test_mixed_magnitude_cancellation");

        run_test(test_hilbert_4_inversion, "test_hilbert_4_inversion");
        run_test(test_hilbert_6_inversion, "test_hilbert_6_inversion");
        run_test(test_hilbert_8_inversion, "test_hilbert_8_inversion");

        run_test(test_near_singular_det_does_not_blow_up,
                 "test_near_singular_det_does_not_blow_up");
        run_test(test_singular_inv_returns_error,
                 "test_singular_inv_returns_error");

        run_test(test_large_value_norm, "test_large_value_norm");
        run_test(test_small_value_norm, "test_small_value_norm");

        fprintf(stdout, "\n=== All numerical tests passed ===\n\n");
        return EXIT_SUCCESS;
}
