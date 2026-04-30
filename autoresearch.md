# Autoresearch: mat_inv speed optimization

## Objective
Optimize `mat_inv` (matrix inversion via Gauss-Jordan elimination with partial pivoting) in `src/linal.c`. Uses augmented matrix [A|I] with stride=2*n, which doubles memory traffic vs standard LU decomposition.

Current state: Has O3 attr + ivdep + OpenMP parallel for but **threshold is conservative** (`n >= 32`) and **no num_threads() specified**.

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
(Empty — fresh session. mat_det findings may provide transferable insights.)
