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
- **`src/linal.c`** — Contains `mat_mul()` implementation. This is the primary target for optimization.
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
_(Updated as experiments accumulate)_
