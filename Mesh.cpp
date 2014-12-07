#include "Mesh.h"

#include "Point.h"

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

void Mesh::pushVertex(const Point &p) {
    _vert.push_back(p);
    _sum += p;
}

void Mesh::pushFace(const std::vector<int> &vs) {
    int lastEnd = _facestart.back() + vs.size();
    _facestart.push_back(lastEnd);
    _facevert.insert(_facevert.end(), vs.begin(), vs.end());
    assert(_facevert.size() == (size_t)_facestart.back());
}

PLYMesh::PLYMesh(const std::string &filename) : Mesh(filename) {
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

TriMesh::TriMesh(const Mesh &m) : _v(m.verts()) {
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
            _n[p1] += norm;
            _n[p2] += norm;
            _n[p3] += norm;
        }
    }
    for (size_t i = 0; i < _n.size(); i++) {
        _n[i].normalize();
    }
    _v.insert(_v.end(), _n.begin(), _n.end());
}

void Mesh::save(const std::string &fn) const {
    std::fstream f(fn, std::ios::out);
    if (!f)
        throw std::invalid_argument("Open file `" + fn + "' failed");

    f << "ply\n";
    f << "format ascii 1.0\n";
    f << "comment Mesh::save()\n";
    f << "element vertex " << numVertices() << std::endl;
    f << "property float x\n";
    f << "property float y\n";
    f << "property float z\n";
    f << "element face " << numFaces() << std::endl;
    f << "property list uchar int vertex_indices\n";
    f << "end_header\n";
    for (auto p = verts().begin(); p != verts().end(); p++)
        f << p->x << " " << p->y << " " << p->z << std::endl;
    for (size_t i = 0; i < numFaces(); i++) {
        PolyFace pf = face(i);
        int n = pf.end - pf.begin;
        f << n;
        for (int j = 0; j < n; j++)
            f << " " << *(pf.begin + j);
        f << std::endl;
    }
}
