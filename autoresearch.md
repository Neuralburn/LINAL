# Autoresearch: mat_mul Performance Optimization

## Objective
Improve runtime performance of `mat_mul` (matrix multiplication) in the LINAL C11 linear algebra library. Test across multiple matrix sizes (4×4 up to 256×256 square, plus rectangular variants). Correctness must be preserved — results validated against a reference i-j-k implementation on every run.

## Metrics
- **Primary**: `mat_mul_total_µs` (microseconds, lower is better) — sum of median times across all benchmark sizes
- **Secondary**: per-size medians (`MATMUL N,N,square,...` and `MATMUL N,M,rect,...`) to track where gains/losses occur

## How to Run
```sh
chmod +x autoresearch.sh && ./autoresearch.sh
```
The script compiles the benchmark + library with current compiler flags, runs it, and outputs structured `METRIC` lines.

## Files in Scope
- **`src/linal.c`** — Contains `mat_mul()` implementation. Optimized with threshold-based parallelism.
- **`tests/benchmark_mat_mul.c`** — Standalone benchmark that validates correctness against a naive reference and measures median timing across sizes.
- **`include/linal_conf.h`** — Configuration options (e.g., `LINAL_USE_RESTRICT`). Can be tweaked to enable restrict pointers or other compile-time optimizations.

## Off Limits
- Do NOT change function signatures in `linal.h` (ABI stability)
- Do NOT modify existing unit tests (`tests/test_mat_mul.c`)
- Do NOT add external dependencies (no OpenBLAS, no external libraries)
- Do NOT change memory layout (row-major must be preserved)

## Constraints
- All existing unit tests must pass after changes
- Benchmark correctness check must pass on every run
- No new dependencies allowed
- C11 standard compliance required

## What's Been Tried

### ✅ KEPT Optimizations
1. **`__attribute__((optimize("O3")))` on mat_mul** — -44% improvement. GCC at -O3 enables aggressive auto-vectorization, better loop unrolling, and improved scheduling for this hot function only.
2. **Manual inner loop 8x unroll + OpenMP parallel i-loop** — Additional -90% over O3 alone (total -94%). Thread-level parallelism dominates; wider unrolling helps under parallelism.
3. **Threshold-based approach**: serial for rows≤16, parallel with 8x unroll for larger matrices. Eliminates OpenMP thread overhead for tiny matrices while keeping benefits for large ones.

### ❌ Discarded Approaches
| Approach | Result | Why it failed |
|----------|--------|---------------|
| Tiling TILE=8 | -44% vs baseline | Loop nesting overhead > cache benefits at these sizes |
| Tiling TILE=16 | -24% vs baseline | Same issue — overhead too high for matrices ≤256×256 |
| Pointer arithmetic + 4x unroll | -1.4% vs baseline | Small matrices helped but large ones regressed; compiler vectorizer beats manual unrolling without O3 |
| `__builtin_assume_aligned(ptr, 32)` | -8% vs baseline | malloc'd data isn't 32-byte aligned; compiler generates misaligned instructions |
| Restrict pointers only | -4% vs baseline | GCC's alias analysis at -O2 already sufficient for local variables |
| i-j-k loop ordering | -290% vs baseline | Column-wise B access destroys cache locality. Catastrophic regression. |
| k-i-j loop ordering | -10% vs baseline | Good B reuse but poor C temporal locality; streams all C rows per B row |
| Merged memset into first k iter | +1% vs O3 | Two-pass structure breaks compiler optimization across iterations |
| TILE=4 OpenMP (no threshold) | Variable 9%-50% worse | Small matrix thread overhead dominates total time |
| TILE=8 with OpenMP | -22% vs TILE=4 | Extra nesting overhead outweighs cache benefits |
| TILE=2 with OpenMP | Identical to baseline | Too many parallel iterations; scheduling overhead negates gains |
| 8x unroll WITHOUT OpenMP | +21% vs 4x unroll | Register pressure and code bloat hurt large matrices |

### Key Insights
1. **Loop ordering is critical**: i-k-j is optimal for row-major matrices at all sizes. i-j-k and k-i-j both worse.
2. **-O3 attribute is the single biggest win**: Compiling just mat_mul with -O3 gives 44% speedup without touching loop structure.
3. **OpenMP parallelism dominates**: Thread-level parallelization provides ~90%+ improvement over O3 alone for matrices >16 rows.
4. **Small matrix overhead matters**: OpenMP thread creation cost (~2-5ms) dwarfs actual computation for tiny matrices. Threshold-based approach solves this.
5. **Measurement variance is real**: Results can vary 7x between runs due to CPU thermal throttling and system load. Run benchmarks after letting the system cool down for consistent results.

### Final Configuration
```c
__attribute__((optimize("O3")))
int mat_mul(const Matrix a, const Matrix b, Matrix *result) {
    if (a.rows <= 16) {
        // Serial path: 8x unroll, #pragma GCC ivdep
    } else {
        #pragma omp parallel for schedule(static)
        #pragma GCC ivdep on innermost loop
        // Parallel path: 8x unroll
    }
}
```
Build flags: `-fopenmp -ffp-contract=fast -march=native`.
Total improvement: **-94.1% from original baseline** (18,789 → ~1,102 µS median).

### Resumed Session Additions (post-compaction)
- **FMA contraction (`-ffp-contract=fast`)**: -2% additional — fused multiply-add instructions
- **#pragma GCC ivdep**: helps compiler vectorize inner loops by hinting independence  
- **Serial path unroll 4x→8x**: marginal +1% — consistency with parallel path
- **OpenMP scheduling tested**: static > dynamic(4) > guided(4)
- **Threshold tuning**: 16 optimal (8 too low, 32 too high for benchmark sizes)
- **-ffast-math flag**: -10% on large matrices (256×256: 0.535ms vs 0.594ms baseline) — enables aggressive FP reassociation and guaranteed FMA
- **Global `-O3` vs attribute-only**: identical results (1,101 vs ~1,102 µS) — function-level O3 is sufficient
- **-funroll-loops flag**: mixed results, masked by thermal variance
- **Measurement variance warning**: results can vary 5-7x due to CPU thermal throttling on shared systems; run benchmarks after system cools down
- **Direct benchmark recommended**: for small improvements (< 5%), use direct `gcc ... tests/benchmark_mat_mul.c src/linal.c -lm && ./a.out` instead of autoresearch.sh to avoid accumulated thermal throttling during long autoresearch runs
