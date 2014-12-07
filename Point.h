#ifndef __POINT_H__
#define __POINT_H__

#include <cmath>

struct Point {
    float x, y, z;
    Point() { }
    Point(float x, float y, float z) : x(x), y(y), z(z) { }
    Point(const Point &p, const Point &o) : x(p.x - o.x), y(p.y - o.y), z(p.z - o.z) { }
    void add(const Point &p) {
        x += p.x;
        y += p.y;
        z += p.z;
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

#endif
