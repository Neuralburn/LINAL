/*
 * @file test_mat_mul.c
 * @brief Unit tests for matrix multiplication.
 */

#include "test_harness.h"

TEST_CASE(test_mat_mul_success_square)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == 0);

        /* [1 2] * [5 6] = [19 22] */
        /* [3 4]   [7 8]   [43 50] */
        TEST_ASSERT(approx_equal(result.data[0], 19.0));
        TEST_ASSERT(approx_equal(result.data[1], 22.0));
        TEST_ASSERT(approx_equal(result.data[2], 43.0));
        TEST_ASSERT(approx_equal(result.data[3], 50.0));

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_success_rectangular)
{
        Matrix a = mat_create(2, 3);
        Matrix b = mat_create(3, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
        init_matrix(&b, 3, 2, (double[]){7.0, 8.0, 9.0, 10.0, 11.0, 12.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == 0);

        TEST_ASSERT(approx_equal(result.data[0], 58.0));
        TEST_ASSERT(approx_equal(result.data[1], 64.0));
        TEST_ASSERT(approx_equal(result.data[2], 139.0));
        TEST_ASSERT(approx_equal(result.data[3], 154.0));

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_identity)
{
        Matrix a = mat_create(3, 3);
        Matrix b = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        /* Create identity matrix A */
        init_matrix(&a, 3, 3,
                    (double[]){1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});
        /* Create identity matrix B */
        init_matrix(&b, 3, 3,
                    (double[]){1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == 0);

        /* A * I = A, so result should be identity */
        for (size_t i = 0; i < 9; i++) {
                if (i % 3 == i / 3) {
                        TEST_ASSERT(approx_equal(result.data[i], 1.0));
                } else {
                        TEST_ASSERT(approx_equal(result.data[i], 0.0));
                }
        }

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_dimension_mismatch)
{
        Matrix a = mat_create(2, 3);
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
        init_matrix(&b, 2, 2, (double[]){7.0, 8.0, 9.0, 10.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_empty_matrix)
{
        Matrix a = mat_create(0, 3);
        Matrix b = mat_create(3, 2);
        Matrix result = mat_create(0, 2);

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_null_result)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        Matrix result = {.rows = 2, .cols = 2, .data = NULL};

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&b);
}

TEST_CASE(test_mat_mul_null_input_a)
{
        Matrix a = {.rows = 2, .cols = 2, .data = NULL};
        Matrix b = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&b);
        mat_free(&result);
}

TEST_CASE(test_mat_mul_null_input_b)
{
        Matrix a = mat_create(2, 2);
        Matrix b = {.rows = 2, .cols = 2, .data = NULL};
        Matrix result = mat_create(2, 2);

        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_mul(a, b, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&a);
        mat_free(&result);
}

/* ---------- Aliasing rejection ----------------------------------------- */

TEST_CASE(test_mat_mul_alias_result_eq_a)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});
        Matrix aliased = {.rows = 2, .cols = 2, .data = a.data};
        TEST_ASSERT(mat_mul(a, b, &aliased) == -1);
        TEST_ASSERT(approx_equal(a.data[0], 1.0));
        mat_free(&a);
        mat_free(&b);
}

TEST_CASE(test_mat_mul_alias_result_eq_b)
{
        Matrix a = mat_create(2, 2);
        Matrix b = mat_create(2, 2);
        init_matrix(&a, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&b, 2, 2, (double[]){5.0, 6.0, 7.0, 8.0});
        Matrix aliased = {.rows = 2, .cols = 2, .data = b.data};
        TEST_ASSERT(mat_mul(a, b, &aliased) == -1);
        TEST_ASSERT(approx_equal(b.data[0], 5.0));
        mat_free(&a);
        mat_free(&b);
}

/* ---------- Vector-shape products -------------------------------------- */

/* Outer product: (m x 1) * (1 x n) -> (m x n). */
TEST_CASE(test_mat_mul_outer_product)
{
        Matrix u = mat_create(3, 1);
        Matrix v = mat_create(1, 4);
        Matrix r = mat_create(3, 4);

        init_matrix(&u, 3, 1, (double[]){1.0, 2.0, 3.0});
        init_matrix(&v, 1, 4, (double[]){10.0, 20.0, 30.0, 40.0});

        TEST_ASSERT(mat_mul(u, v, &r) == 0);

        for (size_t i = 0; i < 3; i++) {
                for (size_t j = 0; j < 4; j++) {
                        double expected = u.data[i] * v.data[j];
                        TEST_ASSERT(approx_equal(r.data[i * 4 + j], expected));
                }
        }
        mat_free(&u);
        mat_free(&v);
        mat_free(&r);
}

/* Inner product as 1x1 result: (1 x n) * (n x 1). */
TEST_CASE(test_mat_mul_inner_product)
{
        Matrix u = mat_create(1, 4);
        Matrix v = mat_create(4, 1);
        Matrix r = mat_create(1, 1);

        init_matrix(&u, 1, 4, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&v, 4, 1, (double[]){5.0, 6.0, 7.0, 8.0});

        TEST_ASSERT(mat_mul(u, v, &r) == 0);
        TEST_ASSERT(approx_equal(r.data[0], 1*5 + 2*6 + 3*7 + 4*8));
        mat_free(&u);
        mat_free(&v);
        mat_free(&r);
}

/* Matrix * column vector -> column vector. */
TEST_CASE(test_mat_mul_matrix_times_vector)
{
        Matrix A = mat_create(3, 3);
        Matrix x = mat_create(3, 1);
        Matrix y = mat_create(3, 1);
        init_matrix(&A, 3, 3,
                    (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});
        init_matrix(&x, 3, 1, (double[]){1.0, 0.0, -1.0});

        TEST_ASSERT(mat_mul(A, x, &y) == 0);
        /* Column 0 - column 2 of A. */
        TEST_ASSERT(approx_equal(y.data[0], 1.0 - 3.0));
        TEST_ASSERT(approx_equal(y.data[1], 4.0 - 6.0));
        TEST_ASSERT(approx_equal(y.data[2], 7.0 - 9.0));

        mat_free(&A);
        mat_free(&x);
        mat_free(&y);
}

/* ---------- Algebraic invariants on hand-picked inputs ----------------- */

/* (AB)^T == B^T A^T. */
TEST_CASE(test_mat_mul_transpose_invariant)
{
        Matrix A = mat_create(2, 3);
        Matrix B = mat_create(3, 4);
        init_matrix(&A, 2, 3, (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
        init_matrix(&B, 3, 4,
                    (double[]){1.0, 0.0, -1.0, 2.0,
                               2.0, 1.0,  0.0, 3.0,
                               3.0, 2.0,  1.0, 4.0});

        Matrix AB = mat_create(2, 4);
        Matrix ABT = mat_create(4, 2);
        TEST_ASSERT(mat_mul(A, B, &AB) == 0);
        TEST_ASSERT(mat_transpose(AB, &ABT) == 0);

        Matrix AT = mat_create(3, 2);
        Matrix BT = mat_create(4, 3);
        Matrix BTAT = mat_create(4, 2);
        TEST_ASSERT(mat_transpose(A, &AT) == 0);
        TEST_ASSERT(mat_transpose(B, &BT) == 0);
        TEST_ASSERT(mat_mul(BT, AT, &BTAT) == 0);

        TEST_ASSERT(mat_equal(ABT, BTAT) == 0);

        mat_free(&A);
        mat_free(&B);
        mat_free(&AB);
        mat_free(&ABT);
        mat_free(&AT);
        mat_free(&BT);
        mat_free(&BTAT);
}

/* Associativity: (AB)C == A(BC). */
TEST_CASE(test_mat_mul_associativity)
{
        Matrix A = mat_create(2, 3);
        Matrix B = mat_create(3, 4);
        Matrix C = mat_create(4, 2);
        init_matrix(&A, 2, 3, (double[]){1, 2, 3, 4, 5, 6});
        init_matrix(&B, 3, 4, (double[]){1, 0, -1, 2,
                                          2, 1, 0, 3,
                                          0, -1, 4, 1});
        init_matrix(&C, 4, 2, (double[]){1, 2, 3, 4, 5, 6, 7, 8});

        Matrix AB = mat_create(2, 4);
        Matrix ABC = mat_create(2, 2);
        Matrix BC = mat_create(3, 2);
        Matrix A_BC = mat_create(2, 2);
        TEST_ASSERT(mat_mul(A, B, &AB) == 0);
        TEST_ASSERT(mat_mul(AB, C, &ABC) == 0);
        TEST_ASSERT(mat_mul(B, C, &BC) == 0);
        TEST_ASSERT(mat_mul(A, BC, &A_BC) == 0);

        TEST_ASSERT(mat_equal(ABC, A_BC) == 0);

        mat_free(&A);
        mat_free(&B);
        mat_free(&C);
        mat_free(&AB);
        mat_free(&ABC);
        mat_free(&BC);
        mat_free(&A_BC);
}

/* Left distributivity: A(B + C) == AB + AC. */
TEST_CASE(test_mat_mul_distributivity_left)
{
        Matrix A = mat_create(2, 2);
        Matrix B = mat_create(2, 3);
        Matrix C = mat_create(2, 3);
        init_matrix(&A, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});
        init_matrix(&B, 2, 3, (double[]){1, 2, 3, 4, 5, 6});
        init_matrix(&C, 2, 3, (double[]){-1, 0, 1, 2, -2, 3});

        Matrix BplusC = mat_create(2, 3);
        Matrix lhs = mat_create(2, 3);
        Matrix AB = mat_create(2, 3);
        Matrix AC = mat_create(2, 3);
        Matrix rhs = mat_create(2, 3);
        TEST_ASSERT(mat_add(B, C, &BplusC) == 0);
        TEST_ASSERT(mat_mul(A, BplusC, &lhs) == 0);
        TEST_ASSERT(mat_mul(A, B, &AB) == 0);
        TEST_ASSERT(mat_mul(A, C, &AC) == 0);
        TEST_ASSERT(mat_add(AB, AC, &rhs) == 0);
        TEST_ASSERT(mat_equal(lhs, rhs) == 0);

        mat_free(&A); mat_free(&B); mat_free(&C);
        mat_free(&BplusC); mat_free(&lhs);
        mat_free(&AB); mat_free(&AC); mat_free(&rhs);
}

/* Larger square N=128 — exercises optimized parallel path. */
TEST_CASE(test_mat_mul_large_square_identity)
{
        size_t n = 128;
        Matrix A = mat_create(n, n);
        Matrix I = mat_identity(n);
        Matrix R = mat_create(n, n);
        for (size_t i = 0; i < n * n; i++) {
                A.data[i] = (double)((i * 31 + 7) % 17) - 8.0;
        }
        TEST_ASSERT(mat_mul(A, I, &R) == 0);
        for (size_t i = 0; i < n * n; i++) {
                TEST_ASSERT(approx_equal(R.data[i], A.data[i]));
        }
        mat_free(&A);
        mat_free(&I);
        mat_free(&R);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_mul tests ===\n\n");

        run_test(test_mat_mul_success_square, "test_mat_mul_success_square");
        run_test(test_mat_mul_success_rectangular,
                 "test_mat_mul_success_rectangular");
        run_test(test_mat_mul_identity, "test_mat_mul_identity");
        run_test(test_mat_mul_dimension_mismatch,
                 "test_mat_mul_dimension_mismatch");
        run_test(test_mat_mul_empty_matrix, "test_mat_mul_empty_matrix");
        run_test(test_mat_mul_null_result, "test_mat_mul_null_result");
        run_test(test_mat_mul_null_input_a, "test_mat_mul_null_input_a");
        run_test(test_mat_mul_null_input_b, "test_mat_mul_null_input_b");

        run_test(test_mat_mul_alias_result_eq_a,
                 "test_mat_mul_alias_result_eq_a");
        run_test(test_mat_mul_alias_result_eq_b,
                 "test_mat_mul_alias_result_eq_b");
        run_test(test_mat_mul_outer_product, "test_mat_mul_outer_product");
        run_test(test_mat_mul_inner_product, "test_mat_mul_inner_product");
        run_test(test_mat_mul_matrix_times_vector,
                 "test_mat_mul_matrix_times_vector");
        run_test(test_mat_mul_transpose_invariant,
                 "test_mat_mul_transpose_invariant");
        run_test(test_mat_mul_associativity, "test_mat_mul_associativity");
        run_test(test_mat_mul_distributivity_left,
                 "test_mat_mul_distributivity_left");
        run_test(test_mat_mul_large_square_identity,
                 "test_mat_mul_large_square_identity");

        fprintf(stdout, "\n=== All mat_mul tests passed ===\n\n");
        return EXIT_SUCCESS;
}
