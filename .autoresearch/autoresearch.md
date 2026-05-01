# Autoresearch: mat_inv performance optimization

## Objective
Improve runtime performance of `mat_inv` (matrix inverse via Gauss-Jordan elimination) in `src/linal.c`. The function works on square matrices of any size and uses an augmented matrix [A | I]. Primary workload is medium-to-large matrices (64x64 to 256x256) where O(n^3) dominates.

## Metrics
- **Primary**: `mat_inv_ms` (ms, lower is better) — average time for mat_inv on 256x256 matrix
- **Secondary**: `mat_inv_128_ms` — avg time for 128x128; `mat_inv_64_ms` — avg time for 64x64

## How to Run
`.autoresearch/autoresearch.sh` — outputs `METRIC mat_inv_ms=...`, `METRIC mat_inv_128_ms=...`, `METRIC mat_inv_64_ms=...` lines. Runs median of 5 iterations per size for stability.

## Files in Scope
- `src/linal.c` — contains `mat_inv` implementation (lines ~790-end). May also touch `mat_mul` if used internally.
- `.autoresearch/bench_mat_inv.c` — standalone benchmark binary source
- `.autoresearch/autoresearch.sh` — build + run script

## Off Limits
- Public API in `include/linal.h` (no signature changes)
- Error handling contract (return codes, stderr messages)
- Correctness: all unit tests must pass
- No new dependencies (only C11, libm, OpenMP if available)

## Constraints
- Tests must pass (`meson test -C build`) after each change
- C11 standard compliance
- Row-major memory layout preserved
- Same numerical accuracy (within existing tolerance ~1e-4 for large matrices)

## Current Implementation Analysis
The current `mat_inv` uses Gauss-Jordan elimination with:
1. Augmented matrix [A | I] in single flat array, stride=2n
2. Partial pivoting (largest abs value in column below diagonal)
3. Row normalization (divide pivot row by pivot element)
4. Column elimination across all other rows
5. OpenMP parallelization for n >= 32 on the elimination loop
6. `#pragma GCC ivdep` for auto-vectorization of inner loops
7. memcpy-based row swap and extraction

## Optimization Opportunities
- Separate augmented matrix into two arrays (A_part, I_part) to halve stride and improve cache utilization during normalization/elimination
- Skip column already processed in elimination (j starts from col+1 for A part since below-diagonal is zeroed; but right side needs full sweep)
- Loop unrolling of inner j-loop (like mat_mul does with 8x unroll)
- Better OpenMP parallelization strategy (taskloop vs parallel for, lower threshold)
- Use `__restrict__` pointers throughout
- Block/tiling optimization for very large matrices to fit in L1/L2 cache
- Avoid redundant work: after normalization pivot_row[col]=1.0, so elimination at j=col is just setting aug[i*stride+col] = 0 (or skipping it)

## What's Been Tried
*(Updated after each experiment)*
