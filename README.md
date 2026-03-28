# LINAL

A lightweight, high-performance linear algebra library written in C11. Designed for scientific computing and machine learning applications, with a focus on portability, cache-friendly memory layouts, and clean interfaces.

## Features

- **Dense Matrix Operations**: Full support for matrix creation, destruction, copying, addition, multiplication, scaling, and transposition
- **C11 Standard Compliance**: Strict adherence to C11 specification with portable code
- **Row-Major Memory Layout**: Contiguous memory storage optimized for sequential access patterns
- **Explicit Memory Management**: Safe allocation and deallocation APIs to prevent leaks
- **Error Handling**: Returns status codes instead of exceptions for graceful error recovery
- **Doxygen Documentation**: Comprehensive inline documentation for automatic API generation
- **Meson Build System**: Modern, cross-platform build configuration with pkg-config support

## Using the Library

### As a Meson subproject

```meson
linal_dep = dependency('linal', fallback: ['linal', 'linal_dep'])
```

The template also exports `meson.override_dependency('linal', ...)`
so downstream Meson builds can resolve the subproject dependency by name.

For subproject builds, include the public header directly:

```c
#include "linal.h"
```

### As an installed dependency

If the library is installed system-wide, include the namespaced header path:

```c
#include <linal/linal.h>
```

If `pkg-config` files are installed in your environment, downstream builds can
also discover the package as `linal`.

The generated version header is available as `linal_version.h` in the
build tree and as `<linal/linal_version.h>` after install.

## Building

```sh
# Library only (release)
meson setup build --buildtype=release -Dbuild_tests=false
meson compile -C build

# With unit tests
meson setup build --buildtype=debug -Dbuild_tests=true
meson compile -C build
meson test -C build --verbose
```

## Quick Start

```c
#include "linal.h"

int main(void)
{
    // Create two 3x3 matrices
    Matrix A = mat_create(3, 3);
    Matrix B = mat_create(3, 3);
    Matrix C = mat_create(3, 3);

    // Initialize with some values (manually set data)
    for (size_t i = 0; i < 9; i++) {
        A.data[i] = (double)(i + 1);
        B.data[i] = (double)(i + 1) * 2;
    }

    // Print matrices
    mat_print("Matrix A:", A);
    mat_print("Matrix B:", B);

    // Add matrices: C = A + B
    mat_add(A, B, C);
    mat_print("Matrix C (A + B):", C);

    // Multiply matrices (requires compatible dimensions)
    Matrix D = mat_create(3, 3);
    mat_mul(A, B, D);
    mat_print("Matrix D (A * B):", D);

    // Clean up
    mat_free(&A);
    mat_free(&B);
    mat_free(&C);
    mat_free(&D);

    return 0;
}
```

## API Reference

### Matrix Lifecycle

```c
Matrix mat_create(size_t r, size_t c);
void mat_free(Matrix *m);
int mat_copy(const Matrix src, Matrix *dest);
```

### Arithmetic Operations

```c
int mat_add(const Matrix a, const Matrix b, Matrix *result);
int mat_mul(const Matrix a, const Matrix b, Matrix *result);
int mat_scale(const Matrix m, double scalar, Matrix *result);
int mat_transpose(const Matrix m, Matrix *result);
int mat_sub(const Matrix a, const Matrix b, Matrix *result);
```

### Element Access

```c
double mat_get(const Matrix m, size_t row, size_t col);
int mat_set(Matrix *m, size_t row, size_t col, double value);
```

### Debug Utilities

```c
void mat_print(const char *label, const Matrix m);
```

For detailed documentation, see the Doxygen comments in `include/linal.h`.

## Use Cases

- **Scientific Computing**: Solve linear systems, perform numerical analysis
- **Machine Learning**: Matrix operations for neural network layers, data transformations
- **Computer Vision**: Image processing with matrix representations
- **Physics Simulations**: Transformations, rotations, and coordinate systems

## Notes

| Topic | Note |
|-------|------|
| **Memory Layout** | Matrices use row-major contiguous memory layout for cache-friendly access patterns |
| **Thread Safety** | API is thread-safe at the level of individual function calls |
| **Error Handling** | Functions return status codes; check return values for errors |
| **Data Types** | All matrix elements are stored as `double` for numerical precision |
| **Dimension Validation** | All arithmetic operations validate dimensions and return error codes on mismatch |
| **Memory Ownership** | Users are responsible for freeing matrices via `mat_free()` to prevent leaks |
