/**
 * @file linal.c
 * @brief Implementation of LINAL.
 */

/* ================ INCLUDES ================================================ */

#include "linal.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* ================ DEFINES ================================================= */

/* ================ STRUCTURES ============================================== */

/* ================ TYPEDEFS ================================================ */

/* ================ STATIC PROTOTYPES ======================================= */

/* ================ STATIC VARIABLES ======================================== */

/* ================ MACROS ================================================== */

/* ================ STATIC FUNCTIONS ======================================== */

/**
 * @brief Check if a pointer is valid to prevent null dereference errors.
 * @param ptr Pointer to test for NULL value
 * @return true if pointer is not NULL, false otherwise
 */
static inline bool
is_valid_matrix(const Matrix *m)
{
        return m != NULL && m->data != NULL;
}

/**
 * @brief Calculate total elements in matrix safely.
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Total number of elements as a size_t value
 */
static inline size_t
calc_total_elements(size_t rows, size_t cols)
{
        return rows * cols;
}

/**
 * @brief Memory allocation helper that ensures successful allocation.
 * @param size Number of bytes to allocate
 * @param name Name identifier for error messages (internal use)
 * @return Pointer to newly allocated memory, or NULL on failure
 */
static inline void *
alloc_safe(size_t size, const char *name)
{
        void *ptr = malloc(size);
        if (!ptr) {
                fprintf(stderr, "Memory allocation failed for %s\n",
                        name ? name : "unknown operation");
        }
        return ptr;
}

/* ================ GLOBAL FUNCTIONS ======================================== */

/**
 * @brief Initialize a Matrix struct and allocate storage for the data pointer.
 * The data is initialized to zero using memset for numerical stability.
 */
Matrix
mat_create(size_t r, size_t c)
{
        Matrix m = {0};
        m.data = NULL;

        if (r > 0 && c > 0) {
                size_t bytes_needed = r * c * sizeof(double);
                double *data_ptr = (double *)malloc(bytes_needed);

                if (!data_ptr) {
                        fprintf(stderr,
                                "Allocation failed creating %zu x %zu matrix\n",
                                r, c);
                        return m;
                }

                memset(data_ptr, 0.0, bytes_needed);
                m.data = data_ptr;
                m.rows = r;
                m.cols = c;
        } else {
                m.rows = r;
                m.cols = c;
        }

        return m;
}

/**
 * @brief Release memory allocated for a matrix.
 * After calling this, the matrix becomes invalid and should not be used.
 */
void
mat_free(Matrix *m)
{
        if (!m || !is_valid_matrix(m)) {
                return;
        }

        free(m->data);
        m->data = NULL;
        m->rows = 0;
        m->cols = 0;
}

/**
 * @brief Copy all data from source matrix to destination.
 * No reallocation occurs; assumes both matrices share the same dimensions.
 */
int
mat_copy(const Matrix src, Matrix *dest)
{
        if (!is_valid_matrix(&src) || !dest || !dest->data
            || src.rows != dest->rows || src.cols != dest->cols) {
                return -1;
        }

        size_t num_elements = src.rows * src.cols;
        memcpy(dest->data, src.data, num_elements * sizeof(double));
        return 0;
}

/**
 * @brief Perform element-wise addition of two matrices.
 * Both matrices must have identical dimensions for a successful result.
 */
int
mat_add(const Matrix a, const Matrix b, Matrix *result)
{
        if (!a.data || !b.data || !result || !result->data || a.rows != b.rows
            || a.cols != b.cols || a.rows != result->rows
            || a.cols != result->cols) {
                fprintf(
                    stderr,
                    "Addition error: Invalid matrix or dimension mismatch\n");
                return -1;
        }

        size_t count = a.rows * a.cols;

#if defined(_OPENMP)
        /* Parallelize only for large matrices to avoid thread overhead */
        if (count >= 1024) {
#pragma omp parallel for schedule(static)
                for (size_t i = 0; i < count; i++) {
                        result->data[i] = a.data[i] + b.data[i];
                }
        } else
#endif
        {
                for (size_t i = 0; i < count; i++) {
                        result->data[i] = a.data[i] + b.data[i];
                }
        }
        return 0;
}

/**
 * @brief Perform matrix multiplication: C = A * B
 * Optimizes loop ordering for cache locality during computation.
 */
__attribute__((optimize("O3")))
int
mat_mul(const Matrix a, const Matrix b, Matrix *result)
{
        if (!a.data || !b.data || !result || !result->data || a.rows == 0
            || a.cols == 0 || b.rows == 0 || b.cols == 0 || a.cols != b.rows) {
                fprintf(stderr, "Multiplication error: Invalid matrix or "
                                "dimension mismatch\n");
                return -1;
        }

        /* Zero-initialize result matrix first */
        memset(result->data, 0, result->rows * result->cols * sizeof(double));

        /* O3 matmul: parallel for large matrices, serial for small ones */
        if (a.rows <= 16) {
                /* Serial version: avoids OpenMP thread overhead for tiny matrices */
                for (size_t i = 0; i < a.rows; i++) {
                        double *r_row = result->data + i * result->cols;
                        for (size_t k = 0; k < a.cols; k++) {
                                double factor = a.data[i * a.cols + k];
                                const double *b_row = b.data + k * b.cols;
                                size_t j = 0;
                                for (; j + 7 < b.cols; j += 8) {
                                        r_row[j]     += factor * b_row[j];
                                        r_row[j + 1] += factor * b_row[j + 1];
                                        r_row[j + 2] += factor * b_row[j + 2];
                                        r_row[j + 3] += factor * b_row[j + 3];
                                        r_row[j + 4] += factor * b_row[j + 4];
                                        r_row[j + 5] += factor * b_row[j + 5];
                                        r_row[j + 6] += factor * b_row[j + 6];
                                        r_row[j + 7] += factor * b_row[j + 7];
                                }
                                for (; j < b.cols; j++) {
                                        r_row[j] += factor * b_row[j];
                                }
                        }
                }
        } else {
                /* Parallel version: OpenMP on i-loop with 8x unroll */
#pragma omp parallel for schedule(static)
                for (size_t i = 0; i < a.rows; i++) {
                        double *r_row = result->data + i * result->cols;
                        for (size_t k = 0; k < a.cols; k++) {
                                double factor = a.data[i * a.cols + k];
                                const double *b_row = b.data + k * b.cols;
                                size_t j = 0;
                                #pragma GCC ivdep
                                for (; j + 7 < b.cols; j += 8) {
                                        r_row[j]     += factor * b_row[j];
                                        r_row[j + 1] += factor * b_row[j + 1];
                                        r_row[j + 2] += factor * b_row[j + 2];
                                        r_row[j + 3] += factor * b_row[j + 3];
                                        r_row[j + 4] += factor * b_row[j + 4];
                                        r_row[j + 5] += factor * b_row[j + 5];
                                        r_row[j + 6] += factor * b_row[j + 6];
                                        r_row[j + 7] += factor * b_row[j + 7];
                                }
                                for (; j < b.cols; j++) {
                                        r_row[j] += factor * b_row[j];
                                }
                        }
                }
        }

        return 0;
}

/**
 * @brief Scale a matrix by a scalar factor.
 * @param m Input matrix to scale
 * @param scalar Scalar multiplier
 * @param result Output matrix containing the scaled values (must not alias m)
 * @return 0 on success, -1 if input is invalid
 */
int
mat_scale(const Matrix m, double scalar, Matrix *result)
{
        if (!m.data || !result || !result->data) {
                fprintf(stderr, "Scale error: Invalid matrix pointer\n");
                return -1;
        }

        if (m.rows != result->rows || m.cols != result->cols) {
                fprintf(stderr, "Scale error: Dimension mismatch\n");
                return -1;
        }

        size_t count = m.rows * m.cols;
#if defined(_OPENMP)
        if (count >= 1024) {
#pragma omp parallel for schedule(static)
                for (size_t i = 0; i < count; i++) {
                        result->data[i] = m.data[i] * scalar;
                }
        } else
#endif
        {
                for (size_t i = 0; i < count; i++) {
                        result->data[i] = m.data[i] * scalar;
                }
        }

        return 0;
}

/**
 * @brief Transpose a matrix (swap rows and columns).
 * @param m Input matrix to transpose
 * @param result Output matrix containing the transposed values (must have
 * dimensions m.cols x m.rows)
 * @return 0 on success, -1 if input is invalid or dimensions mismatch
 */
int
mat_transpose(const Matrix m, Matrix *result)
{
        if (!m.data || !result || !result->data) {
                fprintf(stderr, "Transpose error: Invalid matrix pointer\n");
                return -1;
        }

        if (m.rows != result->cols || m.cols != result->rows) {
                fprintf(stderr,
                        "Transpose error: Dimension mismatch (expected %zu x "
                        "%zu, got %zu x %zu)\n",
                        m.cols, m.rows, result->rows, result->cols);
                return -1;
        }

        for (size_t i = 0; i < m.rows; i++) {
                for (size_t j = 0; j < m.cols; j++) {
                        result->data[j * result->cols + i] =
                            m.data[i * m.cols + j];
                }
        }

        return 0;
}

/**
 * @brief Subtract two matrices element-wise (A - B).
 * Uses auto-vectorization with ivdep hint for serial path and OpenMP simd
 * parallelism for large matrices (≥4096 elements).
 * @param a First operand (minuend)
 * @param b Second operand (subtrahend)
 * @param result Output matrix containing the difference (must not alias a or b)
 * @return 0 on success, -1 on dimension mismatch or invalid result
 */
__attribute__((optimize("O3")))
int
mat_sub(const Matrix a, const Matrix b, Matrix *result)
{
        if (!a.data || !b.data || !result || !result->data || a.rows != b.rows
            || a.cols != b.cols || a.rows != result->rows
            || a.cols != result->cols) {
                fprintf(stderr, "Subtraction error: Invalid matrix or "
                                "dimension mismatch\n");
                return -1;
        }

        size_t count = a.rows * a.cols;

#if defined(_OPENMP)
        /* Parallelize only for large matrices to avoid thread overhead.
         * Threshold of 4096 elements (~64×64) ensures threads have enough work. */
        if (count >= 4096) {
#pragma omp parallel for simd
                for (size_t i = 0; i < count; i++) {
                        result->data[i] = a.data[i] - b.data[i];
                }
        } else
#endif
        {
                /* Let compiler auto-vectorize with ivdep hint */
                const double *A = a.data;
                const double *B = b.data;
                double *R       = result->data;
                #pragma GCC ivdep
                for (size_t i = 0; i < count; i++) {
                        R[i] = A[i] - B[i];
                }
        }
        return 0;
}

/**
 * @brief Get element at specified row and column.
 * @param m Matrix to access
 * @param row Row index (0-based)
 * @param col Column index (0-based)
 * @return Element value on success, NaN if indices out of bounds
 */
double
mat_get(const Matrix m, size_t row, size_t col)
{
        if (!m.data || row >= m.rows || col >= m.cols) {
                return NAN;
        }

        return m.data[row * m.cols + col];
}

/**
 * @brief Set element at specified row and column.
 * @param m Matrix to modify
 * @param row Row index (0-based)
 * @param col Column index (0-based)
 * @param value Value to set
 * @return 0 on success, -1 if indices out of bounds
 */
int
mat_set(Matrix *m, size_t row, size_t col, double value)
{
        if (!m || !m->data || row >= m->rows || col >= m->cols) {
                return -1;
        }

        m->data[row * m->cols + col] = value;
        return 0;
}

/**
 * @brief Print matrix contents to stdout with label header.
 * @param label Optional string identifier to print before matrix
 * @param m Matrix to display
 */
void
mat_print(const char *label, const Matrix m)
{
        if (label) {
                fprintf(stdout, "%s\n", label);
        }

        if (m.data == NULL || m.rows == 0 || m.cols == 0) {
                if (label) {
                        fprintf(stdout, "(empty matrix)\n");
                }
                return;
        }

        for (size_t i = 0; i < m.rows; i++) {
                for (size_t j = 0; j < m.cols; j++) {
                        printf("%12.6f", m.data[i * m.cols + j]);
                        if (j < m.cols - 1) {
                                printf(" ");
                        }
                }
                fprintf(stdout, "\n");
        }
}

Matrix
mat_identity(size_t n)
{
        Matrix I = mat_create(n, n);
        if (!I.data) {
                return I;
        }
        for (size_t i = 0; i < n; i++) {
                I.data[i * n + i] = 1.0;
        }
        return I;
}

double
mat_norm_l2(const Matrix *A)
{
        if (!A || !A->data) {
                return NAN;
        }
        double sum = 0.0;
        for (size_t i = 0; i < A->rows; i++) {
                for (size_t j = 0; j < A->cols; j++) {
                        double val = A->data[i * A->cols + j];
                        sum += val * val;
                }
        }
        return sqrt(sum);
}

double
mat_trace(const Matrix *A)
{
        if (!A || !A->data) {
                return NAN;
        }
        double trace = 0.0;
        size_t n = (A->rows < A->cols) ? A->rows : A->cols;
        for (size_t i = 0; i < n; i++) {
                trace += A->data[i * A->cols + i];
        }
        return trace;
}

double
mat_det(const Matrix *A)
{
        if (!A || !A->data || A->rows != A->cols) {
                return NAN;
        }

        size_t n = A->rows;
        if (n == 0) {
                return 0.0;
        }

        /* Create a copy to avoid modifying the original matrix */
        double *temp = (double *)malloc(n * n * sizeof(double));
        if (!temp) {
                return 0.0;
        }
        memcpy(temp, A->data, n * n * sizeof(double));

        double det = 1.0;
        for (size_t i = 0; i < n; i++) {
                /* Pivot selection */
                size_t pivot = i;
                for (size_t j = i + 1; j < n; j++) {
                        if (fabs(temp[j * n + i]) > fabs(temp[pivot * n + i])) {
                                pivot = j;
                        }
                }

                if (pivot != i) {
                        /* Swap rows */
                        for (size_t k = 0; k < n; k++) {
                                double t = temp[i * n + k];
                                temp[i * n + k] = temp[pivot * n + k];
                                temp[pivot * n + k] = t;
                        }
                        det *= -1.0;
                }

                if (fabs(temp[i * n + i]) < 1e-15) {
                        free(temp);
                        return 0.0;
                }

                det *= temp[i * n + i];

                for (size_t j = i + 1; j < n; j++) {
                        double factor = temp[j * n + i] / temp[i * n + i];
                        for (size_t k = i + 1; k < n; k++) {
                                temp[j * n + k] -= factor * temp[i * n + k];
                        }
                }
        }

        free(temp);
        return det;
}

/**
 * @brief Compute the Frobenius inner product (dot product) of two matrices.
 *
 * The dot product is defined as:
 *   A · B = Σᵢⱼ Aᵢⱼ × Bᵢⱼ
 *
 * Properties:
 * - Commutative: A · B = B · A
 * - Linear: (αA + βB) · C = α(A · C) + β(B · C)
 * - Positive definite: A · A ≥ 0, with equality iff A = 0
 * - Related to Frobenius norm: ||A||_F = sqrt(A · A)
 * - Cauchy-Schwarz: |A · B| ≤ ||A||_F × ||B||_F
 *
 * @param A First matrix (m × n)
 * @param B Second matrix (must also be m × n)
 * @return Dot product value on success, NaN if dimensions mismatch
 */
double
mat_dot(const Matrix A, const Matrix B)
{
        if (!A.data || !B.data || A.rows != B.rows || A.cols != B.cols) {
                return NAN;
        }

        double sum = 0.0;
        size_t count = A.rows * A.cols;

        for (size_t i = 0; i < count; i++) {
                sum += A.data[i] * B.data[i];
        }

        return sum;
}

/**
 * @brief Compute the inverse of a square matrix using Gauss-Jordan elimination.
 *
 * Algorithm:
 * 1. Create augmented matrix [A | I]
 * 2. For each column, normalize pivot row and eliminate other rows
 * 3. Extract inverse from right half of augmented matrix
 *
 * @param A The input square matrix (must be n × n and non-singular)
 * @param result Output matrix containing the inverse (must be pre-allocated
 * with same dimensions)
 * @return 0 on success, -1 if matrix is singular or not square
 */
int
mat_inv(const Matrix A, Matrix *result)
{
        if (!A.data || !result || !result->data) {
                fprintf(stderr, "mat_inv: Invalid matrix pointer\n");
                return -1;
        }

        if (A.rows != A.cols) {
                fprintf(stderr, "mat_inv: Matrix must be square\n");
                return -1;
        }

        if (A.rows != result->rows || A.cols != result->cols) {
                fprintf(stderr, "mat_inv: Result dimension mismatch\n");
                return -1;
        }

        size_t n = A.rows;

        // Create augmented matrix [A | I]
        double *aug = (double *)malloc(n * (2 * n) * sizeof(double));
        if (!aug) {
                fprintf(stderr, "mat_inv: Memory allocation failed\n");
                return -1;
        }

        // Initialize augmented matrix: left side = A, right side = I
        for (size_t i = 0; i < n; i++) {
                for (size_t j = 0; j < n; j++) {
                        aug[i * (2 * n) + j] = A.data[i * n + j]; // Left: A
                        aug[i * (2 * n) + n + j] =
                            (i == j) ? 1.0 : 0.0; // Right: I
                }
        }

        // Gauss-Jordan elimination
        for (size_t col = 0; col < n; col++) {
                // Find pivot (largest absolute value in column)
                size_t pivot_row = col;
                double max_val = fabs(aug[col * (2 * n) + col]);
                for (size_t i = col + 1; i < n; i++) {
                        double val = fabs(aug[i * (2 * n) + col]);
                        if (val > max_val) {
                                max_val = val;
                                pivot_row = i;
                        }
                }

                // Check for zero pivot (singular matrix)
                if (max_val < 1e-15) {
                        free(aug);
                        fprintf(stderr,
                                "mat_inv: Matrix is singular (zero pivot)\n");
                        return -1;
                }

                // Swap rows if needed
                if (pivot_row != col) {
                        for (size_t j = 0; j < 2 * n; j++) {
                                double temp = aug[col * (2 * n) + j];
                                aug[col * (2 * n) + j] =
                                    aug[pivot_row * (2 * n) + j];
                                aug[pivot_row * (2 * n) + j] = temp;
                        }
                }

                // Normalize pivot row
                double pivot = aug[col * (2 * n) + col];
                for (size_t j = 0; j < 2 * n; j++) {
                        aug[col * (2 * n) + j] /= pivot;
                }

                // Eliminate column in all other rows
                for (size_t i = 0; i < n; i++) {
                        if (i != col) {
                                double factor = aug[i * (2 * n) + col];
                                for (size_t j = 0; j < 2 * n; j++) {
                                        aug[i * (2 * n) + j] -=
                                            factor * aug[col * (2 * n) + j];
                                }
                        }
                }
        }

        // Extract inverse from right half of augmented matrix
        for (size_t i = 0; i < n; i++) {
                for (size_t j = 0; j < n; j++) {
                        result->data[i * n + j] = aug[i * (2 * n) + n + j];
                }
        }

        free(aug);
        return 0;
}
