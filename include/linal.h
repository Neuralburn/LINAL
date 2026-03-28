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
 * @param a First operand
 * @param b Second operand
 * @param result Output matrix containing the sum (must not alias a or b)
 * @return 0 on success, -1 on dimension mismatch or invalid result
 */
int mat_add(const Matrix a, const Matrix b, Matrix *result);

/**
 * @brief Multiply two matrices together.
 * @param a Left operand
 * @param b Right operand
 * @param result Output matrix containing the product (must not alias a or b)
 * @return 0 on success, -1 if inner dimensions do not match
 */
int mat_mul(const Matrix a, const Matrix b, Matrix *result);

/**
 * @brief Scale a matrix by a scalar factor.
 * @param m Input matrix to scale
 * @param scalar Scalar multiplier
 * @param result Output matrix containing the scaled values (must not alias m)
 * @return 0 on success, -1 if input is invalid
 */
int mat_scale(const Matrix m, double scalar, Matrix *result);

/**
 * @brief Transpose a matrix (swap rows and columns).
 * @param m Input matrix to transpose
 * @param result Output matrix containing the transposed values (must have
 * swapped dimensions)
 * @return 0 on success, -1 if input is invalid or dimensions mismatch
 */
int mat_transpose(const Matrix m, Matrix *result);

/**
 * @brief Subtract two matrices element-wise (A - B).
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

#ifdef __cplusplus
}
#endif

#endif /* LINAL_H_ */
