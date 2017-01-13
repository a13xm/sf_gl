#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "vector.h"

template <typename T> T min_3(T a, T b, T c) {
    return std::min(std::min(a, b), c);
}

template <typename T> T max_3(T a, T b, T c) {
    return std::max(std::max(a, b), c);
}

inline int orientation_2d(Vec3i& a, Vec3i& b, Vec3i& c) {
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

inline float triangle_area(Vec3i a, Vec3i b, Vec3i c) {
    return std::abs(0.5f * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)));
}

inline bool is_top_left_edge(Vec3i va, Vec3i vb, Vec3i vc) { // test that va - vb is top or left edge
    return va.y > vb.y || (va.y == vb.y && va.y > vc.y);
}


#endif //__GEOMETRY_H__
