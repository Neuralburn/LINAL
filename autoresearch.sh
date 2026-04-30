#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

# Quick syntax check (compile without linking) — must pass
gcc -fsyntax-only -O3 -march=native -fopenmp -I include -I build src/linal.c 2>/dev/null || { echo "COMPILE FAIL"; exit 1; }

# Compile benchmark + library
gcc -O3 -march=native -fopenmp \
    -I include -I build \
    -o /tmp/bench_norm_l2 \
    tests/benchmark_mat_norm.c src/linal.c -lm 2>/dev/null || { echo "COMPILE FAIL"; exit 1; }

# Heavy warmup: run multiple times to stabilize CPU frequency
for i in $(seq 1 5); do
    /tmp/bench_norm_l2 > /dev/null 2>&1
done

# Run benchmark — outputs METRIC line + size-by-size results
/tmp/bench_norm_l2 2>&1
