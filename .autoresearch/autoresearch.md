# Autoresearch: mat_inv performance optimization

## Objective
Improve runtime performance of `mat_inv` (matrix inverse via Gauss-Jordan elimination) in `src/linal.c`.

## Results
- **Baseline**: 3.92ms (256×256), 1.62ms (128×128), 0.68ms (64×64)
- **Best**: 3.04ms (256×256), 1.15ms (128×128), 0.51ms (64×64)
- **Improvement**: ~22% faster across all sizes
- **Confidence**: 4.9× noise floor

## Winning Optimizations (all applied)
1. **8x loop unrolling** on inner j-loop with `#pragma GCC ivdep` (serial path)
2. **Skip zero columns** - left half starts at col+1 (already zeroed below diagonal)
3. **`#pragma GCC target("avx2,fma")`** - forces AVX2 256-bit SIMD + FMA instructions
4. **`__restrict__` pointers** on row pointers for better compiler optimization
5. **`__builtin_prefetch`** for next target row during elimination
6. **`__builtin_alloca`** for row swap (simpler than conditional malloc)
7. **Explicit pivot=1.0** and **tr[col]=0.0** instead of dividing/eliminating

## How to Run
`.autoresearch/autoresearch.sh` — outputs `METRIC mat_inv_ms=...`, `METRIC mat_inv_128_ms=...`, `METRIC mat_inv_64_ms=...` lines. Median of 20 iterations per size.

## Files in Scope
- `src/linal.c` — contains `mat_inv` implementation
- `.autoresearch/bench_mat_inv.c` — standalone benchmark binary source
- `.autoresearch/autoresearch.sh` — build + run script

## What's Been Tried

### Successful
- 8x unrolled inner loops + skip zero columns: ✅ ~10-20% improvement
- `__restrict__` + `__builtin_prefetch`: ✅ ~5-10% improvement  
- `#pragma GCC target("avx2,fma")`: ✅ ~5-10% improvement
- `__builtin_alloca` for row swap: ✅ neutral/slight improvement, simpler code

### Failed
- Split arrays (A_part + R_part): ❌ Cache contention between two large arrays
- Pointer indirection for row swap: ❌ Extra indirection overhead on every access
- Two OpenMP parallel regions: ❌ Massive fork/join overhead inside column loop
- Lower OpenMP threshold (n>=16): ❌ No benefit, serial path not used for benchmark sizes
- `schedule(static,1)`: ❌ Chunk overhead
- `schedule(guided)`: ❌ Dynamic scheduling overhead catastrophic in tight loop
- `num_threads(4)`: ❌ Too few threads for 28-thread system
- `aligned_alloc(32, ...)`: ❌ No consistent improvement
- `__builtin_assume_aligned(32)`: ❌ Compiler already generates optimal SIMD
- `unroll-loops` flag: ❌ Register pressure and code bloat
- LU decomposition: ❌ Complex to implement correctly, bugs in back substitution + permutation

## Constraints
- Tests must pass (`meson test -C build`)
- C11 standard compliance
- Same numerical accuracy (within existing tolerance ~1e-4)
- No new dependencies

## System
- Intel Xeon E5-2680 v4 @ 2.40GHz (Broadwell, AVX2+FMA, no AVX-512)
- 28 threads (14 cores × 2 HT)
- 35MB L3 cache
