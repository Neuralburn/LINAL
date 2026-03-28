LINAL Documentation
===================

A lightweight linear algebra library for scientific computing.

Table of Contents
-----------------

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   api/modules

API Reference
-------------

The LINAL library provides a comprehensive set of matrix operations for
scientific computing. All functions are documented below.

.. doxygenstruct:: Matrix
   :project: linal
   :members:

Matrix Operations
~~~~~~~~~~~~~~~~~

Creation and Memory Management
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: mat_create
   :project: linal

.. doxygenfunction:: mat_free
   :project: linal

.. doxygenfunction:: mat_copy
   :project: linal

.. doxygenfunction:: mat_identity
   :project: linal

Arithmetic Operations
^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: mat_add
   :project: linal

.. doxygenfunction:: mat_sub
   :project: linal

.. doxygenfunction:: mat_mul
   :project: linal

.. doxygenfunction:: mat_scale
   :project: linal

.. doxygenfunction:: mat_transpose
   :project: linal

Element Access
^^^^^^^^^^^^^^

.. doxygenfunction:: mat_get
   :project: linal

.. doxygenfunction:: mat_set
   :project: linal

Utility Functions
^^^^^^^^^^^^^^^^^

.. doxygenfunction:: mat_print
   :project: linal

.. doxygenfunction:: mat_norm_l2
   :project: linal

.. doxygenfunction:: mat_trace
   :project: linal

.. doxygenfunction:: mat_det
   :project: linal

.. doxygenfunction:: mat_inv
   :project: linal

.. doxygenfunction:: mat_dot
   :project: linal

Quick Start
-----------

Here's a simple example of how to use LINAL:

.. code-block:: c

   #include <linal.h>
   
   int main() {
       // Create a 3x3 matrix
       Matrix A = mat_create(3, 3);
       
       // Set some values
       mat_set(&A, 0, 0, 1.0);
       mat_set(&A, 0, 1, 2.0);
       mat_set(&A, 0, 2, 3.0);
       
       // Create identity matrix
       Matrix I = mat_identity(3);
       
       // Multiply matrices
       Matrix result;
       mat_mul(A, I, &result);
       
       // Clean up
       mat_free(&A);
       mat_free(&I);
       mat_free(&result);
       
       return 0;
   }

Building the Library
--------------------

LINAL uses Meson as its build system. To build the library:

.. code-block:: bash

   # Configure and build (library only)
   meson setup build --wipe --buildtype=release -Dbuild_tests=false
   meson compile -C build
   
   # Configure, build, and run unit tests
   meson setup build --wipe --buildtype=debug -Dbuild_tests=true
   meson compile -C build
   meson test -C build --verbose

See the `CONTRIBUTING <../CONTRIBUTING.md>`_ file for more details.
