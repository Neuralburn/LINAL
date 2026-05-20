# LINAL Documentation

This directory contains the documentation for the LINAL library, generated using Doxygen and Sphinx/Breathe.

## Prerequisites

The following packages are required:

- `doxygen` - For parsing C/C++ source code and generating XML
- `python-sphinx` - For generating HTML documentation
- `python-breathe` - For integrating Doxygen XML with Sphinx

## Building Documentation

### Quick Start

Generate HTML documentation:

```bash
cd docs
make html
```

The documentation will be available in `docs/_build/html/index.html`.

### Available Targets

```bash
make all        # Generate HTML documentation (default)
make doxygen    # Generate Doxygen XML output only
make html       # Generate Sphinx HTML documentation
make latex      # Generate LaTeX files
make pdf        # Build PDF documentation
make view       # Generate HTML and open in browser
make clean      # Remove build directory
make check-deps # Verify all dependencies are installed
make help       # Show help message
```

## Documentation Structure

- `Doxyfile` - Doxygen configuration file
- `conf.py` - Sphinx configuration file
- `index.rst` - Main documentation page
- `operations.rst` - Narrative guide for matrix operations and examples
- `api/` - API reference documentation
- `_build/` - Generated output (gitignored)

## Source Files

The documentation is generated from:

- `include/linal.h` - Main library header with Doxygen annotations
- `include/linal_conf.h` - Configuration header

## Adding New Documentation

1. Add Doxygen comments to your source code in the standard format
2. Update `index.rst`, `operations.rst`, or create new `.rst` files as needed
3. Run `make html` to regenerate the documentation

## Doxygen Comment Format

Use standard Doxygen format for documenting functions, structs, and variables:

```c
/**
 * @brief Function description
 * @param param1 Description of parameter 1
 * @return Description of return value
 */
```
