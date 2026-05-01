# Autoresearch Ideas for mat_inv

## Current Best: 2.67ms (-32% vs 3.92ms baseline)
- 4/10 adaptive thread count (n<=128 ? 4 : 10)
- Single prefetch in OpenMP path
- `#pragma omp simd safelen(32)`
- `#pragma GCC target("avx2,fma")`
- 8x unrolled serial loops

## Explored & Rejected
- `num_threads(4)`: Too few for 256x256 (4.03ms)
- `num_threads(14)`: Good but not optimal (3.05ms)
- `schedule(static,4)`: Cache thrashing (3.35ms)
- `schedule(static,16)`: Less parallelism (3.43ms)
- `simdlen(4)`: Restricted compiler (3.24ms)
- `aligned(pr,tr:32)`: False alignment (3.27ms)
- Explicit AVX2 intrinsics: No improvement (3.02ms)
- `linear(factor:1)`: GCC compile error
- Double prefetch: Overhead (2.71ms)
- LU decomposition: Bugs in implementation
- `fp-contract=fast`: No improvement

## Remaining Ideas (untested)

### Thread tuning
- **4/11 split**: Test 11 threads for n>128
- **Dynamic thread count**: Use `omp_get_num_procs()` at runtime
- **Thread affinity**: Pin threads to specific cores

### Algorithmic
- **Cache blocking**: Process rows in L2-cache-sized blocks (~50 rows)
- **Block elimination**: Process multiple columns at a time to reuse pivot rows in cache
- **Strassen-like**: Block matrix inversion formula for large matrices

### Compiler
- **PGO (Profile-Guided Optimization)**: `-fprofile-generate` + `-fprofile-use`
- **LTO (Link-Time Optimization)**: `-flto`
- **Different unroll factors**: 4x, 16x, 32x instead of 8x

### SIMD
- **AVX-512 fallback**: If available, use 512-bit SIMD (8 doubles/instruction)
- **Explicit intrinsics with alignment**: Use aligned loads if memory is aligned

### Memory
- **Column-major right half**: Store inverse in column-major for better write patterns
- **Stack allocation for small matrices**: alloca for n<=32
