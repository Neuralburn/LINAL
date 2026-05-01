#!/bin/bash
set -euo pipefail

# autoresearch.sh — build and run mat_inv benchmark
# Outputs METRIC lines parsed by run_experiment

cd "$(dirname "$0")/.."  # project root

BUILD_DIR="build"

# Fast syntax check (compile only, <1s)
if ! gcc -fsyntax-only -std=c11 -I include src/linal.c 2>/dev/null; then
    echo "FAIL: syntax error in linal.c" >&2
    exit 1
fi

# Build if needed (reuse existing build dir for speed)
if [ ! -f "$BUILD_DIR/linal.a" ] || [ src/linal.c -nt "$BUILD_DIR/linal.a" ]; then
    meson setup "$BUILD_DIR" --wipe --buildtype=release -Dbuild_tests=false >/dev/null 2>&1
    meson compile -C "$BUILD_DIR" >/dev/null 2>&1
fi

# Compile standalone benchmark (fast, links against library)
BENCH="$BUILD_DIR/_bench_mat_inv"
gcc -O2 -std=c11 -I include \
    .autoresearch/bench_mat_inv.c \
    build/src/liblinal.a -lm -fopenmp -lgomp \
    -o "$BENCH" 2>/dev/null || { echo "FAIL: bench compile"; exit 1; }

# Run benchmark — outputs METRIC lines
exec "$BENCH"
