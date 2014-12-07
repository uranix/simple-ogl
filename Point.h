#ifndef __POINT_H__
#define __POINT_H__

#include <cmath>
#include <ostream>

struct Point {
    float x, y, z;
    Point() { }
    Point(float x, float y, float z) : x(x), y(y), z(z) { }
    Point(const Point &p, const Point &o) : x(p.x - o.x), y(p.y - o.y), z(p.z - o.z) { }
    Point operator+=(const Point &p) {
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }
    Point operator-=(const Point &p) {
        x -= p.x;
        y -= p.y;
        z -= p.z;
        return *this;
    }
    Point operator-() const {
        return Point(-x, -y, -z);
    }
    void normalize() {
        float r = sqrt(x * x + y * y + z * z);
        x /= r;
        y /= r;
        z /= r;
    }
};

inline Point operator*(float m, const Point &p) {
    return Point(m * p.x, m * p.y, m * p.z);
}

inline std::ostream &operator<<(std::ostream &o, const Point &p) {
    return o << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}

#endif
