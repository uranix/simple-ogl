#include "DooSabin.h"

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cassert>

int DooSabin::maxFaceOrder() {
    return 100;
}

std::vector<int> DooSabin::initOffsetTable() {
    std::vector<int> ret;
    ret.push_back(0); /* n = 0 */
    ret.push_back(0); /* n = 1 */
    for (int n = 1; n <= maxFaceOrder(); n++) {
        int last = ret.back();
        ret.push_back(last + n);
    }
    return ret;
}

std::vector<float> DooSabin::initCosTable() {
    std::vector<float> ret;
    const float pi = 4 * atan(1.f);
    for (int n = 1; n <= maxFaceOrder(); n++) {
        float alpha = 2 * pi / n;
        for (int ij = 0; ij < n; ij++)
            ret.push_back(cos(alpha * ij));
    }
    return ret;
}

float DooSabin::a(int n, int i, int j) {
    static std::vector<int> offs = initOffsetTable();
    static std::vector<float> cost = initCosTable();
    int ij = abs(i - j);
    if (n > maxFaceOrder())
        throw std::range_error("Please increase maxFaceOrder");
    float cv = cost[offs[n] + ij];
    return (!ij ? 0.25f : 0) + 0.25f * (3.f + 2.f * cv) / n;
}

struct NewPoint {
    int vertex;
    int oldface;
    NewPoint() { }
    NewPoint(int v, int f) : vertex(v), oldface(f) { }
};

DooSabin::DooSabin(const Mesh &m) : Mesh(m.filename() + "*") {
    std::vector<std::vector<int> > origEdges(m.numVertices());
    std::vector<Point> innerPoint(m.numVertices());
    std::vector<std::vector<int> > origFaces(m.numVertices());
    std::vector<bool> borderVertex(m.numVertices());
    std::vector<bool> orphanVertex(m.numVertices());

    for (size_t i = 0; i < m.numFaces(); i++) {
        PolyFace f = m.face(i);
        for (auto it = f.begin; it + 1 != f.end; it++) {
            origEdges[*it].push_back(*(it + 1));
            origEdges[*(it + 1)].push_back(*it);
        }
        origEdges[*(f.end - 1)].push_back(*f.begin);
        origEdges[*f.begin].push_back(*(f.end - 1));
        for (auto it = f.begin; it != f.end; it++)
            origFaces[*it].push_back(i);
    }

    for (size_t i = 0; i < origEdges.size(); i++) {
        std::vector<int> &v = origEdges[i];
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());

        std::vector<int> &w = origFaces[i];
        std::sort(w.begin(), w.end());
        w.erase(std::unique(w.begin(), w.end()), w.end());
    }

    for (size_t i = 0; i < origEdges.size(); i++) {
        borderVertex[i] = origEdges[i].size() != origFaces[i].size();
        orphanVertex[i] = origFaces[i].size() == 0;

        if (!borderVertex[i] && !orphanVertex[i]) {
            if (origFaces[i].size() < 3) {
                std::cerr << "Vertex " << i << " has less than 3 faces around and is not a boundary one" << std::endl;
                throw std::logic_error("Non-boundary vertex with less than 3 faces");
            }
            continue;
        }
        std::cerr << "Defect at vertex " << i
            << ", edges = " << origEdges[i].size()
            << ", faces = " << origFaces[i].size();
        int def = origEdges[i].size() - origFaces[i].size();
        if (def == 1)
            std::cout << " Looks like a face fan" << std::endl;
        else if (def == 0)
            std::cout << " Orphan vertex" << std::endl;
        else {
            std::cout << " No way to resolve" << std::endl;
            throw std::logic_error("Unrecoverable topology error in mesh");
        }
    }

    std::vector<std::vector<NewPoint> > newVertex(m.numVertices());
    /* Shrink old faces */
    for (size_t i = 0; i < m.numFaces(); i++) {
        PolyFace f = m.face(i);
        int n = f.end - f.begin;
        std::vector<Point> ps;
        for (auto j = f.begin; j < f.end; j++)
            ps.push_back(m.vert(*j));
        std::vector<int> fv;
        for (int j = 0; j < n; j++) {
            Point p(0, 0, 0);
            for (int k = 0; k < n; k++) {
                float al = a(n, j, k);
                p += al * ps[k];
            }
            int currentNewPoint = verts().size();
            fv.push_back(currentNewPoint);
            newVertex[*(f.begin + j)].push_back(NewPoint(currentNewPoint, i));
            pushVertex(p);
        }

        pushFace(fv);
    }

    /* New faces at old vertices */
    for (size_t i = 0; i < newVertex.size(); i++) {
        std::vector<NewPoint> &v = newVertex[i];
        std::vector<int> vFace;

        if (orphanVertex[i])
            continue;

        if (borderVertex[i]) {
            size_t jstarting = v.size();
            for (size_t j = 0; j < v.size(); j++) {
                PolyFace pf = m.face(v[j].oldface);
                int n = pf.end - pf.begin;
                int k = 1;
                for (auto it = pf.begin; it != pf.end; it++, k++)
                    if (*it == (int)i)
                        break;
                k %= n;
                int is = *(pf.begin + k);

                bool starting = true;

                for (size_t k = 0; k < v.size(); k++) {
                    if (k == j)
                        continue;
                    PolyFace pfs = m.face(v[k].oldface);
                    for (auto it = pfs.begin; it != pfs.end; it++)
                        if (*it == is)
                            starting = false;
                }

                if (starting) {
                    jstarting = j;
                    break;
                }
            }
            assert(jstarting != v.size());
            if (jstarting != 0)
                std::swap(v[0], v[jstarting]);
        }

        for (size_t j = 0; j < v.size(); j++) {
            PolyFace pf = m.face(v[j].oldface);
            int n = pf.end - pf.begin;
            int k = n - 1;
            for (auto it = pf.begin; it != pf.end; it++, k++)
                if (*it == (int)i)
                    break;
            k %= n;
            int is = *(pf.begin + k);
            size_t jnext = 0;
            for (size_t js = j + 1; js < v.size(); js++) {
                PolyFace pfs = m.face(v[js].oldface);
                bool found = false;
                for (auto it = pfs.begin; !found && it != pfs.end; it++)
                    if (*it == is)
                        found = true;
                if (found) {
                    jnext = js;
                    break;
                }
            }
            if (j + 1 != v.size()) {
                if (jnext == 0) {
                    std::cerr << "Vertex " << i << std::endl;
                    for (size_t z = 0; z < v.size(); z++) {
                        PolyFace pf = m.face(v[z].oldface);
                        std::cout << "Face " << v[z].oldface << ":";
                        for (auto x = pf.begin; x != pf.end; x++)
                            std::cout << " " << *x;
                        std::cout << std::endl;
                    }
                    throw std::logic_error("Faces do not form a fan around vertex");
                }
                if (jnext != j + 1)
                    std::swap(v[jnext], v[j + 1]);
            }
            vFace.push_back(v[j].vertex);
        }
        assert(!vFace.empty());
        if (!borderVertex[i])
            pushFace(vFace);
    }

    /* Faces at old edges */
    for (size_t i = 0; i < origEdges.size(); i++) {
        for (auto it = origEdges[i].begin(); it != origEdges[i].end(); it++) {
            size_t j = *it;
            if (j < i)
                continue;
            std::vector<NewPoint> cloud1 = newVertex[i];
            std::vector<NewPoint> cloud2 = newVertex[j];
            cloud1.push_back(cloud1.front());
            cloud2.push_back(cloud2.front());
            std::reverse(cloud2.begin(), cloud2.end());
/*
            std::cout << i << " -> " << j << std::endl;
            for (auto &z : cloud1)
                std::cout << z.vertex << "," << z.oldface << " ";
            std::cout << std::endl;
            for (auto &z : cloud2)
                std::cout << z.vertex << "," << z.oldface << " ";
            std::cout << std::endl;
*/
            bool found = false;
            size_t ii, jj;
            if (cloud1.size() < 3 || cloud2.size() < 3)
                continue;
            for (size_t i1 = 0; !found && i1 < cloud1.size() - 1; i1++)
                for (size_t i2 = 0; !found && i2 < cloud2.size() - 1; i2++)
                    if ((cloud1[i1    ].oldface == cloud2[i2    ].oldface) &&
                        (cloud1[i1 + 1].oldface == cloud2[i2 + 1].oldface))
                    {
                        ii = i1;
                        jj = i2;
                        found = true;
                    }
            if (found) {
                std::vector<int> quad;
                quad.push_back(cloud1[ii + 1].vertex);
                quad.push_back(cloud1[ii    ].vertex);
                quad.push_back(cloud2[jj    ].vertex);
                quad.push_back(cloud2[jj + 1].vertex);
                pushFace(quad);
            }
        }
    }
}
