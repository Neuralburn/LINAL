# Autoresearch: mat_inv performance optimization — COMPLETE

## Final Results
| Size | Baseline | Optimized | Improvement |
|------|----------|-----------|-------------|
| 64×64 | 0.68ms | 0.49ms | **-28%** |
| 128×128 | 1.62ms | 1.12ms | **-31%** |
| 256×256 | 3.92ms | 3.00ms | **-23.5%** |

**Confidence: 5.4× noise floor** — improvement is very likely real.

## Winning Optimizations (all applied in src/linal.c)

1. **`#pragma GCC target("avx2,fma")`** — Forces AVX2 256-bit SIMD + FMA instructions (140 FMA ops generated)
2. **8x loop unrolling** with `#pragma GCC ivdep` — Serial path processes 8 elements per iteration
3. **Skip zero columns** — Left half starts at col+1 (already zeroed below diagonal)
4. **`__restrict__` pointers** — Helps compiler optimize memory access patterns
5. **`__builtin_prefetch`** — Prefetches next target row during elimination
6. **`#pragma omp simd safelen(32)`** — Hints 32-byte vectorization width for OpenMP path
7. **`__builtin_alloca`** for row swap — Simpler than conditional malloc
8. **Explicit pivot=1.0** and **tr[col]=0.0** — Avoid redundant division/elimination

## System
- Intel Xeon E5-2680 v4 @ 2.40GHz (Broadwell, AVX2+FMA, no AVX-512)
- 28 threads (14 cores × 2 HT), 35MB L3 cache

## What Was Tried (16 experiments)

### Successful (kept)
- 8x unrolled inner loops + skip zero columns
- `__restrict__` + `__builtin_prefetch`
- `#pragma GCC target("avx2,fma")`
- `__builtin_alloca` for row swap
- `#pragma omp simd safelen(32)`

### Failed (discarded)
- Split arrays (A_part + R_part): Cache contention
- Pointer indirection for row swap: Indirection overhead
- Two OpenMP parallel regions: Fork/join overhead
- Lower OpenMP threshold: No benefit
- `schedule(static,1)`, `schedule(guided)`: Scheduling overhead
- `num_threads(4)`: Too few threads
- `aligned_alloc(32, ...)`: No consistent improvement
- `__builtin_assume_aligned(32)`: Compiler already optimal
- `unroll-loops` flag: Register pressure
- LU decomposition: Complex, bugs in implementation
- `fp-contract=fast`: No improvement

## Remaining Ideas (see autoresearch.ideas.md)
- Explicit AVX2 intrinsics
- Cache blocking for L2 fit
- Profile-guided optimization (PGO)
- Link-time optimization (LTO)
- AVX-512 fallback (if available)
