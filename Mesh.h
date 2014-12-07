#ifndef __MESH_H__
#define __MESH_H__

#include "Point.h"

#include <string>
#include <vector>

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

struct PolyFace {
    std::vector<int>::const_iterator begin;
    std::vector<int>::const_iterator end;
    PolyFace(const size_t f, const std::vector<int> &fs, const std::vector<int> &fv) {
        begin = fv.begin() + fs[f];
        end   = fv.begin() + fs[f + 1];
    }
};

class Mesh {
    std::vector<Point> _vert;
    std::vector<int> _facestart;
    std::vector<int> _facevert;

    Point _sum;

    std::string _filename;
public:
    Mesh(const std::string &filename) : _sum(0, 0, 0), _filename(filename) {
        _facestart.push_back(0);
    }
    void save(const std::string &fn) const;
    std::string filename() const { return _filename; }
    size_t numVertices() const { return _vert.size(); }
    size_t numFaces() const { return _facestart.size() - 1; }
    Point center() const {
        size_t nV = numVertices();
        return Point(_sum.x / nV, _sum.y / nV, _sum.z / nV);
    }

    const std::vector<Point> &verts() const { return _vert; }
    const Point &vert(size_t idx) const { return verts()[idx]; }
    PolyFace face(size_t idx) const { return PolyFace(idx, _facestart, _facevert); }

protected:
    void pushVertex(const Point &p);
    void pushFace(const std::vector<int> &vs);
};

class PLYMesh : public Mesh {
    bool startsWith(const std::string &line, const std::string &prefix) {
        return 0 == line.compare(0, prefix.size(), prefix);
    }
public:
    PLYMesh(const std::string &filename);
};

class TriMesh {
    std::vector<Point> _v;
    std::vector<Face> _f;
public:
    const std::vector<Point> &vertsWithNormals() const { return _v; }
    const std::vector<Face> &faces() const { return _f; }
    TriMesh(const Mesh &m);
};

#endif
