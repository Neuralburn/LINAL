/*
 * @file test_mat_scale.c
 * @brief Unit tests for matrix scaling.
 */

#include "test_harness.h"

TEST_CASE(test_mat_scale_success)
{
        Matrix m = mat_create(3, 3);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 3, 3,
                    (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == 0);

        TEST_ASSERT(approx_equal(result.data[0], 2.0));
        TEST_ASSERT(approx_equal(result.data[1], 4.0));
        TEST_ASSERT(approx_equal(result.data[2], 6.0));
        TEST_ASSERT(approx_equal(result.data[3], 8.0));
        TEST_ASSERT(approx_equal(result.data[4], 10.0));
        TEST_ASSERT(approx_equal(result.data[5], 12.0));
        TEST_ASSERT(approx_equal(result.data[6], 14.0));
        TEST_ASSERT(approx_equal(result.data[7], 16.0));
        TEST_ASSERT(approx_equal(result.data[8], 18.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_zero)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 0.0, &result);

        TEST_ASSERT(result_code == 0);
        for (size_t i = 0; i < 4; i++) {
                TEST_ASSERT(result.data[i] == 0.0);
        }

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_negative)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, -1.5, &result);

        TEST_ASSERT(result_code == 0);
        TEST_ASSERT(approx_equal(result.data[0], -1.5));
        TEST_ASSERT(approx_equal(result.data[1], -3.0));
        TEST_ASSERT(approx_equal(result.data[2], -4.5));
        TEST_ASSERT(approx_equal(result.data[3], -6.0));

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_null_input)
{
        Matrix m = {.rows = 2, .cols = 2, .data = NULL};
        Matrix result = mat_create(2, 2);

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&result);
}

TEST_CASE(test_mat_scale_null_result)
{
        Matrix m = mat_create(2, 2);
        Matrix result = {.rows = 2, .cols = 2, .data = NULL};

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
}

TEST_CASE(test_mat_scale_null_result_ptr)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(2, 2);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 2.0, NULL);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_dimension_mismatch)
{
        Matrix m = mat_create(2, 2);
        Matrix result = mat_create(3, 3);

        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        int result_code = mat_scale(m, 2.0, &result);

        TEST_ASSERT(result_code == -1);

        mat_free(&m);
        mat_free(&result);
}

TEST_CASE(test_mat_scale_alias_result_eq_input)
{
        Matrix m = mat_create(2, 2);
        init_matrix(&m, 2, 2, (double[]){1.0, 2.0, 3.0, 4.0});

        Matrix aliased = {.rows = 2, .cols = 2, .data = m.data};
        TEST_ASSERT(mat_scale(m, 5.0, &aliased) == -1);
        TEST_ASSERT(approx_equal(m.data[0], 1.0));
        TEST_ASSERT(approx_equal(m.data[3], 4.0));

        mat_free(&m);
}

TEST_CASE(test_mat_scale_one_is_identity)
{
        Matrix m = mat_create(3, 3);
        Matrix r = mat_create(3, 3);
        init_matrix(&m, 3, 3,
                    (double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});
        TEST_ASSERT(mat_scale(m, 1.0, &r) == 0);
        TEST_ASSERT(mat_equal(m, r) == 0);
        mat_free(&m);
        mat_free(&r);
}

TEST_CASE(test_mat_scale_neg_one_flips_sign)
{
        Matrix m = mat_create(2, 2);
        Matrix r = mat_create(2, 2);
        init_matrix(&m, 2, 2, (double[]){1.0, -2.0, 3.0, -4.0});
        TEST_ASSERT(mat_scale(m, -1.0, &r) == 0);
        for (size_t i = 0; i < 4; i++) {
                TEST_ASSERT(approx_equal(r.data[i], -m.data[i]));
        }
        mat_free(&m);
        mat_free(&r);
}

/* Distributivity: alpha * (A + B) == alpha*A + alpha*B. */
TEST_CASE(test_mat_scale_distributivity)
{
        Matrix A = mat_create(2, 3);
        Matrix B = mat_create(2, 3);
        init_matrix(&A, 2, 3, (double[]){1, 2, 3, 4, 5, 6});
        init_matrix(&B, 2, 3, (double[]){-1, 0, 1, 2, -2, 3});
        double alpha = 2.5;

        Matrix sum = mat_create(2, 3);
        Matrix lhs = mat_create(2, 3);
        Matrix sA = mat_create(2, 3);
        Matrix sB = mat_create(2, 3);
        Matrix rhs = mat_create(2, 3);

        TEST_ASSERT(mat_add(A, B, &sum) == 0);
        TEST_ASSERT(mat_scale(sum, alpha, &lhs) == 0);
        TEST_ASSERT(mat_scale(A, alpha, &sA) == 0);
        TEST_ASSERT(mat_scale(B, alpha, &sB) == 0);
        TEST_ASSERT(mat_add(sA, sB, &rhs) == 0);

        TEST_ASSERT(mat_equal(lhs, rhs) == 0);

        mat_free(&A); mat_free(&B);
        mat_free(&sum); mat_free(&lhs);
        mat_free(&sA); mat_free(&sB); mat_free(&rhs);
}

int
main(void)
{
        fprintf(stdout, "\n=== Running mat_scale tests ===\n\n");

        run_test(test_mat_scale_success, "test_mat_scale_success");
        run_test(test_mat_scale_zero, "test_mat_scale_zero");
        run_test(test_mat_scale_negative, "test_mat_scale_negative");
        run_test(test_mat_scale_null_input, "test_mat_scale_null_input");
        run_test(test_mat_scale_null_result, "test_mat_scale_null_result");
        run_test(test_mat_scale_null_result_ptr,
                 "test_mat_scale_null_result_ptr");
        run_test(test_mat_scale_dimension_mismatch,
                 "test_mat_scale_dimension_mismatch");

        run_test(test_mat_scale_alias_result_eq_input,
                 "test_mat_scale_alias_result_eq_input");
        run_test(test_mat_scale_one_is_identity,
                 "test_mat_scale_one_is_identity");
        run_test(test_mat_scale_neg_one_flips_sign,
                 "test_mat_scale_neg_one_flips_sign");
        run_test(test_mat_scale_distributivity,
                 "test_mat_scale_distributivity");

        fprintf(stdout, "\n=== All mat_scale tests passed ===\n\n");
        return EXIT_SUCCESS;
}
