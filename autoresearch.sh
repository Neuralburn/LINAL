#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

# ── Pre-check: compile (fast) ────────────────────────────────────────
gcc -O3 -std=c11 -D_GNU_SOURCE -fopenmp \
    -I include -I build \
    -lm -lgomp tests/benchmark_mat_det.c src/linal.c -o /tmp/bench_det 2>&1 || exit 1

# ── Run benchmark 3× and take median of last (256×256) time ─────────
run_once() {
    /tmp/bench_det 2>/dev/null | grep '^METRIC '
}

results=()
for i in 1 2 3; do
    results+=("$(run_once)")
done

# Parse mat_det_ms from each run
vals=()
for r in "${results[@]}"; do
    val="${r#*=}"
    vals+=("$val")
done

# Sort numerically and pick median (index 1 of 3)
IFS=$'\n' sorted=($(printf '%s\n' "${vals[@]}" | sort -g)); unset IFS
median="${sorted[1]}"

echo "METRIC mat_det_ms=${median}"
for i in 0 1 2; do
    echo "METRIC mat_det_run_${i}ms=${vals[$i]}"
done
