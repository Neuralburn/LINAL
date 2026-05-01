# Autoresearch Ideas for mat_inv

## Unexplored Optimizations

### Algorithmic
- **LU decomposition (correct implementation)**: PA=LU, invert L (forward sub), solve U×X=L⁻¹ (back sub), apply P to columns. Needs careful permutation handling (perm⁻¹ on columns, not perm on rows).
- **Cache blocking**: Process rows in blocks to fit in L2 cache (256KB). For n=256, augmented matrix is 1MB (fits in 35MB L3 but not L2). Block size ~64 rows = 128KB per half.
- **Strassen-like block inversion**: Use block matrix inversion formula for better cache behavior on very large matrices.

### SIMD
- **Explicit AVX2 intrinsics**: Use `_mm256_fmadd_pd` etc. for full control over SIMD. Complex but potentially 10-20% faster.
- **AVX-512 fallback**: If available, use 512-bit SIMD (8 doubles/instruction). Requires runtime CPU detection.

### Memory
- **Column-major right half**: Store the inverse being built in column-major order for better write patterns during elimination.
- **Single allocation**: Allocate augmented matrix and result buffer together for better locality.
- **Stack allocation for small matrices**: Use alloca for n<=32 to avoid malloc overhead entirely.

### Parallel
- **Block-level parallelism**: Parallelize across blocks of rows instead of individual rows, reducing OpenMP overhead.
- **Thread-local buffers**: Each thread works on its own block of rows, reducing cache contention.
- **Nested parallelism**: Parallelize both the row loop and the inner SIMD loop.

### Compiler
- **Profile-guided optimization (PGO)**: Use `-fprofile-generate` + `-fprofile-use` for better inlining/unrolling decisions.
- **Link-time optimization (LTO)**: Use `-flto` for cross-function optimization.
- **Different unroll factors**: Try 4x, 16x, or 32x instead of 8x.

### Correctness
- **Condition number estimation**: Add optional condition number check to warn about near-singular matrices.
- **Iterative refinement**: One step of iterative refinement to improve accuracy without significant overhead.
