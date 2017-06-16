Matrices    {#mathfu_guide_matrices}
========

Matrices consist of a set of elements (usually floating point or integer
scalars) arranged in rows and columns.  For more information see
[this description](http://en.wikipedia.org/wiki/Matrix_(mathematics))
of matrices.

The [MathFu][] [Matrix][] class is a template declared in
[mathfu/matrix.h](@ref mathfu/matrix.h) which has been specialized and
optimized for the most regularly used case (floating point elements with
4 rows and 4 columns).
Implementing [Matrix][] as a template reduces code duplication, provides
compile time optimization opportunities through specialization and allows
users to use the class with any scalar type.

# Declaration  {#mathfu_guide_matrices_declaration}

The [Matrix][] class template takes either 2 or 3 arguments in the form:

    * Matrix<type, number_of_rows_and_columns>
    * Matrix<type, number_of_rows, number_of_columns>

For example, a floating point 4 row, 4 column (4x4) matrix can be declared
using either:

~~~{.cpp}
    mathfu::Matrix<float, 4, 4> matrix;
~~~

or:

~~~{.cpp}
    mathfu::Matrix<float, 4> matrix;
~~~

To eliminate the need for explicit template instantiation for common matrices,
[GLSL][] style `typedef`s are provided in
[mathfu/glsl_mappings.h](@ref mathfu/glsl_mappings.h).  Using a [GLSL][]
style `typedef` a 4x4 floating point matrix variable is declared
using the following:

~~~{.cpp}
    mathfu::mat4 matrix;
~~~

## Initialization  {#mathfu_guide_matrices_initialization}

For efficiency, [Matrix][] is uninitialized when constructed.  Constructors
are provided for common matrix sizes that allow initialization on construction.
For example, to initialize a 2x2 floating point matrix:

~~~{.cpp}
    mathfu::mat2 matrix(1.0f, 2.0f,  // column 0
                        3.0f, 4.0f); // column 1
~~~

It's also possible to initialize a [Matrix][] with another instance:

~~~{.cpp}
    const mathfu::mat2 matrix1(1.0f, 2.0f,  // column 0
                               3.0f, 4.0f); // column 1
    mathfu::mat2 matrix2(matrix1);
~~~

This can also be achieved with:

~~~{.cpp}
    const mathfu::mat2 matrix1(1.0f, 2.0f,  // column 0
                               3.0f, 4.0f); // column 1
    mathfu::mat2 matrix2 = matrix1;
~~~

# Accessors    {#mathfu_guide_matrices_accessors}

[Matrix][] provides an array operator to retrieve individual elements in a
flattened array and a parenthesis operator to access an element by row and
column index.

When using the array operator, the [Matrix][] is indexed by rows then columns.
For example, a 3x3 matrix will access the following locations:

Array Index | Column | Row
------------|--------|--------
0           | 0      | 0
1           | 0      | 1
2           | 0      | 2
3           | 1      | 0
4           | 1      | 1
5           | 1      | 2
6           | 2      | 0
7           | 2      | 1
8           | 2      | 2

For example, to access column 1, row 2 of a 3x3 matrix using the array
operator, use index "5":

~~~{.cpp}
    const mathfu::mat3 matrix(1.0f, 2.0f, 3.0f,   // column 0
                              4.0f, 5.0f, 6.0f,   // column 1
                              7.0f, 8.0f, 9.0f);  // column 2
    float column1_row2 = matrix[5]; // 6.0f
~~~

The parenthesis operator allows access to an element by specifying the row
and column indices directly.  For example, to access column 1, row 2 of a
3x3 matrix:

~~~{.cpp}
    const mathfu::mat3 matrix(1.0f, 2.0f, 3.0f,   // column 0
                              4.0f, 5.0f, 6.0f,   // column 1
                              7.0f, 8.0f, 9.0f);  // column 2
    float column1_row2 = matrix(2, 1);
~~~


## Assignment    {#mathfu_guide_matrices_assignment}

[Matrix][] array and parenthesis operators return a reference to an element
which can be modified by the caller.

For example, to update column 1, row 2 of a 3x3 matrix using the array
operator:

~~~{.cpp}
    mathfu::mat3 matrix(1.0f, 2.0f, 3.0f,   // column 0
                        4.0f, 5.0f, 0.0f,   // column 1
                        7.0f, 8.0f, 9.0f);  // column 2
    matrix[5] = 6.0f;
~~~

To update column 1, row 2 of a 3x3 matrix using the parenthesis operator:

~~~{.cpp}
    mathfu::mat3 matrix(1.0f, 2.0f, 3.0f,   // column 0
                        4.0f, 5.0f, 6.0f,   // column 1
                        7.0f, 0.0f, 9.0f);  // column 2
    matrix(2, 1) = 8.0f;
~~~

# Arithmetic    {#mathfu_guide_matrices_arithmetic}

[Matrix][] supports in-place and out-of-place addition and subtraction
with other matrices and scalars.  Multiplication and division of matrix
elements is supported with scalars.

For example, two matrices can be added together using the following:

~~~{.cpp}
    const mathfu::mat2 matrix1(1.0f, 2.0f,
                               3.0f, 4.0f);
    const mathfu::mat2 matrix2(5.0f, 6.0f,
                               7.0f, 8.0f);
    mathfu::mat2 matrix3 = matrix1 + matrix2;
~~~

The above preserves the contents of `matrix1` and `matrix2` with the following
in `matrix3`:

    6.0f,  8.0f
    10.0f, 12.0f

The same can be achieved with an in-place addition which mutates `matrix1`:

~~~{.cpp}
    mathfu::mat2 matrix1(1.0f, 2.0f,
                         3.0f, 4.0f);
    const mathfu::mat2 matrix2(5.0f, 6.0f,
                               7.0f, 8.0f);
    matrix1 += matrix2;
~~~

Addition with a scalar results in the value being added to all elements.
For example:

~~~{.cpp}
    mathfu::mat2 matrix1(1.0f, 2.0f,
                         3.0f, 4.0f);
    matrix1 += 1.0f;
~~~

where `matrix1` contains the following:

    2.0f, 3.0f,
    4.0f, 5.0f

Subtraction is similar to addition:

~~~{.cpp}
    const mathfu::mat2 matrix1(1.0f, 2.0f,
                               3.0f, 4.0f);
    const mathfu::mat2 matrix2(5.0f, 6.0f,
                               7.0f, 8.0f);
    mathfu::mat2 matrix3 = matrix2 - matrix1;
~~~

where `matrix3` contains the following:

    4.0f, 4.0f,
    4.0f, 4.0f

Subtraction with a scalar:

~~~{.cpp}
    mathfu::mat2 matrix1(1.0f, 2.0f,
                         3.0f, 4.0f);
    matrix1 -= 1.0f;
~~~

where `matrix1` contains the following:

    0.0f, 1.0f
    2.0f, 3.0f

Multiplication with a scalar:

~~~{.cpp}
    mathfu::mat2 matrix1(1.0f, 2.0f,
                         3.0f, 4.0f);
    matrix1 *= 2.0f;
~~~

where `matrix1` contains the following:

    2.0f, 4.0f
    6.0f, 8.0f

Division with a scalar:

~~~{.cpp}
    mathfu::mat2 matrix1(1.0f, 2.0f,
                         3.0f, 4.0f);
    matrix1 /= 2.0f;
~~~

where `matrix1` contains the following:

    0.5f, 1.0f
    1.5f, 2.0f

# Matrix Operations    {#mathfu_guide_matrices_matrix_ops}

Identity matrices are constructed using
[Matrix::Identity](@ref mathfu::Matrix::Identity):

~~~{.cpp}
    mathfu::mat2 identity = mathfu::mat2::Identity();
~~~

[Matrix][] supports matrix multiplication using the `* operator`.

Matrix multiplication is performed using the `* operator`:

~~~{.cpp}
    const mathfu::mat2 matrix1(1.0f, 2.0f,
                               3.0f, 4.0f);
    const mathfu::mat2 matrix2(5.0f, 6.0f,
                               7.0f, 8.0f);
    mathfu::mat2 matrix3 = matrix1 * matrix2;
~~~

which preserves `matrix1` and `matrix2` while storing the result in
`matrix3`:

    23.0f, 31.0f
    34.0f, 46.0f

The [inverse](http://en.wikipedia.org/wiki/Invertible_matrix) of a [Matrix][]
can be calculated using [Matrix::Inverse](@ref mathfu::Matrix::Inverse):

~~~{.cpp}
    const mathfu::mat2 matrix(1.0f, 2.0f,
                              3.0f, 4.0f);
    const mathfu::mat2 inverse = matrix.Inverse();
~~~

A [Matrix][] multiplied with its' inverse yields the identity matrix:

~~~{.cpp}
    const mathfu::mat2 matrix(1.0f, 2.0f,
                              3.0f, 4.0f);
    const mathfu::mat2 inverse = matrix.Inverse();
    const mathfu::mat2 identity = matrix * inverse;
~~~

[Matrix][] provides a set of static methods that construct
[transformation matrices][]:

   * [Matrix::FromTranslationVector()](@ref mathfu::Matrix::FromTranslationVector)
   * [Matrix::FromScaleVector()](@ref mathfu::Matrix::FromScaleVector)
   * [Matrix::RotationX()](@ref mathfu::Matrix::RotationX)
   * [Matrix::RotationY()](@ref mathfu::Matrix::RotationY)
   * [Matrix::RotationZ()](@ref mathfu::Matrix::RotationZ)

Transformation matrices yielded by these operations can be multiplied with
vector to translate, scale and rotate.  For example, to rotate a 3-dimensional
vector around the X axis by PI/2 radians (90 degrees):

~~~{.cpp}
    const mathfu::vec3 vector(1.0f, 2.0f, 3.0f);
    const mathfu::mat3 rotation_around_x(mathfu::mat3::RotationX(M_PI / 2.0f));
    const mathfu::vec3 rotated_vector = vector * rotation_around_x;
~~~

To scale a vector:

~~~{.cpp}
    const mathfu::vec3 vector(1.0f, 2.0f, 3.0f);
    const mathfu::mat4 scale_by_2(
      mathfu::mat4::FromScaleVector(mathfu::vec3(2.0f)));
    const mathfu::vec3 scaled_vector = scale_by_2 * vector;
~~~

In addition, a set of static methods are provided to construct
[camera matrices][]:

   * [Matrix::Perspective()](@ref mathfu::Matrix::Perspective)
   * [Matrix::Ortho()](@ref mathfu::Matrix::Ortho)
   * [Matrix::LookAt()](@ref mathfu::Matrix::LookAt)

For example, to construct a perspective matrix:

~~~{.cpp}
  const mathfu::mat4 perspective_matrix_ = mathfu::mat4::Perspective(
      1.0f, 16.0f / 9.0f, 1.0f, 100.0f, -1.0f);
~~~

# Packing    {#mathfu_guide_matrices_packing}

The size of the class can change based upon the [Build Configuration][] so it
should *not* be treated like a C style array.  To serialize the class to a
flat array see [VectorPacked][].

For example, to pack (store) a [Matrix] to an array of packed vectors:

~~~{.cpp}
    mathfu::mat4 matrix(1.0f);
    mathfu::vec4_packed packed[4];
    matrix.Pack(packed);
~~~

To unpack an array of packed vectors:

~~~{.cpp}
    mathfu::vec4_packed packed[4];
    mathfu::mat4 matrix(packed);
~~~

<br>

  [Build Configuration]: @ref mathfu_build_config
  [GLSL]: http://www.opengl.org/documentation/glsl/
  [MathFu]: @ref mathfu_overview
  [Matrix]: @ref mathfu::Matrix
  [camera matrices]: http://en.wikipedia.org/wiki/3D_projection
  [transformation matrices]: http://en.wikipedia.org/wiki/Transformation_matrix
  [transformation matrix]: http://en.wikipedia.org/wiki/Transformation_matrix
  [VectorPacked]: @ref mathfu::VectorPacked
