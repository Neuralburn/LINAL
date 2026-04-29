/*
 * @file test_workloads.c
 * @brief End-to-end realistic workloads built from the public API only.
 *
 * These exercise multi-op chains at sizes where bugs in optimized kernels
 * show up but tiny hand-picked tests do not (e.g. an n=4 tiling bug that
 * passes but breaks at n=257).  Belongs in the `slow` test suite.
 *
 * Tolerances: relative comparisons throughout — accumulated FP roundoff
 * and ill-conditioning make absolute EPSILON inappropriate here.
 */

#include "test_harness.h"

/* Convenience: solve y = A^-1 * b for square A.  Caller frees x. */
static int
solve_via_inverse(const Matrix A, const Matrix b, Matrix *x)
{
        Matrix invA = mat_create(A.rows, A.cols);
        if (mat_inv(A, &invA) != 0) {
                mat_free(&invA);
                return -1;
        }
        int rc = mat_mul(invA, b, x);
        mat_free(&invA);
        return rc;
}

/* ============ Ordinary Least Squares ===================================
 * Synthesize y = X * beta_true + noise; recover beta via normal equations:
 *     beta_hat = (X^T X)^-1 X^T y
 *
 * With moderate noise and n >> d the recovered coefficients should be
 * close to beta_true.  This exercises mat_mul, mat_transpose, mat_inv
 * end-to-end on a realistic shape.
 */
TEST_CASE(workload_ols_regression)
{
        const size_t n = 200;  /* observations */
        const size_t d = 10;   /* features */
        const double noise_scale = 1e-3;

        test_rng rng = rng_seed(0xA5A5A5A5ULL);

        Matrix X = mat_create(n, d);
        Matrix beta_true = mat_create(d, 1);
        Matrix y = mat_create(n, 1);

        /* Random design matrix and true coefficients in roughly unit range. */
        for (size_t i = 0; i < n * d; i++) {
                X.data[i] = rng_next_unit(&rng);
        }
        for (size_t i = 0; i < d; i++) {
                beta_true.data[i] = rng_next_unit(&rng) * 3.0;
        }
        /* y = X*beta_true + small noise */
        TEST_ASSERT(mat_mul(X, beta_true, &y) == 0);
        for (size_t i = 0; i < n; i++) {
                y.data[i] += rng_next_unit(&rng) * noise_scale;
        }

        /* Normal equations. */
        Matrix XT = mat_create(d, n);
        Matrix XtX = mat_create(d, d);
        Matrix XtY = mat_create(d, 1);
        Matrix beta_hat = mat_create(d, 1);

        TEST_ASSERT(mat_transpose(X, &XT) == 0);
        TEST_ASSERT(mat_mul(XT, X, &XtX) == 0);
        TEST_ASSERT(mat_mul(XT, y, &XtY) == 0);
        TEST_ASSERT(solve_via_inverse(XtX, XtY, &beta_hat) == 0);

        /* Recovery quality: relative error in coefficient vector. */
        Matrix diff = mat_create(d, 1);
        TEST_ASSERT(mat_sub(beta_hat, beta_true, &diff) == 0);
        double err = mat_norm_l2(&diff);
        double sig = mat_norm_l2(&beta_true);
        /* With noise_scale=1e-3 and n/d=20, expected error <~ 1e-3 relative. */
        TEST_ASSERT(err / sig < 1e-2);

        mat_free(&X); mat_free(&beta_true); mat_free(&y);
        mat_free(&XT); mat_free(&XtX); mat_free(&XtY);
        mat_free(&beta_hat); mat_free(&diff);
}

/* ============ Ridge Regression (Tikhonov) ==============================
 * beta = (X^T X + lambda*I)^-1 X^T y.  Confirms scaled-identity addition
 * + inversion path on a moderately conditioned matrix.
 */
TEST_CASE(workload_ridge_regression)
{
        const size_t n = 80;
        const size_t d = 12;
        const double lambda = 0.5;

        test_rng rng = rng_seed(0x12340000ULL);
        Matrix X = mat_create(n, d);
        Matrix beta_true = mat_create(d, 1);
        Matrix y = mat_create(n, 1);

        for (size_t i = 0; i < n * d; i++) {
                X.data[i] = rng_next_unit(&rng);
        }
        for (size_t i = 0; i < d; i++) {
                beta_true.data[i] = rng_next_unit(&rng) * 2.0;
        }
        TEST_ASSERT(mat_mul(X, beta_true, &y) == 0);

        /* (X^T X + lambda*I) */
        Matrix XT = mat_create(d, n);
        Matrix XtX = mat_create(d, d);
        Matrix lambdaI_scaled = mat_create(d, d);
        Matrix gram = mat_create(d, d);

        TEST_ASSERT(mat_transpose(X, &XT) == 0);
        TEST_ASSERT(mat_mul(XT, X, &XtX) == 0);

        Matrix I = mat_identity(d);
        TEST_ASSERT(mat_scale(I, lambda, &lambdaI_scaled) == 0);
        TEST_ASSERT(mat_add(XtX, lambdaI_scaled, &gram) == 0);

        Matrix XtY = mat_create(d, 1);
        Matrix beta_hat = mat_create(d, 1);
        TEST_ASSERT(mat_mul(XT, y, &XtY) == 0);
        TEST_ASSERT(solve_via_inverse(gram, XtY, &beta_hat) == 0);

        /* Ridge biases coefficients toward zero — just verify finiteness
         * and that the solution shrinks (||beta_hat|| <= ||beta_true||). */
        double nh = mat_norm_l2(&beta_hat);
        double nt = mat_norm_l2(&beta_true);
        TEST_ASSERT(isfinite(nh));
        TEST_ASSERT(nh <= nt + DEFAULT_ATOL);

        mat_free(&X); mat_free(&beta_true); mat_free(&y);
        mat_free(&XT); mat_free(&XtX);
        mat_free(&lambdaI_scaled); mat_free(&gram); mat_free(&I);
        mat_free(&XtY); mat_free(&beta_hat);
}

/* ============ Scaled dot-product attention (no softmax) ================
 * Q, K, V at transformer-head sizes.  Verifies:
 *   1. matmul associativity holds at realistic dims:
 *      (Q K^T) V == Q (K^T V)   (algebraic identity)
 *   2. all outputs finite for plausible inputs.
 */
TEST_CASE(workload_attention_block)
{
        const size_t T = 64;     /* sequence length */
        const size_t d = 64;     /* head dim */
        const size_t dv = 64;    /* value dim */
        const double scale = 1.0 / sqrt((double)d);

        test_rng rng = rng_seed(0xA770ULL);
        Matrix Q = mat_random(T, d, &rng);
        Matrix K = mat_random(T, d, &rng);
        Matrix V = mat_random(T, dv, &rng);

        Matrix KT = mat_create(d, T);
        Matrix QKT = mat_create(T, T);
        Matrix QKT_scaled = mat_create(T, T);
        Matrix out_left = mat_create(T, dv);

        TEST_ASSERT(mat_transpose(K, &KT) == 0);
        TEST_ASSERT(mat_mul(Q, KT, &QKT) == 0);
        TEST_ASSERT(mat_scale(QKT, scale, &QKT_scaled) == 0);
        TEST_ASSERT(mat_mul(QKT_scaled, V, &out_left) == 0);

        /* Right-associated path: scaling commutes with matmul, so
         *   scale*(Q*KT)*V == Q*(scale*(KT*V)) algebraically. */
        Matrix KtV = mat_create(d, dv);
        Matrix KtV_scaled = mat_create(d, dv);
        Matrix out_right = mat_create(T, dv);
        TEST_ASSERT(mat_mul(KT, V, &KtV) == 0);
        TEST_ASSERT(mat_scale(KtV, scale, &KtV_scaled) == 0);
        TEST_ASSERT(mat_mul(Q, KtV_scaled, &out_right) == 0);

        /* Allow looser rtol — accumulation order differs. */
        TEST_ASSERT(mat_relative_equal(out_left, out_right,
                                        1e-7, 1e-10) == 0);

        /* Finiteness sanity. */
        for (size_t i = 0; i < T * dv; i++) {
                TEST_ASSERT(isfinite(out_left.data[i]));
        }

        mat_free(&Q); mat_free(&K); mat_free(&V);
        mat_free(&KT); mat_free(&QKT); mat_free(&QKT_scaled);
        mat_free(&out_left);
        mat_free(&KtV); mat_free(&KtV_scaled); mat_free(&out_right);
}

/* ============ Two-layer MLP forward pass ================================
 * Out = (X W1 + B1) W2 + B2 — linear-only (no activation; we lack one).
 * Tests bias broadcasting via row-replicated bias matrices and chained
 * mat_mul + mat_add.
 */
static void
fill_row_replicated(Matrix *M, const double *bias_row)
{
        for (size_t i = 0; i < M->rows; i++) {
                memcpy(M->data + i * M->cols, bias_row,
                       M->cols * sizeof(double));
        }
}

TEST_CASE(workload_mlp_forward)
{
        const size_t batch = 32;
        const size_t in_dim = 128;
        const size_t hid = 256;
        const size_t out_dim = 10;

        test_rng rng = rng_seed(0xC0DEFEEDULL);

        Matrix X = mat_random(batch, in_dim, &rng);
        Matrix W1 = mat_random(in_dim, hid, &rng);
        Matrix W2 = mat_random(hid, out_dim, &rng);

        double *b1_row = (double *)malloc(hid * sizeof(double));
        double *b2_row = (double *)malloc(out_dim * sizeof(double));
        TEST_ASSERT(b1_row && b2_row);
        for (size_t i = 0; i < hid; i++) {
                b1_row[i] = rng_next_unit(&rng);
        }
        for (size_t i = 0; i < out_dim; i++) {
                b2_row[i] = rng_next_unit(&rng);
        }
        Matrix B1 = mat_create(batch, hid);
        Matrix B2 = mat_create(batch, out_dim);
        fill_row_replicated(&B1, b1_row);
        fill_row_replicated(&B2, b2_row);

        Matrix XW1 = mat_create(batch, hid);
        Matrix H = mat_create(batch, hid);
        Matrix HW2 = mat_create(batch, out_dim);
        Matrix Y = mat_create(batch, out_dim);

        TEST_ASSERT(mat_mul(X, W1, &XW1) == 0);
        TEST_ASSERT(mat_add(XW1, B1, &H) == 0);
        TEST_ASSERT(mat_mul(H, W2, &HW2) == 0);
        TEST_ASSERT(mat_add(HW2, B2, &Y) == 0);

        TEST_ASSERT(Y.rows == batch && Y.cols == out_dim);
        for (size_t i = 0; i < batch * out_dim; i++) {
                TEST_ASSERT(isfinite(Y.data[i]));
        }

        /* Spot check: bias is added independently per row.  The first
         * column of (HW2 + B2) minus the first column of HW2 must equal
         * b2_row[0] for every row. */
        for (size_t i = 0; i < batch; i++) {
                double diff = Y.data[i * out_dim] - HW2.data[i * out_dim];
                TEST_ASSERT(rel_equal(diff, b2_row[0],
                                      DEFAULT_RTOL, DEFAULT_ATOL));
        }

        free(b1_row); free(b2_row);
        mat_free(&X); mat_free(&W1); mat_free(&W2);
        mat_free(&B1); mat_free(&B2);
        mat_free(&XW1); mat_free(&H);
        mat_free(&HW2); mat_free(&Y);
}

/* ============ Markov chain stationary distribution ======================
 * Build a row-stochastic n×n transition matrix P and raise it to a high
 * power.  Verify rows of P^k converge: rows are nearly identical.
 */
TEST_CASE(workload_markov_stationary)
{
        const size_t n = 16;
        test_rng rng = rng_seed(0xDEADULL);

        Matrix P = mat_create(n, n);
        for (size_t i = 0; i < n; i++) {
                double row_sum = 0.0;
                /* Positive entries -> ergodic chain. */
                for (size_t j = 0; j < n; j++) {
                        double v = 0.5 + 0.5 * (rng_next_unit(&rng) + 1.0);
                        P.data[i * n + j] = v;
                        row_sum += v;
                }
                for (size_t j = 0; j < n; j++) {
                        P.data[i * n + j] /= row_sum;
                }
        }

        /* Pk_buf alternates between two buffers as we square. */
        Matrix A = mat_create(n, n);
        Matrix B = mat_create(n, n);
        TEST_ASSERT(mat_copy(P, &A) == 0);

        /* P^(2^7) = P^128 — overkill for n=16 ergodic. */
        for (int s = 0; s < 7; s++) {
                TEST_ASSERT(mat_mul(A, A, &B) == 0);
                Matrix tmp = A;
                A = B;
                B = tmp;
        }

        /* Each row of P^k should be the stationary distribution.  Compare
         * row 0 to all other rows. */
        for (size_t i = 1; i < n; i++) {
                for (size_t j = 0; j < n; j++) {
                        TEST_ASSERT(rel_equal(A.data[i * n + j],
                                              A.data[j],
                                              1e-6, 1e-9));
                }
        }
        /* Row sums are still 1 (stochastic preserved under multiplication). */
        for (size_t i = 0; i < n; i++) {
                double s = 0.0;
                for (size_t j = 0; j < n; j++) {
                        s += A.data[i * n + j];
                }
                TEST_ASSERT(rel_equal(s, 1.0, 1e-9, 1e-12));
        }

        mat_free(&P);
        mat_free(&A);
        mat_free(&B);
}

/* ============ Gram-Schmidt orthogonalization ==========================
 * Build Q (n x k) from random columns using only mat_dot/mat_scale/mat_sub.
 * Then check Q^T Q == I_k.
 */
TEST_CASE(workload_gram_schmidt)
{
        const size_t n = 30;
        const size_t k = 8;
        test_rng rng = rng_seed(0xC0FFEE00ULL);

        /* Build A column-by-column as random n x 1 vectors stored in Q. */
        Matrix Q = mat_create(n, k);
        for (size_t col = 0; col < k; col++) {
                /* Random column. */
                Matrix v = mat_create(n, 1);
                for (size_t i = 0; i < n; i++) {
                        v.data[i] = rng_next_unit(&rng);
                }

                /* Subtract projections onto previously-stored columns. */
                for (size_t prev = 0; prev < col; prev++) {
                        /* q_prev as n x 1 view (we materialize a copy because
                         * mat_dot/mat_sub want owned matrices and the API
                         * is value-semantic). */
                        Matrix q_prev = mat_create(n, 1);
                        for (size_t i = 0; i < n; i++) {
                                q_prev.data[i] = Q.data[i * k + prev];
                        }
                        double coeff = mat_dot(q_prev, v);
                        Matrix proj = mat_create(n, 1);
                        Matrix v_new = mat_create(n, 1);
                        TEST_ASSERT(mat_scale(q_prev, coeff, &proj) == 0);
                        TEST_ASSERT(mat_sub(v, proj, &v_new) == 0);
                        TEST_ASSERT(mat_copy(v_new, &v) == 0);
                        mat_free(&q_prev);
                        mat_free(&proj);
                        mat_free(&v_new);
                }

                /* Normalize. */
                double norm_sq = mat_dot(v, v);
                double norm = sqrt(norm_sq);
                TEST_ASSERT(norm > 1e-12);
                Matrix v_normed = mat_create(n, 1);
                TEST_ASSERT(mat_scale(v, 1.0 / norm, &v_normed) == 0);

                /* Store as column `col` of Q. */
                for (size_t i = 0; i < n; i++) {
                        Q.data[i * k + col] = v_normed.data[i];
                }

                mat_free(&v);
                mat_free(&v_normed);
        }

        /* Check Q^T Q == I_k. */
        Matrix QT = mat_create(k, n);
        Matrix QtQ = mat_create(k, k);
        Matrix Ik = mat_identity(k);
        TEST_ASSERT(mat_transpose(Q, &QT) == 0);
        TEST_ASSERT(mat_mul(QT, Q, &QtQ) == 0);
        TEST_ASSERT(mat_relative_equal(QtQ, Ik, 1e-8, 1e-10) == 0);

        mat_free(&Q);
        mat_free(&QT);
        mat_free(&QtQ);
        mat_free(&Ik);
}

/* ============ Sample covariance matrix ==================================
 * Given X (n x d), build C = ((X - mu)^T (X - mu)) / (n-1).
 * Verify symmetry, non-negative diagonal (variances), and trace ==
 * sum of per-feature variances computed independently.
 */
TEST_CASE(workload_covariance_matrix)
{
        const size_t n = 100;
        const size_t d = 8;
        test_rng rng = rng_seed(0xC0FA000ULL);

        Matrix X = mat_create(n, d);
        for (size_t i = 0; i < n * d; i++) {
                X.data[i] = rng_next_unit(&rng) * 2.0;
        }

        /* Per-column mean. */
        double mu[8] = {0};
        for (size_t j = 0; j < d; j++) {
                double s = 0.0;
                for (size_t i = 0; i < n; i++) {
                        s += X.data[i * d + j];
                }
                mu[j] = s / (double)n;
        }

        /* Centered = X - mu (broadcast). */
        Matrix Mu = mat_create(n, d);
        for (size_t i = 0; i < n; i++) {
                for (size_t j = 0; j < d; j++) {
                        Mu.data[i * d + j] = mu[j];
                }
        }
        Matrix Xc = mat_create(n, d);
        TEST_ASSERT(mat_sub(X, Mu, &Xc) == 0);

        /* Cov = Xc^T Xc / (n-1). */
        Matrix XcT = mat_create(d, n);
        Matrix Cov_unscaled = mat_create(d, d);
        Matrix Cov = mat_create(d, d);
        TEST_ASSERT(mat_transpose(Xc, &XcT) == 0);
        TEST_ASSERT(mat_mul(XcT, Xc, &Cov_unscaled) == 0);
        TEST_ASSERT(mat_scale(Cov_unscaled, 1.0 / (double)(n - 1), &Cov) == 0);

        /* Symmetry. */
        Matrix CovT = mat_create(d, d);
        TEST_ASSERT(mat_transpose(Cov, &CovT) == 0);
        TEST_ASSERT(mat_relative_equal(Cov, CovT,
                                        DEFAULT_RTOL, DEFAULT_ATOL) == 0);

        /* Non-negative diagonal. */
        for (size_t i = 0; i < d; i++) {
                TEST_ASSERT(Cov.data[i * d + i] >= -DEFAULT_ATOL);
        }

        /* Trace == sum of per-feature variances computed independently. */
        double total_var = 0.0;
        for (size_t j = 0; j < d; j++) {
                double s = 0.0;
                for (size_t i = 0; i < n; i++) {
                        double dev = X.data[i * d + j] - mu[j];
                        s += dev * dev;
                }
                total_var += s / (double)(n - 1);
        }
        double tr = mat_trace(&Cov);
        TEST_ASSERT(rel_equal(tr, total_var, DEFAULT_RTOL, DEFAULT_ATOL));

        mat_free(&X);
        mat_free(&Mu); mat_free(&Xc);
        mat_free(&XcT); mat_free(&Cov_unscaled); mat_free(&Cov);
        mat_free(&CovT);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running workload tests ===\n\n");

        run_test(workload_ols_regression, "workload_ols_regression");
        run_test(workload_ridge_regression, "workload_ridge_regression");
        run_test(workload_attention_block, "workload_attention_block");
        run_test(workload_mlp_forward, "workload_mlp_forward");
        run_test(workload_markov_stationary, "workload_markov_stationary");
        run_test(workload_gram_schmidt, "workload_gram_schmidt");
        run_test(workload_covariance_matrix, "workload_covariance_matrix");

        fprintf(stdout, "\n=== All workload tests passed ===\n\n");
        return EXIT_SUCCESS;
}
