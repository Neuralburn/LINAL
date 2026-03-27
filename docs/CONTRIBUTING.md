# Contributing to LINAL

Thank you for your interest in contributing to LINAL! This document provides guidelines and instructions for contributing to the project.

## Quick Links

- [Project Design Document](docs/design/project_design.md)
- [AGENTS.md](AGENTS.md) - Build commands and conventions

## Development Workflow

### 1. Setup Your Environment

```sh
# Clone the repository
git clone https://github.com/yourusername/linal.git
cd linal

# Configure build with tests
meson setup build --wipe --buildtype=debug -Dbuild_tests=true

# Build the project
meson compile -C build

# Run tests
meson test -C build --verbose
```

### 2. Coding Standards

#### C Code Style

- **Standard**: C11 compliance is mandatory
- **Indentation**: 8 spaces (no tabs)
- **Line Length**: Maximum 80 characters
- **Braces**: Linux style (opening brace on new line)
- **Formatting**: Use `.clang-format` for consistent formatting

```sh
# Format modified files
clang-format -i src/linal.c include/linal.h
```

#### Memory Management

- All dynamically allocated memory must be freed
- Use `mat_create()` for allocation and `mat_free()` for deallocation
- Validate all pointer inputs for NULL before dereferencing
- Check dimensions before array access to prevent buffer overflows

#### Error Handling

- Return `int` status codes (0 for success, -1 for errors)
- Never use exceptions or `assert()` in production code
- Use `_Static_assert` for compile-time checks only

#### Documentation

- All public functions must have Doxygen-style comments
- Include `@brief`, `@param`, and `@return` where applicable
- Document error conditions and preconditions

Example:

```c
/**
 * @brief Scale a matrix by a scalar factor.
 * @param m Input matrix to scale
 * @param scalar Scalar multiplier
 * @param result Output matrix containing the scaled values (must not alias m)
 * @return 0 on success, -1 if input is invalid
 */
int mat_scale(const Matrix m, double scalar, Matrix *result);
```

### 3. Testing Requirements

- **All bug fixes must include tests**: Add test cases to `tests/test_*.c`
- **Test coverage**: New features must have comprehensive test coverage
- **Edge cases**: Test NULL inputs, dimension mismatches, and boundary conditions

Run tests after every change:

```sh
meson compile -C build
meson test -C build --verbose
```

Example test structure:

```c
static void test_mat_scale_dimensions_mismatch(void)
{
    Matrix m   = mat_create(2, 2);
    Matrix out = mat_create(3, 3);

    assert(m.data != NULL);
    assert(out.data != NULL);

    int ret = mat_scale(m, 2.0, &out);
    assert(ret == -1);

    mat_free(&m);
    mat_free(&out);
}
```

### 4. Build System

- **Meson only**: Do not introduce alternative build systems
- **Update meson.build**: Add/remove source files in `meson.build` when changing the codebase
- **Configuration**: Use `linal_conf.h` for compile-time options

### 5. Commit Guidelines

- **Conventional Commits**: Follow the format `type(scope): description`

### 6. Pull Request Process

tbd
