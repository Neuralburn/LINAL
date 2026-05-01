#!/bin/bash
set -euo pipefail

# autoresearch.checks.sh — correctness validation
cd "$(dirname "$0")/.."

BUILD_DIR="build"

# Rebuild with tests if needed
if [ ! -f "$BUILD_DIR/tests/test_mat_inv" ] || [ src/linal.c -nt "$BUILD_DIR/tests/test_mat_inv" ]; then
    meson setup "$BUILD_DIR" --wipe --buildtype=debug -Dbuild_tests=true >/dev/null 2>&1
    meson compile -C "$BUILD_DIR" >/dev/null 2>&1
fi

# Run mat_inv tests (suppress verbose output, show only failures)
meson test -C "$BUILD_DIR" --verbose --suite '' 2>&1 | grep -E '(FAIL|OK|mat_inv)' | tail -20
FAILED=$(meson test -C "$BUILD_DIR" --print-errorlogs 2>&1 | grep -c 'FAIL' || true)
if [ "$FAILED" -gt 0 ]; then
    echo "CHECKS FAILED: $FAILED test(s)" >&2
    exit 1
fi
