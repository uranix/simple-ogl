#ifndef __MESH_H__
#define __MESH_H__

#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>

struct Point {
    float x, y, z;
    Point() { }
    Point(float x, float y, float z) : x(x), y(y), z(z) { }
};

struct Face {
    int v1, v2, v3;
    Face() { }
    Face(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) { }
};

class Mesh {
    bool startsWith(const std::string &line, const std::string &prefix) {
        return 0 == line.compare(0, prefix.size(), prefix);
    }

    std::vector<Point> _v;
    std::vector<Face> _f;
public:
    const std::vector<Point> &verts() const {
        return _v;
    }
    const std::vector<Face> &faces() const {
        return _f;
    }
    Mesh(const std::string &filename) {
        std::string line;
        std::fstream f(filename, std::ios::in);
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
        for (int i = 0; i < nF; i++) {
            getline(f, line);
            ss.str(line);
            int n, p1, p2, p3;
            ss >> n >> p1 >> p2 >> p3;
            if (n != 3)
                throw std::invalid_argument("Non-triangle face encountered");
            _f.push_back(Face(p1, p2, p3));
        }
    }
};

#endif
