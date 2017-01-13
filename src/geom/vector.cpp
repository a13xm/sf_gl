#include "matrix.h"

#include "vector.h"

template <>
template <>
Vec3<int>::Vec3
(const Vec3<float> &v) 
: x(int(v.x+.5)), y(int(v.y+.5)), z(int(v.z+.5)) {
}

template <> template <>
Vec3<float>::Vec3
(const Vec3<int> &v) 
: x(v.x), y(v.y), z(v.z) {
}



template <> Vec3<float>::Vec3(Matrix m) : x(m[0][0]/m[3][0]), y(m[1][0]/m[3][0]), z(m[2][0]/m[3][0]) {}
