Quaternions    {#mathfu_guide_quaternions}
===========

[Quaternions] provide a representation of a 3-dimensional orientation or
rotation.  [Quaternions] are especially useful when interpolating between
angles to avoid [Gimbal lock][].  For more information, see [this description].

[MathFu] implements a quaternion as the [Quaternion][] class template which is
constructed from one 3-dimensional [Vector][] and scalar component.  The
[Quaternion][] template is intended to be instanced with `float` or `double`
floating point scalar types.

# Declaration  {#mathfu_guide_quaternions_declaration}

The [Quaternions][] template takes a single argument which specifies the
floating point type of each element in the class:

~~~{.cpp}
    mathfu::Quaternion<float> quaternion;
~~~

## Initialization  {#mathfu_guide_quaternions_initialization}

For efficiency, [Quaternion][] is uninitialized when constructed.  Constructors
are provided to initialize the class from a set of scalars, a 3-dimensional
[Vector][] and a scalar or another [Quaternion][].

For example, to initialize a [Quaternion][] using a set of scalars:

~~~{.cpp}
    mathfu::Quaternion<float> quaternion(0.71f, 0.0f, 0.71f, 0.0f);
~~~

To initialize a [Quaternion][] from a scalar and 3-dimensional vector
component:

~~~{.cpp}
    mathfu::Quaternion<float> quaternion(
        0.50f, mathfu::vec3(0.76f, 0.38f, 0.19f));
~~~

# Accessors    {#mathfu_guide_quaternions_accessors}

[Quaternion][] provides an array operator to retrieve elements of the object.
The first index is the scalar component with the remaining 3 indices referring
to elements in the vector component.

~~~{.cpp}
    const mathfu::Quaternion<float> quaternion(
        0.50f, mathfu::vec3(0.76f, 0.38f, 0.19f));
    float s = quaternion[0];
    float x = quaternion[1];
    float y = quaternion[2];
    float z = quaternion[3];
~~~

## Assignment    {#mathfu_guide_quaternions_assignment}

The array operator returns a reference to an element in the [Quaternion][]
which can be updated:

~~~{.cpp}
    mathfu::Quaternion<float> quaternion;
    quaternion[0] = 0.5f;
    quaternion[1] = 0.76f;
    quaternion[2] = 0.38f;
    quaternion[3] = 0.19f;
~~~

# Converting Between Representations {#mathfu_guide_quaternions_repr}

[Quaternion][] provides a number of ways to create a rotation or orientation:

   * [Quaternion::FromAngleAxis()](@ref mathfu::Quaternion::FromAngleAxis)
   * [Quaternion::FromEulerAngles()](@ref mathfu::Quaternion::FromEulerAngles)
   * [Quaternion::FromMatrix()](@ref mathfu::Quaternion::FromMatrix)

For example, to create a [Quaternion][] which rotates PI / 2 radians around
the X-axis:

~~~{.cpp}
    mathfu::Quaternion<float> quaternion =
        mathfu::Quaternion<float>::FromAngleAxis(
            M_PI / 2.0f, mathfu::vec3(1.0f, 0.0f, 0.0f));
~~~

The rotation of PI / 2 radians around the X-axis can also be achieved with:

~~~{.cpp}
    mathfu::Quaternion<float> quaternion =
        mathfu::Quaternion<float>::FromEulerAngles(
            mathfu::vec3(M_PI / 2.0f, 0.0f, 0.0f));
~~~

Similarly, a quaternion can be constructed from an existing rotation matrix
using:

~~~{.cpp}
    // Assuming rotation_matrix is an existing matrix.
    mathfu::Quaternion<float> quaternion =
        mathfu::Quaternion<float>::FromMatrix(rotation_matrix);
~~~

In addition, methods are provided to calculate an angle / axis pair,
Euler angles, or rotation matrix from a [Quaternion][]:

   * [Quaternion::ToAngleAxis()](@ref mathfu::Quaternion::ToAngleAxis)
   * [Quaternion::ToEulerAngles()](@ref mathfu::Quaternion::ToEulerAngles)
   * [Quaternion::ToMatrix()](@ref mathfu::Quaternion::ToMatrix)

For example, to calculate the angle and axis from a [Quaternion][]:

~~~{.cpp}
    // Assuming "quaternion" is an existing float quaternion.
    float angle;
    mathfu::vec3 axis;
    quaternion.ToAngleAxis(&angle, &axis);
~~~

To calculate a vector of Euler angles from a [Quaternion][]

~~~{.cpp}
    // Assuming "quaternion" is an existing float quaternion.
    mathfu::vec3 euler_angles = quaternion.ToEulerAngles();
~~~

Finally, to convert a [Quaternion][] to a rotation matrix:

~~~{.cpp}
    // Assuming "quaternion" is an existing float quaternion.
    mathfu::Matrix<float, 3> rotation_matrix = quaternion.ToMatrix();
~~~

# Manipulation    {#mathfu_guide_quaternions_manipulation}

[Quaternion][] objects can be multiplied with each other to combine their
rotations.

For example, if a quaternion that rotates by PI / 2 radians around
the X axis is multiplied by itself it will result in a quaternion that rotates
by PI radians around the X axis:

~~~{.cpp}
    mathfu::Quaternion<float> quaternion_pi_by_2_around_x =
        mathfu::Quaternion<float>::FromAngleAxis(
            M_PI / 2.0f, mathfu::vec3(1.0f, 0.0f, 0.0f));
    mathfu::Quaternion<float> quaternion_pi_around_x =
        quaternion_pi_by_2_around_x * quaternion_pi_by_2_around_x;
~~~

It's also possible to multiply a [Quaternion][] with a scalar to scale the
existing rotation expressed by the object.

For example, if a quaternion that rotates by PI / 2 radians around
the X axis is multiplied by 2 it will result in a quaternion that rotates
by PI radians around the X axis:

~~~{.cpp}
    mathfu::Quaternion<float> quaternion_pi_by_2_around_x =
        mathfu::Quaternion<float>::FromAngleAxis(
            M_PI / 2.0f, mathfu::vec3(1.0f, 0.0f, 0.0f));
    mathfu::Quaternion<float> quaternion_pi_around_x =
        quaternion_pi_by_2_around_x * 2.0f;
~~~

In addition, [Quaternion][] provides the ability to perform
[spherical linear interpolation][] between two [Quaternion][] objects enabling
smooth transitions between rotations while avoiding [Gimbal lock][].

For example, the rotation half way between two quaternions can be calculated
by the following:

~~~{.cpp}
    // Assuming "quaternion1" and "quaternion2" represent arbitrary rotations.
    mathfu::Quaternion<float> quaternion3 =
        mathfu::Quaternion<float>(quaternion1, quaternion2, 0.5f);
~~~

Finally, the inverse (opposite rotation) of a [Quaternion][] is calculated
using [Quaternion::Inverse()](@ref mathfu::Quaternion::Inverse).  For example,
if a [Quaternion][] represents a rotation PI / 2 radians around the X axis
the rotation -PI / 2 degrees around the X axis can be calculated by:

~~~{.cpp}
    const mathfu::Quaternion<float> quaternion =
        mathfu::Quaternion<float>::FromAngleAxis(
            M_PI / 2.0f, mathfu::vec3(1.0f, 0.0f, 0.0f));
    mathfu::Quaternion<float> inverse_rotation = quaternion.Inverse();
~~~

# Application    {#mathfu_guide_quaternions_application}

A rotation expressed by a [Quaternion][] is applied to a [Vector][] by
multiplying the [Quaternion][] with the [Vector][].

For example, to rotate a [Vector][] by PI / 2 radians around the X axis:

~~~{.cpp}
    const mathfu::vec3 vector(0.0f, 1.0f, 0.0f);
    const mathfu::Quaternion<float> quaternion =
        mathfu::Quaternion<float>::FromAngleAxis(
            M_PI / 2.0f, mathfu::vec3(1.0f, 0.0f, 0.0f));
    mathfu::vec3 rotated_vector = quaternion * vector;
~~~

When  applying rotations to a large number of vectors or points
(e.g transforming a set of vertices) it is easier to use
[Quaternion::ToMatrix()](@ref mathfu::Quaternion::ToMatrix) to calculate
the rotation matrix and apply the same transform to the set of vectors.

<br>

  [Quaternions]: http://en.wikipedia.org/wiki/Quaternion
  [this description]: http://en.wikipedia.org/wiki/Quaternion
  [Gimbal lock]: http://en.wikipedia.org/wiki/Gimbal_lock
  [spherical linear interpolation]: http://en.wikipedia.org/wiki/Slerp
  [Quaternion]: @ref mathfu::Quaternion
  [MathFu]: @ref mathfu_overview
  [Matrix]: @ref mathfu::Matrix
  [Vector]: @ref mathfu::Vector
