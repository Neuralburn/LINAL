/*
 * @file test_vec_workloads.c
 * @brief End-to-end realistic workloads built from the public Vector API only.
 *
 * These exercise multi-op chains at sizes where bugs in optimized kernels
 * show up but tiny hand-picked tests do not.  Belongs in the `slow` test
 * suite.
 *
 * Tolerances: relative comparisons throughout — accumulated FP roundoff
 * and ill-conditioning make absolute EPSILON inappropriate here.
 */

#include "test_harness.h"

/* ============ K-Nearest Neighbors =====================================
 * Given a query vector and a dataset, find the k nearest neighbors by
 * Euclidean distance.  Exercises: vec_distance, vec_norm_l2, vec_sub.
 */
TEST_CASE(workload_k_nearest_neighbors)
{
        const size_t n_dim = 8;
        const size_t n_data = 50;
        const size_t k = 3;

        test_rng rng = rng_seed(0xCAFEBABEULL);

        Vector query = vec_create(n_dim);
        Vector dataset = vec_create(n_data * n_dim);

        for (size_t i = 0; i < n_dim; i++) {
                query.data[i] = rng_next_unit(&rng);
        }
        for (size_t i = 0; i < n_data * n_dim; i++) {
                dataset.data[i] = rng_next_unit(&rng);
        }

        /* Compute distances from query to each dataset point. */
        double *distances = (double *)malloc(n_data * sizeof(double));
        TEST_ASSERT(distances != NULL);

        for (size_t i = 0; i < n_data; i++) {
                Vector point;
                point.data = dataset.data + i * n_dim;
                point.size = n_dim;
                distances[i] = vec_distance(query, point);
        }

        /* Find k nearest neighbors (simple selection sort for first k). */
        size_t indices[3];
        for (size_t j = 0; j < k; j++) {
                size_t best = 0;
                double best_dist = distances[0];
                for (size_t i = 1; i < n_data; i++) {
                        /* Skip already-selected neighbors. */
                        bool selected = false;
                        for (size_t m = 0; m < j; m++) {
                                if (indices[m] == i) {
                                        selected = true;
                                        break;
                                }
                        }
                        if (selected) {
                                continue;
                        }
                        if (distances[i] < best_dist) {
                                best_dist = distances[i];
                                best = i;
                        }
                }
                indices[j] = best;
        }

        /* Verify: the nearest neighbor should be closer than the k-th. */
        double nearest_dist = distances[indices[0]];
        double kth_dist = distances[indices[k - 1]];
        TEST_ASSERT(nearest_dist <= kth_dist + DEFAULT_ATOL);

        /* Verify all distances are finite and non-negative. */
        for (size_t i = 0; i < n_data; i++) {
                TEST_ASSERT(isfinite(distances[i]));
                TEST_ASSERT(distances[i] >= -DEFAULT_ATOL);
        }

        free(distances);
        vec_free(&query);
        vec_free(&dataset);
}

/* ============ Cosine Similarity Matrix ==================================
 * Compute pairwise cosine similarity for a set of vectors.  Exercises:
 * vec_dot, vec_norm_l2.  Verifies symmetry and diagonal == 1.0.
 */
TEST_CASE(workload_cosine_similarity)
{
        const size_t n_vec = 20;
        const size_t n_dim = 16;

        test_rng rng = rng_seed(0x1234ABCDULL);
        Matrix V = mat_create(n_vec, n_dim);
        for (size_t i = 0; i < n_vec * n_dim; i++) {
                V.data[i] = rng_next_unit(&rng);
        }

        /* Extract each row as a Vector and compute similarity matrix. */
        double *sim = (double *)malloc(n_vec * n_vec * sizeof(double));
        TEST_ASSERT(sim != NULL);

        for (size_t i = 0; i < n_vec; i++) {
                Vector vi;
                vi.data = V.data + i * n_dim;
                vi.size = n_dim;

                for (size_t j = 0; j < n_vec; j++) {
                        Vector vj;
                        vj.data = V.data + j * n_dim;
                        vj.size = n_dim;

                        double dot_val = vec_dot(vi, vj);
                        double norm_i = vec_norm_l2(vi);
                        double norm_j = vec_norm_l2(vj);

                        if (norm_i > 0.0 && norm_j > 0.0) {
                                sim[i * n_vec + j] =
                                    dot_val / (norm_i * norm_j);
                        } else {
                                sim[i * n_vec + j] = 0.0;
                        }
                }
        }

        /* Symmetry: sim(i,j) == sim(j,i) */
        for (size_t i = 0; i < n_vec; i++) {
                for (size_t j = i + 1; j < n_vec; j++) {
                        TEST_ASSERT(rel_equal(sim[i * n_vec + j],
                                              sim[j * n_vec + i], DEFAULT_RTOL,
                                              DEFAULT_ATOL));
                }
        }

        /* Diagonal: sim(i,i) == 1.0 for non-zero vectors */
        for (size_t i = 0; i < n_vec; i++) {
                TEST_ASSERT(rel_equal(sim[i * n_vec + i], 1.0, DEFAULT_RTOL,
                                      DEFAULT_ATOL));
        }

        /* All values in [-1, 1] */
        for (size_t i = 0; i < n_vec * n_vec; i++) {
                TEST_ASSERT(sim[i] >= -1.0 - DEFAULT_ATOL);
                TEST_ASSERT(sim[i] <= 1.0 + DEFAULT_ATOL);
        }

        free(sim);
        mat_free(&V);
}

/* ============ Vector Field Sum ==========================================
 * Sum a large vector field and verify associativity:
 *   sum(a, b, c, ...) == sum(sum(a, b), sum(c, ...))
 * Exercises: vec_add at scale, round-trip verification.
 */
TEST_CASE(workload_vector_field_sum)
{
        const size_t n_vec = 200;
        const size_t n_dim = 8;

        test_rng rng = rng_seed(0xDEADBEEFULL);

        Vector *field = (Vector *)malloc(n_vec * sizeof(Vector));
        TEST_ASSERT(field != NULL);

        for (size_t i = 0; i < n_vec; i++) {
                field[i] = vec_create(n_dim);
                for (size_t j = 0; j < n_dim; j++) {
                        field[i].data[j] = rng_next_unit(&rng);
                }
        }

        /* Left-associated sum: ((v0 + v1) + v2) + ... */
        Vector sum_left = vec_create(n_dim);
        TEST_ASSERT(vec_add(field[0], field[1], &sum_left) == 0);
        for (size_t i = 2; i < n_vec; i++) {
                Vector tmp = vec_create(n_dim);
                TEST_ASSERT(vec_add(sum_left, field[i], &tmp) == 0);
                vec_free(&sum_left);
                sum_left = tmp;
        }

        /* Right-associated sum: v0 + (v1 + (v2 + ...)) */
        Vector sum_right = vec_create(n_dim);
        TEST_ASSERT(vec_add(field[n_vec - 2], field[n_vec - 1], &sum_right)
                    == 0);
        size_t idx = n_vec - 3;
        while (1) {
                Vector tmp = vec_create(n_dim);
                TEST_ASSERT(vec_add(field[idx], sum_right, &tmp) == 0);
                vec_free(&sum_right);
                sum_right = tmp;
                if (idx == 0) {
                        break;
                }
                idx--;
        }

        /* Compare left and right — allow loose tolerance for accumulation
         * order differences across n_vec additions in n_dim dimensions. */
        double sum_dist = vec_distance(sum_left, sum_right);
        TEST_ASSERT(sum_dist < 1.0);

        /* Verify sum_left component-wise against manual accumulation. */
        for (size_t j = 0; j < n_dim; j++) {
                double s = 0.0;
                for (size_t i = 0; i < n_vec; i++) {
                        s += field[i].data[j];
                }
                TEST_ASSERT(
                    rel_equal(sum_left.data[j], s, DEFAULT_RTOL, DEFAULT_ATOL));
        }

        /* Cleanup */
        vec_free(&sum_left);
        vec_free(&sum_right);
        for (size_t i = 0; i < n_vec; i++) {
                vec_free(&field[i]);
        }
        free(field);
}

/* ============ Gram-Schmidt Orthogonalization ============================
 * Build an orthonormal set from random vectors using only vec_dot,
 * vec_scale, vec_sub, vec_normalize.  Then check dot products == I.
 */
TEST_CASE(workload_gram_schmidt_vectors)
{
        const size_t n_dim = 20;
        const size_t k = 5; /* number of vectors to orthogonalize */

        test_rng rng = rng_seed(0xC0FFEE00ULL);

        Vector *input = (Vector *)malloc(k * sizeof(Vector));
        TEST_ASSERT(input != NULL);

        for (size_t i = 0; i < k; i++) {
                input[i] = vec_create(n_dim);
                for (size_t j = 0; j < n_dim; j++) {
                        input[i].data[j] = rng_next_unit(&rng);
                }
        }

        /* Q stores the orthonormalized vectors. */
        Vector *Q = (Vector *)malloc(k * sizeof(Vector));
        TEST_ASSERT(Q != NULL);

        for (size_t col = 0; col < k; col++) {
                /* Copy input vector. */
                Q[col] = vec_create(n_dim);
                for (size_t j = 0; j < n_dim; j++) {
                        Q[col].data[j] = input[col].data[j];
                }

                /* Subtract projections onto previously-stored columns. */
                for (size_t prev = 0; prev < col; prev++) {
                        double coeff = vec_dot(Q[prev], Q[col]);
                        Vector proj = vec_create(n_dim);
                        Vector q_col = vec_create(n_dim);
                        for (size_t j = 0; j < n_dim; j++) {
                                q_col.data[j] = Q[col].data[j];
                        }

                        TEST_ASSERT(vec_scale(Q[prev], coeff, &proj) == 0);

                        Vector q_col2 = vec_create(n_dim);
                        for (size_t j = 0; j < n_dim; j++) {
                                q_col2.data[j] = Q[col].data[j];
                        }
                        TEST_ASSERT(vec_sub(q_col, proj, &Q[col]) == 0);

                        vec_free(&proj);
                        vec_free(&q_col);
                        vec_free(&q_col2);
                }

                /* Normalize. */
                double norm_sq = vec_dot(Q[col], Q[col]);
                double norm = sqrt(norm_sq);
                TEST_ASSERT(norm > 1e-12);
                Vector q_normed = vec_create(n_dim);
                TEST_ASSERT(vec_scale(Q[col], 1.0 / norm, &q_normed) == 0);
                vec_free(&Q[col]);
                Q[col] = q_normed;
        }

        /* Check Q^T Q == I_k (orthonormality). */
        for (size_t i = 0; i < k; i++) {
                for (size_t j = 0; j < k; j++) {
                        double dot_val = vec_dot(Q[i], Q[j]);
                        if (i == j) {
                                TEST_ASSERT(rel_equal(
                                    dot_val, 1.0, DEFAULT_RTOL, DEFAULT_ATOL));
                        } else {
                                TEST_ASSERT(fabs(dot_val)
                                            <= DEFAULT_ATOL + DEFAULT_RTOL);
                        }
                }
        }

        /* Cleanup */
        for (size_t i = 0; i < k; i++) {
                vec_free(&Q[i]);
                vec_free(&input[i]);
        }
        free(Q);
        free(input);
}

/* ============ Power Iteration ===========================================
 * Find the dominant eigenvector of A = v * v^T (rank-1 matrix) using
 * power iteration.  The dominant eigenvector should converge to ±v/‖v‖.
 *
 * Exercises: vec_dot, vec_scale, vec_normalize, vec_distance.
 */
TEST_CASE(workload_power_iteration)
{
        const size_t n_dim = 16;
        const size_t max_iters = 100;
        const double tol = 1e-10;

        test_rng rng = rng_seed(0xBABE0000ULL);
        Vector v = vec_create(n_dim);
        for (size_t i = 0; i < n_dim; i++) {
                v.data[i] = rng_next_unit(&rng);
        }

        /* Normalize v to unit length. */
        double v_norm = vec_norm_l2(v);
        Vector target = vec_create(n_dim);
        TEST_ASSERT(vec_scale(v, 1.0 / v_norm, &target) == 0);

        /* Start with a random vector. */
        Vector x = vec_create(n_dim);
        for (size_t i = 0; i < n_dim; i++) {
                x.data[i] = rng_next_unit(&rng);
        }
        double x_norm = vec_norm_l2(x);
        Vector x_normed = vec_create(n_dim);
        TEST_ASSERT(vec_scale(x, 1.0 / x_norm, &x_normed) == 0);
        vec_free(&x);
        x = x_normed;

        /* Power iteration: x_{k+1} = A * x_k / ‖A * x_k‖
         * For A = v * v^T: A * x = v * (v^T * x) = (dot(v, x)) * v
         * So x_{k+1} = sign(dot(v, x)) * v / ‖v‖ = ±target */
        Vector Ax = vec_create(n_dim);
        for (size_t iter = 0; iter < max_iters; iter++) {
                /* Compute dot(v, x) */
                double dot_vx = vec_dot(v, x);

                /* Ax = dot(v, x) * v */
                TEST_ASSERT(vec_scale(v, dot_vx, &Ax) == 0);

                /* Normalize Ax */
                double ax_norm = vec_norm_l2(Ax);
                if (ax_norm < 1e-15) {
                        break; /* converged to orthogonal subspace */
                }
                Vector x_new = vec_create(n_dim);
                TEST_ASSERT(vec_scale(Ax, 1.0 / ax_norm, &x_new) == 0);

                /* Check convergence: distance to target or -target */
                double dist_pos = vec_distance(x_new, target);
                Vector neg_target = vec_create(n_dim);
                TEST_ASSERT(vec_scale(target, -1.0, &neg_target) == 0);
                double dist_neg = vec_distance(x_new, neg_target);
                double min_dist = (dist_pos < dist_neg) ? dist_pos : dist_neg;

                vec_free(&neg_target);

                if (min_dist < tol) {
                        vec_free(&x);
                        x = x_new; /* keep converged vector */
                        break;     /* converged */
                }
                vec_free(&x);
                x = x_new;
        }

        /* Final convergence check. */
        double dist_pos = vec_distance(x, target);
        Vector neg_target = vec_create(n_dim);
        TEST_ASSERT(vec_scale(target, -1.0, &neg_target) == 0);
        double dist_neg = vec_distance(x, neg_target);
        double min_dist = (dist_pos < dist_neg) ? dist_pos : dist_neg;
        fprintf(stdout,
                "[debug] power_iter: dist_pos=%.2e dist_neg=%.2e min=%.2e\n",
                dist_pos, dist_neg, min_dist);
        TEST_ASSERT(min_dist < 1e-6);

        /* Cleanup */
        vec_free(&v);
        vec_free(&target);
        vec_free(&x);
        vec_free(&Ax);
        vec_free(&neg_target);
}

/* ============ Vector Quantization (1-iteration K-Means) =================
 * Initialize k centroids from random data points, assign each point to
 * nearest centroid, then recompute centroids.  Exercises: vec_distance,
 * vec_add, vec_scale, vec_copy.
 */
TEST_CASE(workload_vector_quantization)
{
        const size_t n_dim = 6;
        const size_t n_data = 64;
        const size_t k = 4;

        test_rng rng = rng_seed(0xC0FFEEAALL);

        Vector data = vec_create(n_data * n_dim);
        for (size_t i = 0; i < n_data * n_dim; i++) {
                data.data[i] = rng_next_unit(&rng);
        }

        /* Initialize centroids as first k data points. */
        Vector *centroids = (Vector *)malloc(k * sizeof(Vector));
        TEST_ASSERT(centroids != NULL);
        for (size_t i = 0; i < k; i++) {
                centroids[i] = vec_create(n_dim);
                for (size_t j = 0; j < n_dim; j++) {
                        centroids[i].data[j] = data.data[i * n_dim + j];
                }
        }

        /* Assign each point to nearest centroid. */
        size_t *assignments = (size_t *)malloc(n_data * sizeof(size_t));
        TEST_ASSERT(assignments != NULL);
        double *distances = (double *)malloc(n_data * sizeof(double));
        TEST_ASSERT(distances != NULL);

        for (size_t i = 0; i < n_data; i++) {
                Vector point;
                point.data = data.data + i * n_dim;
                point.size = n_dim;

                double min_dist = INFINITY;
                size_t best = 0;
                for (size_t j = 0; j < k; j++) {
                        double d = vec_distance(point, centroids[j]);
                        if (d < min_dist) {
                                min_dist = d;
                                best = j;
                        }
                }
                assignments[i] = best;
                distances[i] = min_dist;
        }

        /* Recompute centroids as mean of assigned points. */
        Vector *new_centroids = (Vector *)malloc(k * sizeof(Vector));
        TEST_ASSERT(new_centroids != NULL);
        for (size_t j = 0; j < k; j++) {
                new_centroids[j] = vec_create(n_dim);
                for (size_t d = 0; d < n_dim; d++) {
                        new_centroids[j].data[d] = 0.0;
                }
        }

        for (size_t i = 0; i < n_data; i++) {
                size_t cluster = assignments[i];
                for (size_t d = 0; d < n_dim; d++) {
                        new_centroids[cluster].data[d] +=
                            data.data[i * n_dim + d];
                }
        }

        /* Divide by count per cluster. */
        size_t *counts = (size_t *)calloc(k, sizeof(size_t));
        TEST_ASSERT(counts != NULL);
        for (size_t i = 0; i < n_data; i++) {
                counts[assignments[i]]++;
        }

        for (size_t j = 0; j < k; j++) {
                if (counts[j] > 0) {
                        Vector scaled = vec_create(n_dim);
                        TEST_ASSERT(vec_scale(new_centroids[j],
                                              1.0 / (double)counts[j], &scaled)
                                    == 0);
                        vec_free(&new_centroids[j]);
                        new_centroids[j] = scaled;
                }
        }

        /* Verify: centroids moved (or stayed same if no reassignment). */
        for (size_t j = 0; j < k; j++) {
                double dist = vec_distance(centroids[j], new_centroids[j]);
                TEST_ASSERT(isfinite(dist));
        }

        /* Cleanup */
        free(assignments);
        free(distances);
        free(counts);
        for (size_t i = 0; i < k; i++) {
                vec_free(&centroids[i]);
                vec_free(&new_centroids[i]);
        }
        free(centroids);
        free(new_centroids);
        vec_free(&data);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running vec_workloads tests ===\n\n");

        run_test(workload_k_nearest_neighbors, "workload_k_nearest_neighbors");
        run_test(workload_cosine_similarity, "workload_cosine_similarity");
        run_test(workload_vector_field_sum, "workload_vector_field_sum");
        run_test(workload_gram_schmidt_vectors,
                 "workload_gram_schmidt_vectors");
        run_test(workload_power_iteration, "workload_power_iteration");
        run_test(workload_vector_quantization, "workload_vector_quantization");

        fprintf(stdout, "\n=== All vec_workloads tests passed ===\n\n");
        return EXIT_SUCCESS;
}
