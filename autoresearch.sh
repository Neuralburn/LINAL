#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

# Quick syntax check — compile the benchmark
CC="${CC:-gcc}"
CFLAGS="${CFLAGS:--O2 -march=native -fopenmp}"

echo "=== Compiling with $CC $CFLAGS ==="
$CC $CFLAGS \
    -I include \
    tests/benchmark_mat_mul.c \
    src/linal.c \
    -lm \
    -o /tmp/benchmark_mat_mul 2>&1 || { echo "COMPILE FAIL"; exit 1; }

# Run benchmark, capture output
echo "=== Running benchmark ==="
OUTPUT=$(/tmp/benchmark_mat_mul 2>&1) || {
    echo "$OUTPUT"
    echo "RUNTIME FAIL"
    exit 1
}
echo "$OUTPUT"

# Extract and sum all median times for a single scalar metric
TOTAL_US=$(echo "$OUTPUT" | grep '^MATMUL' \
    | awk -F, '{total += $4 * 1000} END {printf "%.0f", total}')

echo "METRIC mat_mul_total_µs=$TOTAL_US"
echo "$OUTPUT" | grep 'VALID OK' > /dev/null && echo "CORRECT: YES" || echo "CORRECT: NO"
