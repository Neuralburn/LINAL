/**
 * @file linal.c
 * @brief Implementation of LINAL.
 */

/* ================ INCLUDES ================================================ */

#include "linal.h"

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
        for (size_t i = 0; i < count; i++) {
                result->data[i] = a.data[i] + b.data[i];
        }
        return 0;
}

/**
 * @brief Perform matrix multiplication: C = A * B
 * Optimizes loop ordering for cache locality during computation.
 */
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
        memset(result->data, 0.0, result->rows * result->cols * sizeof(double));

        /* Compute C[i][j] = sum_k(A[i][k] * B[k][j]) */
        for (size_t i = 0; i < a.rows; i++) {
                for (size_t j = 0; j < b.cols; j++) {
                        double sum = 0.0;
                        for (size_t k = 0; k < a.cols; k++) {
                                sum += a.data[i * a.cols + k]
                                       * b.data[k * b.cols + j];
                        }
                        result->data[i * result->cols + j] = sum;
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
        for (size_t i = 0; i < count; i++) {
                result->data[i] = m.data[i] * scalar;
        }

        return 0;
}

/**
 * @brief Transpose a matrix (swap rows and columns).
 * @param m Input matrix to transpose
 * @param result Output matrix containing the transposed values (must have dimensions m.cols x m.rows)
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
                fprintf(stderr, "Transpose error: Dimension mismatch (expected %zu x %zu, got %zu x %zu)\n",
                        m.cols, m.rows, result->rows, result->cols);
                return -1;
        }

        for (size_t i = 0; i < m.rows; i++) {
                for (size_t j = 0; j < m.cols; j++) {
                        result->data[j * result->cols + i] = m.data[i * m.cols + j];
                }
        }

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
