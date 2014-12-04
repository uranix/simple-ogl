#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <cstring>
#include <cassert>
#include <cmath>

class Matrix {
protected:
    constexpr static float degree = 0.017453292519943295769f;
    float m[4][4];
    Matrix() {
        memset(m, 0, 4 * 4 * sizeof(float));
    }
    void replaceWithProd(const Matrix &A, const Matrix &B) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                float sum = 0;
                for (int k = 0; k < 4; k++)
                    sum += A.m[i][k] * B.m[k][j];
                m[i][j] = sum;
            }
    }
public:
    Matrix &multWithRight(const Matrix &right) {
        /* this = this * right */
        Matrix left = *this;
        replaceWithProd(left, right);
        return *this;
    }
    Matrix &multWithLeft(const Matrix &left) {
        /* this = left * this */
        Matrix right = *this;
        replaceWithProd(left, right);
        return *this;
    }
    const float *data() const {
        return reinterpret_cast<const float *>(m);
    }
};

struct IdentityMatrix : public Matrix {
    IdentityMatrix() {
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
    }
};

struct Translate : public Matrix {
    Translate(float x, float y, float z) {
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
        m[0][3] = x;
        m[1][3] = y;
        m[2][3] = z;
    }
};

struct Scale : public Matrix {
    Scale(float s) : Scale(s, s, s) { }
    Scale(float sx, float sy, float sz) {
        m[0][0] = sx;
        m[1][1] = sy;
        m[2][2] = sz;
        m[3][3] = 1;
    }
};

struct PerspectiveMatrix : public Matrix {
public:
    PerspectiveMatrix(float zNear, float zFar, float vHalfAngleDegrees, float aspectRatio) {
        assert(zNear > 0 && zFar > zNear);
        float S = 1.f / tanf(vHalfAngleDegrees * degree);
        m[0][0] = S / aspectRatio;
        m[1][1] = S;
        m[2][2] = (zFar + zNear) / (zNear - zFar);
        m[2][3] = (2 * zFar * zNear) / (zNear - zFar);
        m[3][2] = -1;
    }
};

struct RotateMatrix : public Matrix {
public:
    RotateMatrix(float phid, float thetad) {
        float phi = phid * degree;
        float theta = thetad * degree;

        float cf = cos(phi);
        float sf = sin(phi);
        float ct = cos(theta);
        float st = sin(theta);

        m[0][0] = cf;
        m[0][2] = sf;
        m[1][0] = st * sf;
        m[1][1] = ct;
        m[1][2] = -cf * st;
        m[2][0] = -ct * sf;
        m[2][1] = st;
        m[2][2] = ct * cf;
        m[3][3] = 1;
    }
};

#endif
