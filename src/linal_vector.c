/**
 * @file linal_vector.c
 * @brief Implementation of Vector operations for LINAL.
 */

#define _GNU_SOURCE

#include "linal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <sys/mman.h>
#endif

/* ============ Internal helpers ========================================== */

/**
 * @brief Check if a Vector pointer and its data are valid.
 * @param v Pointer to the vector to validate.
 * @return 1 if valid, 0 otherwise.
 */
static int
vec_is_valid(const Vector *v)
{
        return v != NULL && v->data != NULL;
}

/**
 * @brief Safely allocate memory for a vector of given size.
 * @param size Number of double elements to allocate.
 * @return Pointer to allocated memory, or NULL on failure.
 */
static double *
vec_alloc_safe(size_t size)
{
        if (size == 0) {
                return NULL;
        }
        double *data = (double *)malloc(size * sizeof(double));
        if (!data) {
                fprintf(stderr,
                        "Allocation failed creating vector of size %zu\n",
                        size);
        }
        return data;
}

/**
 * @brief Print a standardized error message to stderr.
 * @param op Name of the operation that failed.
 * @param reason Human-readable reason for the failure.
 */
static void
vec_error(const char *op, const char *reason)
{
        fprintf(stderr, "Vector %s error: %s\n", op, reason);
}

/* ============ Public API ================================================= */

/**
 * @brief Create a new vector with specified size, initializing data to zero.
 */
Vector
vec_create(size_t size)
{
        Vector v = {0};
        v.data = vec_alloc_safe(size);
        if (v.data && size > 0) {
                memset(v.data, 0, size * sizeof(double));
                v.size = size;
        }
        return v;
}

/**
 * @brief Free the memory associated with a vector.
 */
void
vec_free(Vector *v)
{
        if (!v || !v->data) {
                return;
        }
        free(v->data);
        v->data = NULL;
        v->size = 0;
}

/**
 * @brief Add two vectors element-wise.
 *
 * Uses OpenMP parallel + SIMD for large vectors (2 threads to limit
 * memory bandwidth pressure) and omp simd for the serial path.
 */
__attribute__((optimize("O3")))
int
vec_add(const Vector a, const Vector b, Vector *result)
{
        if (!vec_is_valid(&a) || !vec_is_valid(&b) || !result || !result->data
            || a.size != b.size || a.size != result->size) {
                vec_error("addition", "Invalid vector or dimension mismatch");
                return -1;
        }

        if (result->data == a.data || result->data == b.data) {
                vec_error("addition", "result aliases input operand");
                return -1;
        }

        size_t count = a.size;

#if defined(_OPENMP)
        /* 2 threads — balances parallelism with memory bandwidth for 3-array access */
        if (count >= 262144) {
                const double *A = a.data;
                const double *B = b.data;
                double *R       = result->data;
                /* Process in 64K blocks for cache-friendly static scheduling */
                size_t block_size = 131072;
                size_t full_blocks = count / block_size;
                size_t tail_start  = full_blocks * block_size;
#pragma omp parallel for num_threads(2) schedule(static, 2)
                for (size_t block = 0; block < full_blocks; block++) {
                        size_t start = block * block_size;
                        /* Prefetch next 2 blocks for deeper lookahead */
                        size_t pf1 = start + block_size;
                        size_t pf2 = start + 2 * block_size;
                        __builtin_prefetch(&A[pf1], 0, 3);
                        __builtin_prefetch(&B[pf1], 0, 3);
                        __builtin_prefetch(&R[pf1], 1, 3);
                        if (pf2 < count) {
                                __builtin_prefetch(&A[pf2], 0, 1);
                                __builtin_prefetch(&B[pf2], 0, 1);
                                __builtin_prefetch(&R[pf2], 1, 1);
                        }
                        #pragma omp simd safelen(2) aligned(A:8) aligned(B:8) aligned(R:8)
                        for (size_t i = start; i < start + block_size; i++) {
                                R[i] = A[i] + B[i];
                        }
                }
                /* Tail without prefetch */
                #pragma omp simd safelen(2) aligned(A:16) aligned(B:16) aligned(R:16)
                for (size_t i = tail_start; i < count; i++) {
                        R[i] = A[i] + B[i];
                }
        } else
#endif
        {
                /* Serial SIMD path */
                #pragma omp simd safelen(2)
                for (size_t i = 0; i < count; i++) {
                        result->data[i] = a.data[i] + b.data[i];
                }
        }

        return 0;
}

/**
 * @brief Subtract two vectors element-wise.
 */
int
vec_sub(const Vector a, const Vector b, Vector *result)
{
        if (!vec_is_valid(&a) || !vec_is_valid(&b) || !result || !result->data
            || a.size != b.size || a.size != result->size) {
                vec_error("subtraction",
                          "Invalid vector or dimension mismatch");
                return -1;
        }

        if (result->data == a.data || result->data == b.data) {
                vec_error("subtraction", "result aliases input operand");
                return -1;
        }

        for (size_t i = 0; i < a.size; i++) {
                result->data[i] = a.data[i] - b.data[i];
        }

        return 0;
}

/**
 * @brief Compute the dot product of two vectors.
 *
 * Uses AVX2+FMA target for SIMD vectorization, multiple accumulators with
 * pairwise summation for numerical precision, and OpenMP parallel reduction
 * with explicit chunking for large vectors.
 */
#pragma GCC push_options
#pragma GCC target("avx2,fma")
__attribute__((optimize("O3")))
double
vec_dot(const Vector a, const Vector b)
{
        if (!vec_is_valid(&a) || !vec_is_valid(&b) || a.size != b.size) {
                return NAN;
        }

        const double *__restrict__ A = a.data;
        const double *__restrict__ B = b.data;
        size_t count = a.size;
        double sum = 0.0;

#if defined(_OPENMP)
        /* Parallel reduction for large vectors.
         * Explicit data sharing clauses help compiler optimize access patterns.
         * 8 threads = 8 physical cores on target hardware (optimal for MLB). */
        if (count >= 262144) {
#ifdef __linux__
                madvise((void *)A, count * sizeof(double), POSIX_MADV_SEQUENTIAL);
                madvise((void *)B, count * sizeof(double), POSIX_MADV_SEQUENTIAL);
#endif
#pragma omp parallel for num_threads(8) schedule(static, 65536) \
                default(none) firstprivate(A, B, count) reduction(+:sum)
                for (size_t i = 0; i < count; i++) {
                        sum += A[i] * B[i];
                }
                return sum;
        }
#endif

        /* 8 accumulators with pairwise summation tree for ILP + precision */
        double s[8];
        for (int k = 0; k < 8; k++)
                s[k] = 0.0;

        #pragma GCC ivdep
        for (size_t i = 0; i + 7 < count; i += 8) {
                s[0] += A[i]     * B[i];
                s[1] += A[i + 1] * B[i + 1];
                s[2] += A[i + 2] * B[i + 2];
                s[3] += A[i + 3] * B[i + 3];
                s[4] += A[i + 4] * B[i + 4];
                s[5] += A[i + 5] * B[i + 5];
                s[6] += A[i + 6] * B[i + 6];
                s[7] += A[i + 7] * B[i + 7];
                /* Prefetch next cache line every 4 iterations */
                if ((i >> 5) & 1)
                        __builtin_prefetch(&A[i + 256], 0, 1);
        }
        for (size_t i = count & ~7; i < count; i++) {
                s[0] += A[i] * B[i];
        }

        /* Pairwise summation tree */
        return ((s[0]+s[1])+(s[2]+s[3]))+((s[4]+s[5])+(s[6]+s[7]));
}
#pragma GCC pop_options

/**
 * @brief Scale a vector by a scalar factor.
 */
int
vec_scale(const Vector v, double scalar, Vector *result)
{
        if (!vec_is_valid(&v) || !result || !result->data
            || v.size != result->size) {
                vec_error("scaling", "Invalid vector or dimension mismatch");
                return -1;
        }

        if (result->data == v.data) {
                vec_error("scaling", "result aliases input");
                return -1;
        }

        for (size_t i = 0; i < v.size; i++) {
                result->data[i] = v.data[i] * scalar;
        }

        return 0;
}

/**
 * @brief Compute the L2 norm of a vector.
 *
 * Uses AVX2+FMA target for SIMD vectorization, multiple accumulators with
 * pairwise summation for numerical precision, and OpenMP parallel reduction
 * with explicit chunking for large vectors.
 */
#pragma GCC push_options
#pragma GCC target("avx2,fma")
__attribute__((optimize("O3")))
double
vec_norm_l2(const Vector v)
{
        if (!vec_is_valid(&v)) {
                return NAN;
        }

        const double *__restrict__ A = v.data;
        size_t count = v.size;
        double sum = 0.0;

#if defined(_OPENMP)
        /* Parallel reduction for large vectors.
         * Explicit data sharing clauses help compiler optimize access patterns.
         * 4 threads — fewer threads, less cache contention for single array. */
        if (count >= 16384) {
#ifdef __linux__
                madvise((void *)A, count * sizeof(double), POSIX_MADV_SEQUENTIAL);
#endif
#pragma omp parallel for num_threads(4) schedule(static, 131072) \
                default(none) firstprivate(A, count) reduction(+:sum)
                for (size_t i = 0; i < count; i++) {
                        double val = A[i];
                        sum += val * val;
                }
                return sqrt(sum);
        }
#endif

        /* 8 accumulators with pairwise summation tree for ILP + precision */
        double s[8];
        for (int k = 0; k < 8; k++)
                s[k] = 0.0;

        #pragma GCC ivdep
        for (size_t i = 0; i + 7 < count; i += 8) {
                s[0] += A[i]     * A[i];
                s[1] += A[i + 1] * A[i + 1];
                s[2] += A[i + 2] * A[i + 2];
                s[3] += A[i + 3] * A[i + 3];
                s[4] += A[i + 4] * A[i + 4];
                s[5] += A[i + 5] * A[i + 5];
                s[6] += A[i + 6] * A[i + 6];
                s[7] += A[i + 7] * A[i + 7];
                /* Prefetch next cache line every 4 iterations */
                if ((i >> 5) & 1)
                        __builtin_prefetch(&A[i + 256], 0, 1);
        }
        for (size_t i = count & ~7; i < count; i++) {
                s[0] += A[i] * A[i];
        }

        /* Pairwise summation tree */
        return sqrt(((s[0]+s[1])+(s[2]+s[3]))+((s[4]+s[5])+(s[6]+s[7])));
}
#pragma GCC pop_options

/**
 * @brief Get element at specified index.
 */
double
vec_get(const Vector v, size_t index)
{
        if (!vec_is_valid(&v) || index >= v.size) {
                return NAN;
        }

        return v.data[index];
}

/**
 * @brief Set element at specified index.
 */
int
vec_set(Vector *v, size_t index, double value)
{
        if (!v || !vec_is_valid(v) || index >= v->size) {
                vec_error("set", "invalid vector or index out of bounds");
                return -1;
        }

        v->data[index] = value;
        return 0;
}

/**
 * @brief Deep copy the source vector into destination vector.
 */
int
vec_copy(const Vector src, Vector *dest)
{
        if (!dest || src.size != dest->size) {
                vec_error("copy", "null dest or dimension mismatch");
                return -1;
        }

        /* For non-zero vectors, both data pointers must be valid. */
        if (src.size > 0 && (!src.data || !dest->data)) {
                vec_error("copy", "null data in non-zero vector");
                return -1;
        }

        /* Reject self-copy (aliasing). */
        if (src.data != NULL && src.data == dest->data) {
                vec_error("copy", "self-copy (aliasing)");
                return -1;
        }

        /* Zero-size: src.data and dest->data may both be NULL.
         * memcpy(NULL, NULL, 0) is UB per the standard, so short-circuit. */
        if (src.size == 0) {
                return 0;
        }

        memcpy(dest->data, src.data, src.size * sizeof(double));
        return 0;
}

#if LINAL_ENABLE_DEBUG_PRINT
/**
 * @brief Print vector contents to stdout with label header.
 */
void
vec_print(const char *label, const Vector v)
{
        if (label) {
                fprintf(stdout, "%s\n", label);
        }

        if (v.data == NULL || v.size == 0) {
                if (label) {
                        fprintf(stdout, "(empty vector)\n");
                }
                return;
        }

        fprintf(stdout, "[");
        for (size_t i = 0; i < v.size; i++) {
                if (i > 0) {
                        fprintf(stdout, " ");
                }
                printf("%12.6f", v.data[i]);
        }
        fprintf(stdout, "]\n");
}
#endif

/**
 * @brief Normalize a vector to unit length.
 */
int
vec_normalize(const Vector v, Vector *result)
{
        if (!result || v.size != result->size) {
                vec_error("normalize", "null result or dimension mismatch");
                return -1;
        }

        if (result->data == v.data) {
                vec_error("normalize", "result aliases input");
                return -1;
        }

        double norm = vec_norm_l2(v);
        if (norm <= 0.0) {
                vec_error("normalize", "zero-length vector");
                return -1;
        }

        double inv_norm = 1.0 / norm;
        for (size_t i = 0; i < v.size; i++) {
                result->data[i] = v.data[i] * inv_norm;
        }

        return 0;
}

/**
 * @brief Compute the Euclidean distance between two vectors.
 */
double
vec_distance(const Vector a, const Vector b)
{
        if (a.size != b.size) {
                return NAN;
        }

        if (a.size == 0) {
                return 0.0;
        }

        if (!vec_is_valid(&a) || !vec_is_valid(&b)) {
                return NAN;
        }

        double sum = 0.0;
        for (size_t i = 0; i < a.size; i++) {
                double diff = a.data[i] - b.data[i];
                sum += diff * diff;
        }

        return sqrt(sum);
}

/**
 * @brief Compute the angle between two vectors in radians.
 */
double
vec_angle(const Vector a, const Vector b)
{
        if (!vec_is_valid(&a) || !vec_is_valid(&b) || a.size != b.size) {
                return NAN;
        }

        double dot = vec_dot(a, b);
        double norm_a = vec_norm_l2(a);
        double norm_b = vec_norm_l2(b);

        if (norm_a <= 0.0 || norm_b <= 0.0) {
                return NAN;
        }

        double cos_theta = dot / (norm_a * norm_b);

        /* Clamp to [-1, 1] to handle floating-point domain errors. */
        if (cos_theta > 1.0) {
                cos_theta = 1.0;
        } else if (cos_theta < -1.0) {
                cos_theta = -1.0;
        }

        return acos(cos_theta);
}

/**
 * @brief Compute the element-wise (Hadamard) product of two vectors.
 */
int
vec_hadamard(const Vector a, const Vector b, Vector *result)
{
        if (!result || a.size != b.size || a.size != result->size) {
                vec_error("hadamard", "null result or dimension mismatch");
                return -1;
        }

        if (!vec_is_valid(&a) || !vec_is_valid(&b)) {
                vec_error("hadamard", "null data");
                return -1;
        }

        /* Reject aliasing for consistency with other binary ops,
         * even though element-wise multiply is safe for in-place use. */
        if (result->data == a.data || result->data == b.data) {
                vec_error("hadamard", "result aliases input operand");
                return -1;
        }

        for (size_t i = 0; i < a.size; i++) {
                result->data[i] = a.data[i] * b.data[i];
        }

        return 0;
}

/**
 * @brief Compute the element-wise absolute value of a vector.
 */
int
vec_abs(const Vector v, Vector *result)
{
        if (!result || !vec_is_valid(&v) || v.size != result->size) {
                vec_error("abs", "null data or dimension mismatch");
                return -1;
        }

        if (result->data == v.data) {
                vec_error("abs", "result aliases input");
                return -1;
        }

        for (size_t i = 0; i < v.size; i++) {
                result->data[i] = fabs(v.data[i]);
        }

        return 0;
}
