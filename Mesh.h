#ifndef __MESH_H__
#define __MESH_H__

#include "Point.h"

#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

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
    const std::string &filename() const { return _filename; }
    size_t numVertices() const { return _vert.size(); }
    size_t numFaces() const { return _facestart.size() - 1; }
    Point center() const {
        size_t nV = numVertices();
        return Point(_sum.x / nV, _sum.y / nV, _sum.z / nV);
    }

    std::vector<Point> verts() const { return _vert; }
    PolyFace face(size_t idx) const { return PolyFace(idx, _facestart, _facevert); }

protected:
    void pushVertex(const Point &p) {
        _vert.push_back(p);
        _sum.add(p);
    }

    void pushFace(const std::vector<int> &vs) {
        int lastEnd = _facestart.back() + vs.size();
        _facestart.push_back(lastEnd);
        _facevert.insert(_facevert.end(), vs.begin(), vs.end());
        assert(_facevert.size() == _facestart.back());
    }
};

class PLYMesh : public Mesh {
    bool startsWith(const std::string &line, const std::string &prefix) {
        return 0 == line.compare(0, prefix.size(), prefix);
    }
public:
    PLYMesh(const std::string &filename) : Mesh(filename) {
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
        ss.exceptions(std::ios::failbit);
        for (int i = 0; i < nV; i++) {
            getline(f, line);
            ss.str(line);
            float x, y, z;
            ss >> x >> y >> z;
            ss.clear();
            pushVertex(Point(x, y, z));
        }
        for (int i = 0; i < nF; i++) {
            getline(f, line);
            ss.str(line);
            int n;
            ss >> n;
            if (n < 3)
                throw std::invalid_argument("Face has less than 3 vertices");
            std::vector<int> fs;
            for (int j = 0; j < n; j++) {
                int p;
                ss >> p;
                fs.push_back(p);
            }
            pushFace(fs);
            ss.clear();
        }
    }
};

class TriMesh {
    std::vector<Point> _v;
    std::vector<Face> _f;
public:
    const std::vector<Point> &vertsWithNormals() const {
        return _v;
    }
    const std::vector<Face> &faces() const {
        return _f;
    }
    TriMesh(const Mesh &m) : _v(m.verts()) {
        std::vector<Point> _n(_v.size(), Point(0, 0, 0));
        for (size_t i = 0; i < m.numFaces(); i++) {
            const PolyFace &pf = m.face(i);
            std::vector<int> face(pf.begin, pf.end);
            int p1 = face[0];
            for (size_t j = 1; j < face.size() - 1; j++) {
                int p2 = face[j];
                int p3 = face[j + 1];
                const Face &f = Face(p1, p2, p3);
                _f.push_back(f);
                const Point norm = f.normal(_v);
                _n[p1].add(norm);
                _n[p2].add(norm);
                _n[p3].add(norm);
            }
        }
        for (size_t i = 0; i < _n.size(); i++) {
            _n[i].normalize();
        }
        _v.insert(_v.end(), _n.begin(), _n.end());
    }
};

#endif
