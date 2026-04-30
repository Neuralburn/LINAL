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

        /* Reject aliasing: dest must not share storage with src. */
        if (dest->data == src.data) {
                return -1;
        }

        size_t num_elements = src.rows * src.cols;
        memcpy(dest->data, src.data, num_elements * sizeof(double));
        return 0;
}

/**
 * @brief Perform element-wise addition of two matrices.
 * Both matrices must have identical dimensions for a successful result.
 * Uses auto-vectorization with ivdep hint for serial path and OpenMP simd
 * parallelism for large matrices (≥4096 elements).
 */
__attribute__((optimize("O3")))
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

        /* Reject aliasing: result must not share storage with a or b. */
        if (result->data == a.data || result->data == b.data) {
                fprintf(stderr,
                        "Addition error: result aliases input operand\n");
                return -1;
        }

        size_t count = a.rows * a.cols;

#if defined(_OPENMP)
        /* Parallelize only for large matrices to avoid thread overhead.
         * Threshold of 16384 elements (~128×128) avoids boundary spikes. */
        if (count >= 16384) {
#pragma omp parallel for simd
                for (size_t i = 0; i < count; i++) {
                        result->data[i] = a.data[i] + b.data[i];
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
                        R[i] = A[i] + B[i];
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

        /* Reject aliasing: result must not share storage with a or b. */
        if (result->data == a.data || result->data == b.data) {
                fprintf(stderr, "Multiplication error: result aliases input "
                                "operand\n");
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
 * Uses auto-vectorization with ivdep hint for serial path and OpenMP simd
 * parallelism for large matrices (≥16384 elements).
 * @param m Input matrix to scale
 * @param scalar Scalar multiplier
 * @param result Output matrix containing the scaled values (must not alias m)
 * @return 0 on success, -1 if input is invalid
 */
__attribute__((optimize("O3")))
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

        /* Reject aliasing: result must not share storage with input. */
        if (result->data == m.data) {
                fprintf(stderr, "Scale error: result aliases input\n");
                return -1;
        }

        size_t count = m.rows * m.cols;
#if defined(_OPENMP)
        if (count >= 16384) {
#pragma omp parallel for simd
                for (size_t i = 0; i < count; i++) {
                        result->data[i] = m.data[i] * scalar;
                }
        } else
#endif
        {
                const double *M = m.data;
                double *R       = result->data;
                #pragma GCC ivdep
                for (size_t i = 0; i < count; i++) {
                        R[i] = M[i] * scalar;
                }
        }

        return 0;
}

/**
 * @brief Transpose a matrix (swap rows and columns).
 * Uses cache blocking/tiling for large matrices to ensure both reads
 * and writes are sequential within each block, fitting in L1 cache.
 * @param m Input matrix to transpose
 * @param result Output matrix containing the transposed values (must have
 * dimensions m.cols x m.rows)
 * @return 0 on success, -1 if input is invalid or dimensions mismatch
 */
__attribute__((optimize("O3")))
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

        /* Reject aliasing: result must not share storage with input. */
        if (result->data == m.data) {
                fprintf(stderr, "Transpose error: result aliases input\n");
                return -1;
        }

        size_t rows = m.rows;
        size_t cols = m.cols;

        /* Block size tuned for L1 cache (~32KB). 64×64 doubles ≈ 32KB. */
#define TRANSPOSE_BLOCK 64

        if (rows > TRANSPOSE_BLOCK && cols > TRANSPOSE_BLOCK) {
                const double *M  = m.data;
                double *R        = result->data;
                /* Buffered block transpose: load block row-by-row (sequential reads),
                 * then write transposed with sequential dest writes.
                 * Block size chosen so buffer fits in L1 cache (~32KB for 64x64). */
                double buf[TRANSPOSE_BLOCK * TRANSPOSE_BLOCK];
                for (size_t ii = 0; ii < rows; ii += TRANSPOSE_BLOCK) {
                        size_t i_end =
                            (ii + TRANSPOSE_BLOCK < rows)
                                ? ii + TRANSPOSE_BLOCK : rows;
                        size_t bh = i_end - ii;
                        for (size_t jj = 0; jj < cols; jj += TRANSPOSE_BLOCK) {
                                size_t j_end = (jj + TRANSPOSE_BLOCK < cols)
                                                   ? jj + TRANSPOSE_BLOCK
                                                   : cols;
                                size_t bw = j_end - jj;

                                /* Load source block into buffer row-by-row */
                                for (size_t bi = 0; bi < bh; bi++) {
                                        const double *src = M + (ii + bi) * cols + jj;
                                        double *dst_buf   = buf + bi * bw;
                                        memcpy(dst_buf, src, bw * sizeof(double));
                                }

                                /* Write transposed: outer=bj(dest row), inner=bi(dest col)
                                 * This gives sequential writes along each dest row. */
                                for (size_t bj = 0; bj < bw; bj++) {
                                        double *dst_row = R + (jj + bj) * rows + ii;
                                        for (size_t bi = 0; bi < bh; bi++) {
                                                dst_row[bi] = buf[bi * bw + bj];
                                        }
                                }
                        }
                }
        } else {
                /* Small matrix — naive loop is fine */
                for (size_t i = 0; i < rows; i++) {
                        for (size_t j = 0; j < cols; j++) {
                                result->data[j * rows + i] =
                                    m.data[i * cols + j];
                        }
                }
        }
#undef TRANSPOSE_BLOCK

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

        /* Reject aliasing: result must not share storage with a or b. */
        if (result->data == a.data || result->data == b.data) {
                fprintf(stderr,
                        "Subtraction error: result aliases input operand\n");
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

__attribute__((optimize("O3,unroll-loops")))
double
mat_norm_l2(const Matrix *A)
{
        if (!A || !A->data) {
                return NAN;
        }

        const double *__restrict__ data = A->data;
        size_t count = A->rows * A->cols;
        double sum = 0.0;

#if defined(_OPENMP)
        /* Thread-level parallelism with SIMD for large matrices.
         * Using all available threads (no num_threads limit). */
        if (count >= 16384) { /* 128*128 = 16K elements */
#pragma omp parallel for simd reduction(+:sum)
                for (size_t i = 0; i < count; i++) {
                        double val = data[i];
                        sum += val * val;
                }
                return sqrt(sum);
        } else
#endif
        {
                /* Small-medium matrices: simple loop + ivdep. Same perf as omp simd.
                 * Simpler code, easier to maintain. */
                #pragma GCC ivdep
                for (size_t i = 0; i < count; i++) {
                        double val = data[i];
                        sum += val * val;
                }
        }

        return sqrt(sum);
}

__attribute__((optimize("O3")))
double
mat_trace(const Matrix *A)
{
        if (!A || !A->data) {
                return NAN;
        }
        size_t n = (A->rows < A->cols) ? A->rows : A->cols;
#if defined(_OPENMP)
        if (n >= 65536) {
                /* Parallel reduction for very large matrices */
                double sum = 0.0;
#pragma omp parallel for simd reduction(+:sum)
                for (size_t i = 0; i < n; i++) {
                        sum += A->data[i * A->cols + i];
                }
                return sum;
        }
#endif
        /* Use 8 accumulators with pairwise summation tree for ILP + precision */
        double t[8];
        const double *__restrict__ diag = A->data;
        size_t stride = A->cols;
        size_t i;
        for (int k = 0; k < 8; k++) t[k] = 0.0;
        #pragma GCC ivdep
        for (i = 0; i + 7 < n; i += 8) {
                t[0] += diag[i * stride + i];
                t[1] += diag[(i + 1) * stride + (i + 1)];
                t[2] += diag[(i + 2) * stride + (i + 2)];
                t[3] += diag[(i + 3) * stride + (i + 3)];
                t[4] += diag[(i + 4) * stride + (i + 4)];
                t[5] += diag[(i + 5) * stride + (i + 5)];
                t[6] += diag[(i + 6) * stride + (i + 6)];
                t[7] += diag[(i + 7) * stride + (i + 7)];
                /* Prefetch every 4 iterations to overlap latency */
                if ((i >> 3) & 1)
                        __builtin_prefetch(&diag[(i + 8) * stride + (i + 8)], 0, 1);
        }
        for (; i < n; i++) {
                t[0] += diag[i * stride + i];
        }
        /* Pairwise summation tree */
        return ((t[0]+t[1])+(t[2]+t[3]))+((t[4]+t[5])+(t[6]+t[7]));
}

__attribute__((optimize("O3")))
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

        double *__restrict__ r = temp;
        double det = 1.0;
#if defined(_OPENMP) && n > 128
        /* Task-based elimination: single parallel region, tasks per row.
         * Reduces fork/join from ~250 spawns to 1 thread pool creation. */
#pragma omp parallel num_threads(4)
        {
#pragma omp single
                {
                        for (size_t i = 0; i < n; i++) {
                                /* Pivot selection */
                                size_t pivot = i;
                                for (size_t j = i + 1; j < n; j++) {
                                        if (fabs(r[j * n + i]) > fabs(r[pivot * n + i])) {
                                                pivot = j;
                                        }
                                }

                                if (pivot != i) {
                                        if (n <= 64) {
                                                for (size_t k = 0; k < n; k++) {
                                                        double t = r[i * n + k];
                                                        r[i * n + k] = r[pivot * n + k];
                                                        r[pivot * n + k] = t;
                                                }
                                        } else {
                                                size_t bytes = n * sizeof(double);
                                                double *__restrict__ tmp =
                                                    (double *)__builtin_alloca(bytes);
                                                memcpy(tmp, r + i * n, bytes);
                                                memcpy(r + i * n, r + pivot * n, bytes);
                                                memcpy(r + pivot * n, tmp, bytes);
                                        }
                                        det *= -1.0;
                                }

                                if (fabs(r[i * n + i]) < 1e-15) {
                                        free(temp);
                                        return det;
                                }

                                det *= r[i * n + i];

                                double inv_pivot = 1.0 / r[i * n + i];
                                const double *__restrict__ src = r + i * n;
                                __builtin_prefetch(src, 0, 3);

                                if (n - i >= 8) {
#pragma omp taskloop grainsize(4)
                                        for (size_t j = i + 1; j < n; j++) {
                                                double factor = r[j * n + i] * inv_pivot;
                                                size_t k = i + 1;
                                                #pragma GCC ivdep
                                                for (; k + 7 < n; k += 8) {
                                                        r[j * n + k] -= factor * src[k];
                                                }
                                                for (; k < n; k++) {
                                                        r[j * n + k] -= factor * src[k];
                                                }
                                        }
#pragma omp taskwait
                                } else {
                                        for (size_t j = i + 1; j < n; j++) {
                                                double factor = r[j * n + i] * inv_pivot;
                                                size_t k = i + 1;
                                                #pragma GCC ivdep
                                                for (; k + 7 < n; k += 8) {
                                                        r[j * n + k] -= factor * src[k];
                                                }
                                                for (; k < n; k++) {
                                                        r[j * n + k] -= factor * src[k];
                                                }
                                        }
                                }
#pragma omp taskwait  // ensure all tasks complete before next iteration
                        } /* end single loop */
                } /* end single */
        } /* end parallel */
#else
        for (size_t i = 0; i < n; i++) {
                /* Pivot selection */
                size_t pivot = i;
                for (size_t j = i + 1; j < n; j++) {
                        if (fabs(r[j * n + i]) > fabs(r[pivot * n + i])) {
                                pivot = j;
                        }
                }

                if (pivot != i) {
                        /* Swap rows */
                        for (size_t k = 0; k < n; k++) {
                                double t = r[i * n + k];
                                r[i * n + k] = r[pivot * n + k];
                                r[pivot * n + k] = t;
                        }
                        det *= -1.0;
                }

                if (fabs(r[i * n + i]) < 1e-15) {
                        free(temp);
                        return 0.0;
                }

                det *= r[i * n + i];

                /* Elimination: compute factor once per row, then update */
                double inv_pivot = 1.0 / r[i * n + i];
                const double *__restrict__ src = r + i * n;
                /* Prefetch source row once — stays in cache across all j iterations */
                __builtin_prefetch(src, 0, 3);
#if defined(_OPENMP)
                if (n - i >= 16) {
#pragma omp parallel for schedule(static) num_threads(4)
                        for (size_t j = i + 1; j < n; j++) {
                                double factor = r[j * n + i] * inv_pivot;
                                double *__restrict__ dest = r + j * n;
                                /* Prefetch dest row — overlaps memory read with compute */
                                __builtin_prefetch(dest, 1, 3);
                                size_t k = i + 1;
                                #pragma GCC ivdep
                                for (; k + 7 < n; k += 8) {
                                        dest[k]     -= factor * src[k];
                                        dest[k + 1] -= factor * src[k + 1];
                                        dest[k + 2] -= factor * src[k + 2];
                                        dest[k + 3] -= factor * src[k + 3];
                                        dest[k + 4] -= factor * src[k + 4];
                                        dest[k + 5] -= factor * src[k + 5];
                                        dest[k + 6] -= factor * src[k + 6];
                                        dest[k + 7] -= factor * src[k + 7];
                                }
                                for (; k < n; k++) {
                                        dest[k] -= factor * src[k];
                                }
                        }
                } else
#endif
                {
                        for (size_t j = i + 1; j < n; j++) {
                                double factor = r[j * n + i] * inv_pivot;
                                double *__restrict__ dest = r + j * n;
                                /* Prefetch dest row — overlaps memory read with compute */
                                __builtin_prefetch(dest, 1, 3);
                                size_t k = i + 1;
                                #pragma GCC ivdep
                                for (; k + 7 < n; k += 8) {
                                        dest[k]     -= factor * src[k];
                                        dest[k + 1] -= factor * src[k + 1];
                                        dest[k + 2] -= factor * src[k + 2];
                                        dest[k + 3] -= factor * src[k + 3];
                                        dest[k + 4] -= factor * src[k + 4];
                                        dest[k + 5] -= factor * src[k + 5];
                                        dest[k + 6] -= factor * src[k + 6];
                                        dest[k + 7] -= factor * src[k + 7];
                                }
                                for (; k < n; k++) {
                                        dest[k] -= factor * src[k];
                                }
                        }
                }
        }
#endif

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

        const double *__restrict__ a = A.data;
        const double *__restrict__ b = B.data;
        double sum = 0.0;
        size_t count = A.rows * A.cols;

#if defined(_OPENMP)
        /* Parallel with controlled thread count — fewer threads reduce variance */
        if (count >= 65536) {
#pragma omp parallel for num_threads(24) schedule(static) reduction(+:sum)
                for (size_t i = 0; i < count; i++) {
                        sum += a[i] * b[i];
                }
        } else
#endif
        {
                #pragma GCC ivdep
                for (size_t i = 0; i < count; i++) {
                        sum += a[i] * b[i];
                }
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
 * Optimizations:
 * - memcpy for row initialization, swap, and extraction
 * - #pragma GCC ivdep on inner loops for auto-vectorization (AVX2)
 * - Split elimination loop to avoid branch per iteration
 *
 * @param A The input square matrix (must be n × n and non-singular)
 * @param result Output matrix containing the inverse (must be pre-allocated
 * with same dimensions)
 * @return 0 on success, -1 if matrix is singular or not square
 */
__attribute__((optimize("O3")))
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

        /* Reject aliasing: result must not share storage with input. */
        if (result->data == A.data) {
                fprintf(stderr, "mat_inv: result aliases input\n");
                return -1;
        }

        size_t n = A.rows;
        size_t stride = 2 * n;

        // Create augmented matrix [A | I]
        double *aug = (double *)malloc(n * stride * sizeof(double));
        if (!aug) {
                fprintf(stderr, "mat_inv: Memory allocation failed\n");
                return -1;
        }

        // Initialize: copy A into left half, set identity in right half
        for (size_t i = 0; i < n; i++) {
                memcpy(aug + i * stride, A.data + i * n, n * sizeof(double));
                memset(aug + i * stride + n, 0, n * sizeof(double));
                aug[i * stride + n + i] = 1.0;
        }

        // Gauss-Jordan elimination
        for (size_t col = 0; col < n; col++) {
                // Find pivot (largest absolute value in column)
                size_t pivot_row = col;
                double max_val = fabs(aug[col * stride + col]);
                for (size_t i = col + 1; i < n; i++) {
                        double val = fabs(aug[i * stride + col]);
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

                // Swap rows if needed — use memcpy-based 3-way swap
                if (pivot_row != col) {
                        double *row_a = aug + col * stride;
                        double *row_b = aug + pivot_row * stride;
                        double tmp_buf[64];
                        size_t swap_bytes = stride * sizeof(double);
                        if (swap_bytes <= sizeof(tmp_buf)) {
                                memcpy(tmp_buf, row_a, swap_bytes);
                                memcpy(row_a, row_b, swap_bytes);
                                memcpy(row_b, tmp_buf, swap_bytes);
                        } else {
                                double *tmp = (double *)malloc(swap_bytes);
                                memcpy(tmp, row_a, swap_bytes);
                                memcpy(row_a, row_b, swap_bytes);
                                memcpy(row_b, tmp, swap_bytes);
                                free(tmp);
                        }
                }

                // Normalize pivot row — auto-vectorizable with ivdep
                double *pivot_row_ptr = aug + col * stride;
                double inv_pivot = 1.0 / pivot_row_ptr[col];
                #pragma GCC ivdep
                for (size_t j = 0; j < stride; j++) {
                        pivot_row_ptr[j] *= inv_pivot;
                }

                // Eliminate column in all other rows
#if defined(_OPENMP)
                if (n >= 8) {
#pragma omp parallel for schedule(static)
                        for (long idx = 0; idx < (long)n; idx++) {
                                size_t i = (size_t)idx;
                                if (i == col) continue;
                                double factor = aug[i * stride + col];
                                double *target_row = aug + i * stride;
                                #pragma omp simd
                                for (size_t j = 0; j < stride; j++) {
                                        target_row[j] -=
                                            factor * pivot_row_ptr[j];
                                }
                        }
                } else
#endif
                {
                        // Serial: split to avoid branch per iteration
                        for (size_t i = 0; i < col; i++) {
                                double factor = aug[i * stride + col];
                                double *target_row = aug + i * stride;
                                #pragma GCC ivdep
                                for (size_t j = 0; j < stride; j++) {
                                        target_row[j] -=
                                            factor * pivot_row_ptr[j];
                                }
                        }
                        for (size_t i = col + 1; i < n; i++) {
                                double factor = aug[i * stride + col];
                                double *target_row = aug + i * stride;
                                #pragma GCC ivdep
                                for (size_t j = 0; j < stride; j++) {
                                        target_row[j] -=
                                            factor * pivot_row_ptr[j];
                                }
                        }
                }
        }

        // Extract inverse from right half — use memcpy per row
        for (size_t i = 0; i < n; i++) {
                memcpy(result->data + i * n, aug + i * stride + n,
                       n * sizeof(double));
        }

        free(aug);
        return 0;
}
