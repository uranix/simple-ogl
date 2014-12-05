#ifndef __MESH_H__
#define __MESH_H__

#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

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
};

struct Face {
    int v1, v2, v3;
    Face() { }
    Face(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) { }
    Point normal(const std::vector<Point> &ps) const {
        const Point &p1 = ps[v1];
        const Point &p2 = ps[v2];
        const Point &p3 = ps[v3];
        /* p2 - p1 x p3 - p1 */
        float nx = -p2.z*p3.y + p1.z*(-p2.y + p3.y) + p1.y*(p2.z - p3.z) + p2.y*p3.z;
        float ny = p1.z*(p2.x - p3.x) + p2.z*p3.x - p2.x*p3.z + p1.x*(-p2.z + p3.z);
        float nz = -p2.y*p3.x + p1.y*(-p2.x + p3.x) + p1.x*(p2.y - p3.y) + p2.x*p3.y;
        return Point(nx, ny, nz);
    }
};

class Mesh {
    bool startsWith(const std::string &line, const std::string &prefix) {
        return 0 == line.compare(0, prefix.size(), prefix);
    }

    std::vector<Point> _v;
    std::vector<Face> _f;
public:
    const std::vector<Point> &vertsWithNormals() const {
        return _v;
    }
    const std::vector<Face> &faces() const {
        return _f;
    }
    Mesh(const std::string &filename) {
        std::string line;
        std::fstream f(filename, std::ios::in);
        if (!f)
            throw std::invalid_argument("Could not read mesh file");
        getline(f, line);
        if (!startsWith(line, "ply"))
            throw std::invalid_argument("Invalid PLY header");
        getline(f, line);
        if (!startsWith(line, "format ascii 1.0"))
            throw std::invalid_argument("Only `format ascii 1.0' meshes are supported");
        int nV, nF;
        nV = nF = -1;
        while (getline(f, line)) {
            if (startsWith(line, "element")) {
                std::stringstream ss(line);
                std::string elem, type, count;
                ss >> elem >> type >> count;
                if (type == "vertex")
                    nV = atoi(count.c_str());
                else if (type == "face")
                    nF = atoi(count.c_str());
                else
                    throw std::invalid_argument(std::string("Unknown element type `") + type + "'");
            }
            /* TODO: parse properties properly */
            if (startsWith(line, "end_header"))
                break;
        }
        if (nV < 0 || nF < 0)
            throw std::invalid_argument("No vertex or face element in mesh");
        std::stringstream ss;
        for (int i = 0; i < nV; i++) {
            getline(f, line);
            ss.str(line);
            float x, y, z;
            ss >> x >> y >> z;
            _v.push_back(Point(x, y, z));
        }
        std::vector<Point> _n(nV, Point(0, 0, 0));
        for (int i = 0; i < nF; i++) {
            getline(f, line);
            ss.str(line);
            int n, p1, p2, p3;
            ss >> n >> p1 >> p2 >> p3;
            if (n != 3)
                throw std::invalid_argument("Non-triangle face encountered");
            const Face ff(p1, p2, p3);
            _f.push_back(ff);
            const Point norm = ff.normal(_v);
            _n[p1].add(norm);
            _n[p2].add(norm);
            _n[p3].add(norm);
        }
        _v.insert(_v.end(), _n.begin(), _n.end());
    }
};

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
    void add(const Point &p) {
        if (p.x > x2) x2 = p.x;
        if (p.x < x1) x1 = p.x;
        if (p.y > y2) y2 = p.y;
        if (p.y < y1) y1 = p.y;
        if (p.z > z2) z2 = p.z;
        if (p.z < z1) z1 = p.z;
    }
    bool isInLeft(const Point &pp, const Point &c) const {
        const Point p(pp, c);
        float dx, dy, dz;
        dx = x2 - x1;
        dy = y2 - y1;
        dz = z2 - z1;
        if (dx >= dy && dx >= dz)
            return 2 * p.x < x1 + x2;
        if (dy >= dx && dy >= dz)
            return 2 * p.y < y1 + y2;
        if (dz >= dx && dz >= dy)
            return 2 * p.z < z1 + z2;
        assert(false);
        return false;
    }
    bool hasOnLeft (const Face &f, const std::vector<Point> &ps, const Point &c) {
        return isInLeft(ps[f.v1], c) ||
               isInLeft(ps[f.v2], c) ||
               isInLeft(ps[f.v3], c);
    }
    bool hasOnRight(const Face &f, const std::vector<Point> &ps, const Point &c) {
        return !isInLeft(ps[f.v1], c) ||
               !isInLeft(ps[f.v2], c) ||
               !isInLeft(ps[f.v3], c);
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
