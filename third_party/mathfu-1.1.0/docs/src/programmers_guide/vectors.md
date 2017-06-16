Vectors    {#mathfu_guide_vectors}
=======

Vectors consist of a set of elements (usually floating point or integer
scalars) that are regularly used to describe a point in space or a direction.
For more information see this description of
[Euclidiean Vectors](http://en.wikipedia.org/wiki/Euclidean_vector).

The [MathFu][] [Vector][] class is a template declared in
[mathfu/vector.h](@ref mathfu/vector.h) which has been specialized and
optimized for regularly used cases.  Implementing [Vector][] as a template
reduces code duplication, provides compile time optimization opportunities
through specialization and allows users to use the class with any scalar
type.

# Declaration  {#mathfu_guide_vectors_declaration}

[Vector][] template takes two arguments: the type and number of elements in
the [Vector][].

For example, a 2-dimensional floating point vector variable is declared using
the following:

~~~{.cpp}
    mathfu::Vector<float, 2> vector;
~~~

To eliminate the need for explicit template instantiation, [GLSL][] style
`typedef`s are provided in
[mathfu/glsl_mappings.h](@ref mathfu/glsl_mappings.h).  Using a [GLSL][]
style `typedef` a 2-dimensional floating point vector variable is declared
using the following:

~~~{.cpp}
    math::vec2 vector;
~~~

## Initialization  {#mathfu_guide_vectors_initialization}

For efficiency, [Vector][] is uninitialized when constructed.  Constructors
are provided for common vector sizes that allow initialization on construction:

~~~{.cpp}
    mathfu::vec2 vector(1.0f, 2.0f);
~~~

It's also possible to initialize a [Vector][] with another instance:

~~~{.cpp}
    mathfu::vec2 vector1(1.0f, 2.0f);
    mathfu::vec2 vector2(vector1);
~~~

This can also be achieved with:

~~~{.cpp}
    mathfu::vec2 vector1(1.0f, 2.0f);
    mathfu::vec2 vector2 = vector1;
~~~

# Accessors    {#mathfu_guide_vectors_accessors}

[Vector][] provides array and [GLSL][] style accessors.  For example, to
read two elements from a 2-dimensional vector using array accessors:

~~~{.cpp}
    const mathfu::vec2 vector(1.0f, 2.0f);
    float x = vector[0];
    float y = vector[1];
~~~

It's also possible to read elements from 2, 3 and 4-dimensional vectors using
[GLSL][] style accessors:

~~~{.cpp}
    const mathfu::vec4 vector(1.0f, 2.0f, 3.0f, 4.0f);
    float x = vector.x();
    float y = vector.y();
    float z = vector.z();
    float w = vector.w();
~~~

Similar to [GLSL][], [Vector][] provides accessors which allow a subset of
elements to be accessed:

~~~{.cpp}
    const mathfu::vec3 vector1(1.0f, 2.0f, 3.0f);
    mathfu::vec3 xy = vector1.xy();
~~~

## Assignment    {#mathfu_guide_vectors_assignment}

Individual elements returned by [Vector][]'s array accessors are references
that can be assigned values to update the contents of the class:

~~~{.cpp}
    mathfu::vec2 vector(1.0f, 2.0f);
    vector[0] = 3.0f;
    vector[1] = 4.0f;
~~~

# Arithmetic    {#mathfu_guide_vectors_arithmetic}

[Vector][] supports in-place and out-of-place arithmetic operators
(addition, subtraction, multiplication, division) that perform
component-wise operations.

For example, two vectors can be added together using the following:

~~~{.cpp}
    const mathfu::vec2 vector1(1.0f, 2.0f), vector2(3.0f, 4.0f);
    mathfu::vec2 vector3 = vector1 + vector2;
~~~

The above results in the values `(4.0f, 6.0f)` stored in `vector3` while
preserving the values of `vector1` and `vector2`.

The same can be achieved with an in-place addition which mutates `vector1`:

~~~{.cpp}
    mathfu::vec2 vector1(1.0f, 2.0f);
    const mathfu::vec2 vector2(3.0f, 4.0f);
    vector1 += vector2;
~~~

Subtraction is similar to addition:

~~~{.cpp}
    const mathfu::vec2 vector1(4.0f, 6.0f), vector2(3.0f, 4.0f);
    mathfu::vec2 vector3 = vector2 - vector1;
~~~

Multiplication is performed component-wise, which means that each component
is multiplied with the same index component in the other [Vector][] involved
in the operation:

~~~{.cpp}
    const mathfu::vec2 vector1(2.0f, 0.5f), vector2(3.0f, 10.0f);
    vector3 = vector1 * vector2;
~~~

The above results in the values `(6.0f, 5.0f)` stored in `vector3` while
preserving the values of `vector1` and `vector2`.

Similar to the other operators, multiplication can be performed in place:

~~~{.cpp}
    mathfu::vec2 vector1(2.0f, 0.5f);
    const mathfu::vec2 vector2(3.0f, 10.0f);
    vector1 *= vector2;
~~~

Division is also a component-wise operation:

~~~{.cpp}
    const mathfu::vec2 vector1(4.0f, 4.0f), vector2(2.0f, 4.0f);
    vector3 = vector1 / vector2;
~~~

The above results in the values `(2.0f, 1.0f)` stored in `vector3` while
preserving the values of `vector1` and `vector2`.

# Constants    {#mathfu_guide_vectors_constants}

Commonly used constants are provided by
[mathfu/constants.h](@ref mathfu/constants.h).  These values eliminate the need
to construct [Vector][] objects for common values like cardinal axes.

For example, the following initializes a 2-dimensional vector with the X-axis:

~~~{.cpp}
    const mathfu::vec2 vector = mathfu::kAxisX2f;
~~~

# Geometric Operations    {#mathfu_guide_vectors_geometric}

[Vector][] also provides functions for commonly used geometric operations
that are utilized by graphics and games developers.

For example, the length of a [Vector][] is calculated using
[Length()](@ref mathfu::Vector::Length):

~~~{.cpp}
    const mathfu::vec2 vector(3.0f, 4.0f);
    float length = vector.Length();
~~~

The projection of one [Vector][] onto another (dot product) can be calculated
using [DotProduct()](@ref mathfu::Vector::DotProduct).  For example, the
following calculates the projection of a vector onto the X-axis:

~~~{.cpp}
  float projection = mathfu::vec2::DotProduct(mathfu::vec2(5.0f, 2.0f),
                                              mathfu::kAxisX2f);
~~~

It's possible to normalize (scale to a length of 1) a vector in-place using
[Normalize()](@ref mathfu::Vector::Normalize) or out-of-place using
[Normalized()](@ref mathfu::Vector::Normalized).  For example, the following
normalizes the vector in-place:

~~~{.cpp}
    mathfu::vec2 vector(3.0f, 4.0f);
    vector.Normalize();
~~~

The cross product of two 3-dimensional [Vectors][] (the vector perpendicular
to two vectors) can be calculated using
[CrossProduct()](@ref mathfu::Vector::CrossProduct), for example:

~~~{.cpp}
    mathfu::vec3 zaxis = mathfu::vec3::CrossProduct(mathfu::kAxisX3f,
                                                    mathfu::kAxisY3f);
~~~

Alternatively, to create three points and compute the normal of the plane
defined by the points use:

~~~{.cpp}
    mathfu::vec3 point1(0.5f, 0.4f, 0.1f);
    mathfu::vec3 point2(0.4f, 0.9f, 0.1f);
    mathfu::vec3 point3(0.1f, 0.8f, 0.6f);

    mathfu::vec3 vector1 = point2 - point1;
    mathfu::vec3 vector2 = point3 - point1;

    mathfu::vec3 normal = Vector<float, 3>::CrossProduct(vector2, vector1);
~~~

# Other Operations    {#mathfu_guide_vectors_other}

In addition, to basic arithmetic and geometric operations, [Vector][] also
implements functions to perform the following:

   * [Lerp](@ref mathfu::Vector::Lerp) to linearly interpolate between two
     vectors.
   * [RandomInRange](@ref mathfu::Vector::RandomInRange) to generate a vector
     with random value for elements.

# Packing    {#mathfu_guide_vectors_packing}

The size of the class can change based upon the [Build Configuration][] so it
should *not* be treated like a C style array.  To serialize the class to a
flat array see [VectorPacked](@ref mathfu::VectorPacked).

For example, to pack (store) an unpacked to packed vector:

~~~{.cpp}
    mathfu::vec3 vector(3.0f, 2.0f, 1.0f);
    mathfu::vec3_packed packed = vector;
~~~

Since [VectorPacked][] is plain-old-data (POD) it can be cast to an array
of elements of the same type used by the [Vector][] so it's possible to use
an array of [VectorPacked][] data structures to contiguous arrays of data like
vertex buffers.

Similarly, [VectorPacked][] can be used to deserialize (load) data into
[Vector][]:

~~~{.cpp}
    VectorPacked<float, 3> packed = { 3, 2, 1 };
    Vector<float, 3> vector(packed);
~~~

<br>

  [Build Configuration]: @ref mathfu_build_config
  [MathFu]: @ref mathfu_overview
  [GLSL]: http://www.opengl.org/documentation/glsl/
  [Vector]: @ref mathfu::Vector
  [VectorPacked]: @ref mathfu::VectorPacked
