#ifndef __DOOSABIN_H__
#define __DOOSABIN_H__

#include <vector>
#

#include "Mesh.h"

/*
 * Based on https://www.rose-hulman.edu/~finn/CCLI/Notes/day37.pdf
 * */

class DooSabin: public Mesh {
    static int maxFaceOrder();
    static std::vector<int> initOffsetTable();
    static std::vector<float> initCosTable();

    static float a(int n, int i, int j);
public:
    DooSabin(const Mesh &m);
};

#endif
