#ifndef __AABB_H__
#define __AABB_H__

#include <limits>
#include <cassert>

struct AABB {
    float x1, y1, z1, x2, y2, z2;
    AABB() {
        x1 = std::numeric_limits<float>::max();
        y1 = std::numeric_limits<float>::max();
        z1 = std::numeric_limits<float>::max();
        x2 = std::numeric_limits<float>::lowest();
        y2 = std::numeric_limits<float>::lowest();
        z2 = std::numeric_limits<float>::lowest();
    }
    void add(float x, float y, float z) {
        if (x > x2) x2 = x;
        if (x < x1) x1 = x;
        if (y > y2) y2 = y;
        if (y < y1) y1 = y;
        if (z > z2) z2 = z;
        if (z < z1) z1 = z;
    }
    bool isInLeft(float x, float y, float z) const {
        float dx, dy, dz;
        dx = x2 - x1;
        dy = y2 - y1;
        dz = z2 - z1;
        if (dx >= dy && dx >= dz)
            return 2 * x < x1 + x2;
        if (dy >= dx && dy >= dz)
            return 2 * y < y1 + y2;
        if (dz >= dx && dz >= dy)
            return 2 * z < z1 + z2;
        assert(false);
        return false;
    }
    float radius() const {
        float dx, dy, dz;
        dx = x2 - x1;
        dy = y2 - y1;
        dz = z2 - z1;
        return (dx + dy + dz) / 2;
    }
    bool isEmpty() const {
        return x2 < x1;
    }
    void inflate(float d) {
        x1 -= d;
        y1 -= d;
        z1 -= d;
        x2 += d;
        y2 += d;
        z2 += d;
    }
    void writeVertex(float buf[]) {
        if (isEmpty()) {
            for (int j = 0; j < 8 * 3; j++)
                buf[j] = 0;
            return;
        }
        int j = 0;
        buf[j++] = x1; buf[j++] = y1; buf[j++] = z1;
        buf[j++] = x2; buf[j++] = y1; buf[j++] = z1;
        buf[j++] = x2; buf[j++] = y2; buf[j++] = z1;
        buf[j++] = x1; buf[j++] = y2; buf[j++] = z1;
        buf[j++] = x1; buf[j++] = y1; buf[j++] = z2;
        buf[j++] = x2; buf[j++] = y1; buf[j++] = z2;
        buf[j++] = x2; buf[j++] = y2; buf[j++] = z2;
        buf[j++] = x1; buf[j++] = y2; buf[j++] = z2;
    }
};


#endif
