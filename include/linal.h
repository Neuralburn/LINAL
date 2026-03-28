/**
 * @file linal.h
 * @brief  A lightweight linear algebra library for scientific computing
 */

#ifndef LINAL_H_
#define LINAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ================ INCLUDES ================================================ */

#include "linal_conf.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* ================ DEFINES ================================================= */

/* ================ STATIC ASSERTIONS ======================================= */
/* C11 compliance check */
_Static_assert(__STDC_VERSION__ >= 201112L, "LINAL requires C11 or later");

/* Type size checks */
_Static_assert(sizeof(double) == 8, "LINAL requires 64-bit double precision");
_Static_assert(sizeof(size_t) >= 4, "LINAL requires size_t >= 32-bit");

/* ================ STRUCTURES ============================================== */

/* ================ TYPEDEFS ================================================ */

/**
 * @struct Matrix
 * @brief Represents a 2D matrix stored in row-major order.
 *
 * The data buffer is allocated dynamically and must be freed manually
 * after the matrix is no longer needed. Use mat_create() to initialize
 * and mat_free() to release memory safely.
 */
typedef struct {
        double *data; /**< Pointer to contiguous array storing rows * cols
                         elements */
        size_t rows;  /**< Number of rows in the matrix */
        size_t cols;  /**< Number of columns in the matrix */
} Matrix;

/* ================ MACROS ================================================== */

/* ================ GLOBAL VARIABLES ======================================== */

/* ================ GLOBAL PROTOTYPES ======================================= */

/**
 * @brief Create a new matrix with specified dimensions, initializing data to
 * zero.
 * @param r Number of rows to create
 * @param c Number of columns to create
 * @return New Matrix struct initialized with zeros
 */
Matrix mat_create(size_t r, size_t c);

/**
 * @brief Free the memory associated with a matrix.
 * @param m Pointer to Matrix whose data field shall be freed
 */
void mat_free(Matrix *m);

/**
 * @brief Deep copy the source matrix into destination matrix.
 * @param src Source matrix to copy from
 * @param dest Pointer to destination matrix (must have matching dimensions)
 * @return 0 on success, -1 if dimensions mismatch or invalid pointers
 */
int mat_copy(const Matrix src, Matrix *dest);

/**
 * @brief Add two matrices element-wise.
 *
 * Computes the element-wise sum of two matrices:
 * @f$ C = A + B @f$
 * @f$ C_{ij} = A_{ij} + B_{ij} @f$
 *
 * @param a First operand
 * @param b Second operand
 * @param result Output matrix containing the sum (must not alias a or b)
 * @return 0 on success, -1 on dimension mismatch or invalid result
 */
int mat_add(const Matrix a, const Matrix b, Matrix *result);

/**
 * @brief Multiply two matrices together.
 *
 * Computes the matrix product using standard linear algebra multiplication:
 * @f$ C = A \times B @f$
 * @f$ C_{ij} = \sum_k(A_{ik} \times B_{kj}) @f$
 *
 * Requires A.cols == B.rows for valid multiplication.
 *
 * @param a Left operand (m × k matrix)
 * @param b Right operand (k × n matrix)
 * @param result Output matrix containing the product (m × n matrix)
 * @return 0 on success, -1 if inner dimensions do not match
 */
int mat_mul(const Matrix a, const Matrix b, Matrix *result);

/**
 * @brief Scale a matrix by a scalar factor.
 *
 * Multiplies each element of the matrix by a scalar value:
 * @f$ B = \alpha \times A @f$
 * @f$ B_{ij} = \alpha \times A_{ij} @f$
 *
 * @param m Input matrix to scale
 * @param scalar Scalar multiplier
 * @param result Output matrix containing the scaled values (must not alias m)
 * @return 0 on success, -1 if input is invalid
 */
int mat_scale(const Matrix m, double scalar, Matrix *result);

/**
 * @brief Transpose a matrix (swap rows and columns).
 *
 * Computes the matrix transpose by swapping rows and columns:
 * @f$ B = A^T @f$
 * @f$ B_{ij} = A_{ji} @f$
 *
 * @param m Input matrix to transpose (m × n matrix)
 * @param result Output matrix containing the transposed values (n × m matrix)
 * @return 0 on success, -1 if input is invalid or dimensions mismatch
 */
int mat_transpose(const Matrix m, Matrix *result);

/**
 * @brief Subtract two matrices element-wise.
 *
 * Computes the element-wise difference of two matrices:
 * @f$ C = A - B @f$
 * @f$ C_{ij} = A_{ij} - B_{ij} @f$
 *
 * @param a First operand (minuend)
 * @param b Second operand (subtrahend)
 * @param result Output matrix containing the difference (must not alias a or b)
 * @return 0 on success, -1 on dimension mismatch or invalid result
 */
int mat_sub(const Matrix a, const Matrix b, Matrix *result);

/**
 * @brief Get element at specified row and column.
 * @param m Matrix to access
 * @param row Row index (0-based)
 * @param col Column index (0-based)
 * @return Element value on success, NaN if indices out of bounds
 */
double mat_get(const Matrix m, size_t row, size_t col);

/**
 * @brief Set element at specified row and column.
 * @param m Matrix to modify
 * @param row Row index (0-based)
 * @param col Column index (0-based)
 * @param value Value to set
 * @return 0 on success, -1 if indices out of bounds
 */
int mat_set(Matrix *m, size_t row, size_t col, double value);

/**
 * @brief Print matrix contents to stdout with label header.
 * @param label Optional string identifier to print before matrix
 * @param m Matrix to display
 */
#if LINAL_ENABLE_DEBUG_PRINT
void mat_print(const char *label, const Matrix m);
#endif

/**
 * Create an identity matrix.
 *
 * An identity matrix I is a square matrix with ones on the main diagonal and
 * zeros elsewhere:
 * \f[I_{ij} = \delta_{ij} = \begin{cases} 1 & \text{if } i = j \\ 0 & \text{if
 * } i \neq j \end{cases}\f]
 *
 * The identity matrix acts as the multiplicative identity: \f[A \times I = I
 * \times A = A\f]
 *
 * @param n The dimension of the square identity matrix (n × n)
 * @return A Matrix struct containing the identity matrix
 */
Matrix mat_identity(size_t n);

/**
 * Compute the L2 (Frobenius) norm of a matrix.
 *
 * The Frobenius norm is defined as: \f[\|A\|_F =
 * \sqrt{\sum_{i=1}^{m}\sum_{j=1}^{n} |a_{ij}|^2}\f]
 *
 * @param A The input matrix
 * @return The L2 norm as a double value
 */
double mat_norm_l2(const Matrix *A);

/**
 * Compute the trace of a matrix (sum of diagonal elements).
 *
 * The trace is defined as: \f[\text{tr}(A) = \sum_{i=1}^{n} a_{ii}\f]
 *
 * For non-square matrices, only the diagonal elements up to min(rows, cols) are
 * summed.
 *
 * @param A The input matrix
 * @return The trace as a double value
 */
double mat_trace(const Matrix *A);

/**
 * Compute the determinant of a square matrix.
 *
 * The determinant is a scalar value computed from a square matrix using
 * cofactor expansion:
 * \f[\det(A) = \sum_{j=1}^{n} (-1)^{i+j} a_{ij} \det(M_{ij})\f]
 * where \f$M_{ij}\f$ is the minor matrix obtained by removing row i and column
 * j.
 *
 * Properties:
 * - \f$\det(A) = 0\f$ if and only if A is singular (non-invertible)
 * - \f$\det(A \times B) = \det(A) \times \det(B)\f$
 * - \f$\det(A^T) = \det(A)\f$
 * - For 2×2: \f$\det\begin{pmatrix}a & b \\ c & d\end{pmatrix} = ad - bc\f$
 *
 * @param A The input square matrix (must be n × n)
 * @return The determinant as a double value, or 0 if matrix is not square
 */
double mat_det(const Matrix *A);

/**
 * Compute the inverse of a square matrix using Gauss-Jordan elimination.
 *
 * The inverse of a matrix A is defined such that: \f[A \times A^{-1} = A^{-1}
 * \times A = I\f] where I is the identity matrix.
 *
 * The matrix must be non-singular (det(A) ≠ 0) for the inverse to exist.
 *
 * Algorithm: Gauss-Jordan elimination
 * 1. Augment A with identity matrix: [A | I]
 * 2. Apply row operations to transform A to I
 * 3. The right side becomes A⁻¹: [I | A⁻¹]
 *
 * @param A The input square matrix (must be n × n and non-singular)
 * @param result Output matrix containing the inverse (must be pre-allocated
 * with same dimensions)
 * @return 0 on success, -1 if matrix is singular or not square
 */
int mat_inv(const Matrix A, Matrix *result);

/**
 * Compute the dot product (Frobenius inner product) of two matrices.
 *
 * The dot product is defined as the sum of element-wise products:
 * \f[\text{dot}(A, B) = \langle A, B \rangle = \sum_{i=1}^{m} \sum_{j=1}^{n}
 * a_{ij} b_{ij}\f]
 *
 * This is equivalent to the Frobenius inner product and satisfies:
 * \f[\|A\|_F = \sqrt{\langle A, A \rangle}\f]
 *
 * For column vectors u and v (n × 1 matrices), this reduces to the standard
 * vector dot product:
 * \f[u \cdot v = \sum_{i=1}^{n} u_i v_i\f]
 *
 * Properties:
 * - Commutative: \f$\langle A, B \rangle = \langle B, A \rangle\f$
 * - Linear: \f$\langle \alpha A + \beta C, B \rangle = \alpha \langle A, B
 * \rangle + \beta \langle C, B \rangle\f$
 * - Positive definite: \f$\langle A, A \rangle \geq 0\f$ with equality iff A =
 * 0
 * - Cauchy-Schwarz: \f$|\langle A, B \rangle| \leq \|A\|_F \|B\|_F\f$
 *
 * Requires both matrices to have identical dimensions.
 *
 * @param A First matrix (m × n)
 * @param B Second matrix (m × n, must match dimensions of A)
 * @return The dot product as a double value, or NaN if dimensions mismatch
 */
double mat_dot(const Matrix A, const Matrix B);

#ifdef __cplusplus
}
#endif

#endif /* LINAL_H_ */
