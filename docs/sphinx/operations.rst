Operations Guide
================

This guide expands on the public API reference with matrix definitions,
practical examples, and a few core algebraic properties. It is intended to
complement the Doxygen API docs rather than duplicate every parameter detail.

Matrix Representation
---------------------

LINAL stores matrices in row-major contiguous memory. An element at ``(i, j)``
is stored at ``data[i * cols + j]``.

.. code-block:: text

   Matrix A (2x3):
       [ a00  a01  a02 ]
       [ a10  a11  a12 ]

   Memory layout: [a00, a01, a02, a10, a11, a12]
   Index formula: A[i][j] -> data[i * cols + j]

Most operations that write a result expect the output matrix to be created in
advance with matching dimensions.

Core Operations
---------------

Matrix Addition
~~~~~~~~~~~~~~~

**Function:** ``int mat_add(const Matrix a, const Matrix b, Matrix *result)``

.. math::

   C = A + B

.. math::

   c_{ij} = a_{ij} + b_{ij}

Matrix addition is defined element-wise and requires ``a`` and ``b`` to have
the same shape.

Properties:

- Commutative: :math:`A + B = B + A`
- Associative: :math:`(A + B) + C = A + (B + C)`
- Defined only for matching dimensions

.. code-block:: c

   Matrix A = mat_create(2, 2);
   Matrix B = mat_create(2, 2);
   Matrix C = mat_create(2, 2);

   A.data[0] = 1.0; A.data[1] = 2.0;
   A.data[2] = 3.0; A.data[3] = 4.0;
   B.data[0] = 5.0; B.data[1] = 6.0;
   B.data[2] = 7.0; B.data[3] = 8.0;

   mat_add(A, B, &C);  /* C = [6, 8; 10, 12] */

   mat_free(&A);
   mat_free(&B);
   mat_free(&C);

Matrix Subtraction
~~~~~~~~~~~~~~~~~~

**Function:** ``int mat_sub(const Matrix a, const Matrix b, Matrix *result)``

.. math::

   C = A - B

.. math::

   c_{ij} = a_{ij} - b_{ij}

Subtraction is also element-wise, but unlike addition it is not commutative.

Properties:

- Not commutative: :math:`A - B \neq B - A`
- :math:`A - A = 0`
- Defined only for matching dimensions

.. code-block:: c

   Matrix A = mat_create(2, 2);
   Matrix B = mat_create(2, 2);
   Matrix C = mat_create(2, 2);

   A.data[0] = 5.0; A.data[1] = 6.0;
   A.data[2] = 7.0; A.data[3] = 8.0;
   B.data[0] = 1.0; B.data[1] = 2.0;
   B.data[2] = 3.0; B.data[3] = 4.0;

   mat_sub(A, B, &C);  /* C = [4, 4; 4, 4] */

   mat_free(&A);
   mat_free(&B);
   mat_free(&C);

Matrix Multiplication
~~~~~~~~~~~~~~~~~~~~~

**Function:** ``int mat_mul(const Matrix a, const Matrix b, Matrix *result)``

.. math::

   C = A B

.. math::

   c_{ij} = \sum_{k=1}^{p} a_{ik} b_{kj}

For :math:`A \in \mathbb{R}^{m \times p}` and
:math:`B \in \mathbb{R}^{p \times n}`, the product is a matrix
:math:`C \in \mathbb{R}^{m \times n}`. Each output element is the dot product
of one row from ``a`` and one column from ``b``.

Properties:

- Not commutative in general: :math:`AB \neq BA`
- Associative: :math:`(AB)C = A(BC)`
- Distributive over addition
- Requires ``a.cols == b.rows``

.. math::

   \begin{bmatrix} 1 & 2 \\ 3 & 4 \end{bmatrix}
   \begin{bmatrix} 5 & 6 \\ 7 & 8 \end{bmatrix}
   =
   \begin{bmatrix} 19 & 22 \\ 43 & 50 \end{bmatrix}

.. code-block:: c

   Matrix A = mat_create(2, 2);
   Matrix B = mat_create(2, 2);
   Matrix C = mat_create(2, 2);

   A.data[0] = 1.0; A.data[1] = 2.0;
   A.data[2] = 3.0; A.data[3] = 4.0;
   B.data[0] = 5.0; B.data[1] = 6.0;
   B.data[2] = 7.0; B.data[3] = 8.0;

   mat_mul(A, B, &C);  /* C = [19, 22; 43, 50] */

   mat_free(&A);
   mat_free(&B);
   mat_free(&C);

Scalar Multiplication
~~~~~~~~~~~~~~~~~~~~~

**Function:** ``int mat_scale(const Matrix m, double scalar, Matrix *result)``

.. math::

   B = \alpha A

.. math::

   b_{ij} = \alpha a_{ij}

Scalar multiplication multiplies every element by the same factor.

Properties:

- Distributive over addition
- :math:`(\alpha \beta)A = \alpha(\beta A)`
- :math:`1A = A`

.. code-block:: c

   Matrix A = mat_create(2, 2);
   Matrix B = mat_create(2, 2);

   A.data[0] = 1.0; A.data[1] = 2.0;
   A.data[2] = 3.0; A.data[3] = 4.0;

   mat_scale(A, 3.0, &B);  /* B = [3, 6; 9, 12] */

   mat_free(&A);
   mat_free(&B);

Matrix Transpose
~~~~~~~~~~~~~~~~

**Function:** ``int mat_transpose(const Matrix m, Matrix *result)``

.. math::

   B = A^T

.. math::

   b_{ij} = a_{ji}

The transpose swaps rows and columns, turning an :math:`m \times n` matrix
into an :math:`n \times m` matrix.

Properties:

- Involutive: :math:`(A^T)^T = A`
- :math:`(A + B)^T = A^T + B^T`
- :math:`(AB)^T = B^T A^T`

.. code-block:: c

   Matrix A = mat_create(2, 3);
   Matrix B = mat_create(3, 2);

   A.data[0] = 1.0; A.data[1] = 2.0; A.data[2] = 3.0;
   A.data[3] = 4.0; A.data[4] = 5.0; A.data[5] = 6.0;

   mat_transpose(A, &B);  /* B = [1, 4; 2, 5; 3, 6] */

   mat_free(&A);
   mat_free(&B);

Identity Matrix
~~~~~~~~~~~~~~~

**Function:** ``Matrix mat_identity(size_t n)``

.. math::

   I_{ij} = \delta_{ij} =
   \begin{cases}
   1 & \text{if } i = j \\
   0 & \text{if } i \neq j
   \end{cases}

The identity matrix is square, with ones on the main diagonal and zeros
elsewhere.

Properties:

- Multiplicative identity: :math:`AI = IA = A`
- Symmetric: :math:`I^T = I`
- Idempotent: :math:`I^2 = I`

.. code-block:: c

   Matrix I = mat_identity(3);
   Matrix A = mat_create(3, 3);
   Matrix C = mat_create(3, 3);

   mat_mul(A, I, &C);  /* C = A */

   mat_free(&I);
   mat_free(&A);
   mat_free(&C);

Norms and Inner Products
------------------------

Frobenius Norm
~~~~~~~~~~~~~~

**Function:** ``double mat_norm_l2(const Matrix *A)``

.. math::

   \|A\|_F = \sqrt{\sum_{i=1}^{m} \sum_{j=1}^{n} |a_{ij}|^2}

This norm treats the matrix as a single vector of entries.

Alternative form:

.. math::

   \|A\|_F = \sqrt{\operatorname{tr}(A^T A)}

Properties:

- Always non-negative
- Zero only for the zero matrix
- :math:`\|\alpha A\|_F = |\alpha| \|A\|_F`

.. code-block:: c

   Matrix A = mat_create(2, 2);

   A.data[0] = 1.0; A.data[1] = 2.0;
   A.data[2] = 3.0; A.data[3] = 4.0;

   double norm = mat_norm_l2(&A);  /* sqrt(30) */

   mat_free(&A);

Trace
~~~~~

**Function:** ``double mat_trace(const Matrix *A)``

.. math::

   \operatorname{tr}(A) = \sum_{i=1}^{n} a_{ii}

The trace is the sum of diagonal entries.

Properties:

- Linear: :math:`\operatorname{tr}(A + B) = \operatorname{tr}(A) + \operatorname{tr}(B)`
- Scalar-compatible: :math:`\operatorname{tr}(\alpha A) = \alpha\operatorname{tr}(A)`
- Cyclic: :math:`\operatorname{tr}(AB) = \operatorname{tr}(BA)`

.. code-block:: c

   Matrix A = mat_create(3, 3);

   A.data[0] = 1.0; A.data[4] = 5.0; A.data[8] = 9.0;

   double trace = mat_trace(&A);  /* 15 */

   mat_free(&A);

Dot Product
~~~~~~~~~~~

**Function:** ``double mat_dot(const Matrix A, const Matrix B)``

.. math::

   A \cdot B = \sum_{i=1}^{m} \sum_{j=1}^{n} a_{ij} b_{ij}

This is the Frobenius inner product of two same-sized matrices.

Alternative form:

.. math::

   A \cdot B = \operatorname{tr}(A^T B)

Properties:

- Commutative: :math:`A \cdot B = B \cdot A`
- Linear in each argument
- :math:`A \cdot A \geq 0`
- Related to the Frobenius norm by :math:`\|A\|_F = \sqrt{A \cdot A}`

.. code-block:: c

   Matrix A = mat_create(2, 2);
   Matrix B = mat_create(2, 2);

   A.data[0] = 1.0; A.data[1] = 2.0;
   A.data[2] = 3.0; A.data[3] = 4.0;
   B.data[0] = 5.0; B.data[1] = 6.0;
   B.data[2] = 7.0; B.data[3] = 8.0;

   double dot = mat_dot(A, B);  /* 70 */

   mat_free(&A);
   mat_free(&B);

Element Access
--------------

Get Element
~~~~~~~~~~~

**Function:** ``double mat_get(const Matrix m, size_t row, size_t col)``

.. math::

   \operatorname{get}(A, i, j) = a_{ij}

The implementation uses row-major indexing internally. If the matrix is
invalid or the indices are out of bounds, the function returns ``NaN``.

.. code-block:: c

   Matrix A = mat_create(2, 3);
   double value;

   A.data[5] = 6.0;
   value = mat_get(A, 1, 2);  /* 6.0 */

   mat_free(&A);

Set Element
~~~~~~~~~~~

**Function:** ``int mat_set(Matrix *m, size_t row, size_t col, double value)``

.. math::

   a_{ij} \leftarrow v

The function returns ``0`` on success and ``-1`` for invalid matrices or
out-of-bounds indices.

.. code-block:: c

   Matrix A = mat_create(2, 2);

   mat_set(&A, 0, 1, 42.0);

   mat_free(&A);

Complexity Overview
-------------------

.. list-table::
   :header-rows: 1

   * - Operation
     - Time complexity
     - Extra space
   * - Addition / subtraction
     - :math:`O(mn)`
     - :math:`O(1)`
   * - Multiplication
     - :math:`O(mnp)`
     - :math:`O(1)`
   * - Scalar multiplication
     - :math:`O(mn)`
     - :math:`O(1)`
   * - Transpose
     - :math:`O(mn)`
     - :math:`O(1)`
   * - Frobenius norm
     - :math:`O(mn)`
     - :math:`O(1)`
   * - Trace
     - :math:`O(\min(m, n))`
     - :math:`O(1)`
   * - Get / set
     - :math:`O(1)`
     - :math:`O(1)`

Error Handling
--------------

The API uses two error-reporting conventions, chosen by return type:

**Functions returning** ``int`` **(mutating operations)**:

- ``0`` on success
- ``-1`` on error (null pointer, dimension mismatch, singular matrix, etc.)

**Functions returning** ``double`` **(scalar queries)**:

- The computed value on success
- ``NaN`` on error (null pointer, invalid matrix, dimension mismatch, non-square
  input where square is required)

Callers should check with ``isnan()`` from ``<math.h>``.

**Functions returning** ``Matrix`` **(constructors)**:

- A valid ``Matrix`` on success
- A zeroed struct with ``.data = NULL`` on allocation failure

This keeps the API explicit and predictable in plain C code.
