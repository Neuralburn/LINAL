# AGENTS.md

## Project-specific instructions

**Project:** `LINAL`
**Primary goal:** A small linear algebra Meson-based C11 library

### Essential commands

#### Configure and build (library only)

```sh
meson setup build --wipe --buildtype=release -Dbuild_tests=false
meson compile -C build
```

#### Configure, build, and run unit tests

```sh
meson setup build --wipe --buildtype=debug -Dbuild_tests=true
meson compile -C build
meson test -C build --verbose
```

### CI / source of truth

- CI definitions live in `.github/workflows/ci.yml`.
- Prefer running the same commands locally as CI runs.
- If `pre-commit` is configured later, run it before committing.

## Docs / commit conventions

- Use Conventional Commits when asked to commit.
- Keep commits focused and explain why the change exists.

## C style expectations

### Build and configuration

- Use the Meson build system; do not introduce another build system.
- Update `meson.build` when adding or removing source files.

### Formatting

- `.clang-format` is present and should be used on modified `.c` and `.h` files.
- Do not reformat unrelated code.
- Key settings: 8-space indent, `BreakBeforeBraces: Linux`, column limit 80.

### Style and correctness

- Match the conventions in the existing files.
- Keep public headers minimal and stable.
- Prefer explicit fixed-width integer types when ABI or serialization matters.
- Use `linal_conf.h` for compile-time configuration options. This header is automatically included by `linal.h` and can be overridden before including the main header.

### Testing

- Run `meson test -C build` after changes.
- Add a test case for each bug fix.
- Keep tests in `tests/test_*.c`.

### Design Validation

The implementation should adhere to the project design document at `docs/design/project_design.md`. Key requirements:

- C11 standard compliance with `_Static_assert` for compile-time checks
- Row-major contiguous memory layout for matrices
- Explicit memory management via `mat_create()` and `mat_free()`
- Doxygen-style comments for all public functions
- Error codes (int return values) instead of exceptions
- Thread-safe API level (no internal parallelization required)

All core functions are implemented and tested.
