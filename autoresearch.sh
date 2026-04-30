#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

# ── Pre-check: compile (fast) ────────────────────────────────────────
gcc -O3 -march=native -std=c11 -D_GNU_SOURCE -fopenmp \
    -I include -I build \
    -lm -lgomp tests/benchmark_mat_inv.c src/linal.c \
    -o /tmp/bench_inv 2>&1 | grep -i error || true
if [ ! -f /tmp/bench_inv ]; then
    echo "BUILD FAILED" >&2
    exit 1
fi

# ── Warmup: prime OpenMP thread pool and caches (5 iterations) ─────
for _ in 1 2 3 4 5; do /tmp/bench_inv > /dev/null 2>&1; done

# ── Run benchmark (3 repeats, report median) ────────────────────────
times=()
for _ in 1 2 3; do
    out=$(/tmp/bench_inv 2>/dev/null)
    ms=$(echo "$out" | grep 'METRIC mat_inv_ms' | sed 's/.*mat_inv_ms=//')
    times+=("$ms")
done

# Print individual runs for diagnostics
for i in "${!times[@]}"; do
    printf 'METRIC mat_inv_run_%dms=%.6f\n' "$i" "${times[$i]}"
done

# Compute median
sorted=$(printf '%s\n' "${times[@]}" | sort -g)
n=${#times[@]}
mid=$(( (n + 1) / 2 ))
median=$(echo "$sorted" | sed -n "${mid}p")

printf 'METRIC mat_inv_ms=%.6f\n' "$median"
