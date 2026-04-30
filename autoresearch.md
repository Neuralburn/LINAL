# Autoresearch: mat_inv speed optimization

## Objective
Optimize `mat_inv` (matrix inversion via Gauss-Jordan elimination with partial pivoting) in `src/linal.c`. Uses augmented matrix [A|I] with stride=2*n, which doubles memory traffic vs standard LU decomposition.

Current state: Has O3 attr + ivdep + OpenMP parallel for with threshold n>=8 (was n>=32 originally).

## Metrics
- **Primary**: `mat_inv_ms` (ms, lower is better) — wall-clock time for 256×256 inversion at O3
- **Secondary**: `all_sizes_ms` — times for all tested sizes (4,8,16,32,64,128,256), comma-separated

## How to Run
`./autoresearch.sh` — builds and runs the benchmark. Outputs `METRIC mat_inv_ms=number`.

## Files in Scope
- **`src/linal.c`** — only file modified. Target function: `mat_inv()` (~lines 878-990). May add O3 attr options, OpenMP optimizations (num_threads, taskloop, threshold tuning), ivdep hints, loop unrolling, prefetch hints, restrict pointers.
- **`tests/benchmark_mat_inv.c`** — read-only benchmark. Used by autoresearch.sh to compile and measure.

## Off Limits
- Do not change the function signature or API contract of `mat_inv`.
- Do not modify test files or meson.build.
- Do not add external dependencies.

## Constraints
- Must compile cleanly with `gcc -O3 -std=c11`.
- All existing unit tests (`meson test -C build`) must pass after each keep.
- Inverse must be numerically correct: A * inv(A) ≈ I within EPSILON=1e-4 relative error for all tested sizes.
- Must handle edge cases: null pointers, non-square matrices, singular matrices (return -1).

## What's Been Tried

### Successful Optimizations
- **Lower OMP threshold from n>=32 to n>=8** → 3.53ms → ~3.15ms (-11%). Gauss-Jordan eliminates ALL rows per column (not just lower triangular), so even small n has enough work for OpenMP threads. Each row SAXPY processes stride=2*n doubles (4KB at n=256).

### Threshold Curve Mapped (parabolic optimum at 8)
| Threshold | Result | Notes |
|---|---|---|
| n >= 2 | ~3.32ms | Worse than baseline — thread overhead dominates |
| n >= 4 | ~3.53ms | Same as baseline — no parallelism benefit |
| **n >= 8** | **~3.15ms** | **OPTIMAL — sweet spot** |
| n >= 16 | ~3.53ms | Worse than baseline — less parallelism |

### Dead Ends
- **taskloop without enclosing `#pragma omp parallel`**: O(n²) tasks created per call → catastrophic slowdown (~18ms vs 3.5ms). Taskloop needs an enclosing parallel region + single construct to work properly.
- **num_threads(4)**: Slower than default system threads (~5.5ms vs 3.2ms). Gauss-Jordan has more parallelism potential than mat_det — eliminating ALL rows per column means more work per thread. System default (all cores) is better here.
- **num_threads(8)**: Slightly worse than no num_threads (~3.32ms vs 3.15ms). Still not optimal.
- **restrict pointers + ivdep**: Broke aliasing assumptions → slower or compilation errors. The augmented matrix [A|I] has specific memory layout that restrict violates.
- **Prefetch hints in inner loop**: No measurable benefit over `#pragma omp simd`. The auto-vectorizer already handles this well.
- **8× manual unroll**: No improvement over `#pragma omp simd` auto-vectorization. Stride=2*n means plenty of data per SAXPY for the compiler's vectorizer.
- **Blocked Gauss-Jordan (B=8)**: Not yet tried — complex implementation with preprocessor/runtime if bugs. Would process multiple columns together to reduce dest writes.

### Key Insights
- Gauss-Jordan is fundamentally different from LU decomposition: eliminates ALL rows per column → more parallelism potential but also more memory traffic.
- The augmented matrix [A|I] has stride=2*n which doubles memory bandwidth requirements vs standard LU. This makes parallelism even more important.
- OpenMP fork/join overhead is the main bottleneck — threshold tuning matters significantly.
- System default thread count works better than limiting threads for this workload (unlike mat_det where num_threads(4) was optimal).
- `#pragma omp simd` on inner loop is already doing a good job with auto-vectorization; manual unrolling doesn't help much.
- The normalization step (pivot row scaling) is serial and could potentially benefit from vectorization or software pipelining.

## Latest Config (3.11ms)
- OMP threshold: n >= 8 (optimal from curve mapping)
- Compile flags: `-O3 -march=native -funroll-loops`
- No num_threads() — system default threads work best for Gauss-Jordan

### Combined Improvements
| Change | Result | Delta |
|---|---|---|
| Baseline (n>=32) | 3.53ms | baseline |
| Threshold n>=8 | ~3.15ms | -10% |
| + -funroll-loops | **~3.11ms** | **-12%** total |
