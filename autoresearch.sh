#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"

# Compiler flags — override with EXTRA_CFLAGS env var for autoresearch iterations
CFLAGS="${EXTRA_CFLAGS:--O3 -march=native -fopenmp}"

echo "=== Building linal (CFLAGS=$CFLAGS) ==="
meson setup build --wipe --buildtype=release \
  -Dc_args="$CFLAGS" -Dcpp_args="$CFLAGS" \
  -Dbuild_tests=true 2>&1 | tail -3
meson compile -C build 2>&1 | tail -3

# Compile benchmark standalone (links against built library)
echo "=== Compiling benchmark ==="
gcc $CFLAGS \
  -I include -I build \
  tests/benchmark_mat_add.c \
  -L build -llinal -lm -fopenmp \
  -o benchmark_mat_add

# Run benchmark and capture output
echo "=== Running benchmark ==="
OUTPUT=$(./benchmark_mat_add 2>&1)
echo "$OUTPUT"

# Check correctness (exit code from METRIC line if fail_count > 0)
if echo "$OUTPUT" | grep -q "METRIC mat_add_ns_elt=999999"; then
    exit 1
fi

# Extract primary metric
NS_ELT=$(echo "$OUTPUT" | grep "METRIC mat_add_ns_elt=" | sed 's/.*mat_add_ns_elt=\([0-9.]*\)/\1/')
echo "PRIMARY_METRIC: ${NS_ELT} ns/element"
