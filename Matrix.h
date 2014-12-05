#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <cstring>
#include <cassert>
#include <cmath>
#include <algorithm>

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
    void inverse() {
        Matrix a(*this);

        float det =
            a.m[0][3]*a.m[1][2]*a.m[2][1]*a.m[3][0] - a.m[0][2]*a.m[1][3]*a.m[2][1]*a.m[3][0] -
            a.m[0][3]*a.m[1][1]*a.m[2][2]*a.m[3][0] + a.m[0][1]*a.m[1][3]*a.m[2][2]*a.m[3][0] +
            a.m[0][2]*a.m[1][1]*a.m[2][3]*a.m[3][0] - a.m[0][1]*a.m[1][2]*a.m[2][3]*a.m[3][0] -
            a.m[0][3]*a.m[1][2]*a.m[2][0]*a.m[3][1] + a.m[0][2]*a.m[1][3]*a.m[2][0]*a.m[3][1] +
            a.m[0][3]*a.m[1][0]*a.m[2][2]*a.m[3][1] - a.m[0][0]*a.m[1][3]*a.m[2][2]*a.m[3][1] -
            a.m[0][2]*a.m[1][0]*a.m[2][3]*a.m[3][1] + a.m[0][0]*a.m[1][2]*a.m[2][3]*a.m[3][1] +
            a.m[0][3]*a.m[1][1]*a.m[2][0]*a.m[3][2] - a.m[0][1]*a.m[1][3]*a.m[2][0]*a.m[3][2] -
            a.m[0][3]*a.m[1][0]*a.m[2][1]*a.m[3][2] + a.m[0][0]*a.m[1][3]*a.m[2][1]*a.m[3][2] +
            a.m[0][1]*a.m[1][0]*a.m[2][3]*a.m[3][2] - a.m[0][0]*a.m[1][1]*a.m[2][3]*a.m[3][2] -
            a.m[0][2]*a.m[1][1]*a.m[2][0]*a.m[3][3] + a.m[0][1]*a.m[1][2]*a.m[2][0]*a.m[3][3] +
            a.m[0][2]*a.m[1][0]*a.m[2][1]*a.m[3][3] - a.m[0][0]*a.m[1][2]*a.m[2][1]*a.m[3][3] -
            a.m[0][1]*a.m[1][0]*a.m[2][2]*a.m[3][3] + a.m[0][0]*a.m[1][1]*a.m[2][2]*a.m[3][3];

        float idet = 1 / det;

        m[0][0] = idet * (a.m[1][3]*(-(a.m[2][2]*a.m[3][1]) + a.m[2][1]*a.m[3][2]) + a.m[1][2]*(a.m[2][3]*a.m[3][1] -
                a.m[2][1]*a.m[3][3]) + a.m[1][1]*(-(a.m[2][3]*a.m[3][2]) + a.m[2][2]*a.m[3][3]));
        m[0][1] = idet * (a.m[0][3]*(a.m[2][2]*a.m[3][1] - a.m[2][1]*a.m[3][2]) + a.m[0][2]*(-(a.m[2][3]*a.m[3][1]) +
                a.m[2][1]*a.m[3][3]) + a.m[0][1]*(a.m[2][3]*a.m[3][2] - a.m[2][2]*a.m[3][3]));
        m[0][2] = idet * (a.m[0][3]*(-(a.m[1][2]*a.m[3][1]) + a.m[1][1]*a.m[3][2]) + a.m[0][2]*(a.m[1][3]*a.m[3][1] -
                a.m[1][1]*a.m[3][3]) + a.m[0][1]*(-(a.m[1][3]*a.m[3][2]) + a.m[1][2]*a.m[3][3]));
        m[0][3] = idet * (a.m[0][3]*(a.m[1][2]*a.m[2][1] - a.m[1][1]*a.m[2][2]) + a.m[0][2]*(-(a.m[1][3]*a.m[2][1]) +
                a.m[1][1]*a.m[2][3]) + a.m[0][1]*(a.m[1][3]*a.m[2][2] - a.m[1][2]*a.m[2][3]));
        m[1][0] = idet * (a.m[1][3]*(a.m[2][2]*a.m[3][0] - a.m[2][0]*a.m[3][2]) + a.m[1][2]*(-(a.m[2][3]*a.m[3][0]) +
                a.m[2][0]*a.m[3][3]) + a.m[1][0]*(a.m[2][3]*a.m[3][2] - a.m[2][2]*a.m[3][3]));
        m[1][1] = idet * (a.m[0][3]*(-(a.m[2][2]*a.m[3][0]) + a.m[2][0]*a.m[3][2]) + a.m[0][2]*(a.m[2][3]*a.m[3][0] -
                a.m[2][0]*a.m[3][3]) + a.m[0][0]*(-(a.m[2][3]*a.m[3][2]) + a.m[2][2]*a.m[3][3]));
        m[1][2] = idet * (a.m[0][3]*(a.m[1][2]*a.m[3][0] - a.m[1][0]*a.m[3][2]) + a.m[0][2]*(-(a.m[1][3]*a.m[3][0]) +
                a.m[1][0]*a.m[3][3]) + a.m[0][0]*(a.m[1][3]*a.m[3][2] - a.m[1][2]*a.m[3][3]));
        m[1][3] = idet * (a.m[0][3]*(-(a.m[1][2]*a.m[2][0]) + a.m[1][0]*a.m[2][2]) + a.m[0][2]*(a.m[1][3]*a.m[2][0] -
                a.m[1][0]*a.m[2][3]) + a.m[0][0]*(-(a.m[1][3]*a.m[2][2]) + a.m[1][2]*a.m[2][3]));
        m[2][0] = idet * (a.m[1][3]*(-(a.m[2][1]*a.m[3][0]) + a.m[2][0]*a.m[3][1]) + a.m[1][1]*(a.m[2][3]*a.m[3][0] -
                a.m[2][0]*a.m[3][3]) + a.m[1][0]*(-(a.m[2][3]*a.m[3][1]) + a.m[2][1]*a.m[3][3]));
        m[2][1] = idet * (a.m[0][3]*(a.m[2][1]*a.m[3][0] - a.m[2][0]*a.m[3][1]) + a.m[0][1]*(-(a.m[2][3]*a.m[3][0]) +
                a.m[2][0]*a.m[3][3]) + a.m[0][0]*(a.m[2][3]*a.m[3][1] - a.m[2][1]*a.m[3][3]));
        m[2][2] = idet * (a.m[0][3]*(-(a.m[1][1]*a.m[3][0]) + a.m[1][0]*a.m[3][1]) + a.m[0][1]*(a.m[1][3]*a.m[3][0] -
                a.m[1][0]*a.m[3][3]) + a.m[0][0]*(-(a.m[1][3]*a.m[3][1]) + a.m[1][1]*a.m[3][3]));
        m[2][3] = idet * (a.m[0][3]*(a.m[1][1]*a.m[2][0] - a.m[1][0]*a.m[2][1]) + a.m[0][1]*(-(a.m[1][3]*a.m[2][0]) +
                a.m[1][0]*a.m[2][3]) + a.m[0][0]*(a.m[1][3]*a.m[2][1] - a.m[1][1]*a.m[2][3]));
        m[3][0] = idet * (a.m[1][2]*(a.m[2][1]*a.m[3][0] - a.m[2][0]*a.m[3][1]) + a.m[1][1]*(-(a.m[2][2]*a.m[3][0]) +
                a.m[2][0]*a.m[3][2]) + a.m[1][0]*(a.m[2][2]*a.m[3][1] - a.m[2][1]*a.m[3][2]));
        m[3][1] = idet * (a.m[0][2]*(-(a.m[2][1]*a.m[3][0]) + a.m[2][0]*a.m[3][1]) + a.m[0][1]*(a.m[2][2]*a.m[3][0] -
                a.m[2][0]*a.m[3][2]) + a.m[0][0]*(-(a.m[2][2]*a.m[3][1]) + a.m[2][1]*a.m[3][2]));
        m[3][2] = idet * (a.m[0][2]*(a.m[1][1]*a.m[3][0] - a.m[1][0]*a.m[3][1]) + a.m[0][1]*(-(a.m[1][2]*a.m[3][0]) +
                a.m[1][0]*a.m[3][2]) + a.m[0][0]*(a.m[1][2]*a.m[3][1] - a.m[1][1]*a.m[3][2]));
        m[3][3] = idet * (a.m[0][2]*(-(a.m[1][1]*a.m[2][0]) + a.m[1][0]*a.m[2][1]) + a.m[0][1]*(a.m[1][2]*a.m[2][0] -
                a.m[1][0]*a.m[2][2]) + a.m[0][0]*(-(a.m[1][2]*a.m[2][1]) + a.m[1][1]*a.m[2][2]));
    }
    void transpose() {
        for (int i = 0; i < 4; i++)
            for (int j = i + 1; j < 4; j++)
                std::swap(m[i][j], m[j][i]);
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

struct OrthoMatrix : public Matrix {
public:
    OrthoMatrix(float zNear, float zFar, float vHalfSize, float aspectRatio) {
        assert(zNear > 0 && zFar > zNear);
        float S = 1.f / vHalfSize;
        m[0][0] = S / aspectRatio;
        m[1][1] = S;
        m[2][2] = 2 / (zNear - zFar);
        m[2][3] = (zFar + zNear) / (zNear - zFar);
        m[3][3] = 1;
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

struct RotateAxis : public Matrix {
public:
    RotateAxis(float x1, float y1, float z1, float x2, float y2, float z2) {
        /* Wont work for 180 degree rotation. Not ours case anyway */
        float r1 = sqrt(x1 * x1 + y1 * y1 + z1 * z1);
        float r2 = sqrt(x2 * x2 + y2 * y2 + z2 * z2);

        x1 /= r1;
        y1 /= r1;
        z1 /= r1;

        x2 /= r2;
        y2 /= r2;
        z2 /= r2;

        float vx = y1 * z2 - z1 * y2;
        float vy = z1 * x2 - x1 * z2;
        float vz = x1 * y2 - y1 * x2;

        float c = x1 * x2 + y1 * y2  + z1 * z2;
        /*
         * R = c I + s [u]_x + (1 - c) uu
         * v = s u
         * R = c I + [v]_x + (1 - c) / s^2 vv
         *
         * (1 - c) / s^2 = (1 - c) / (1 - c * c) = 1 / (1 + c)
         *
         * R = c I + [v]_x + 1 / (1 + c) vv
         * */

        float g = 1 / (1 + c);
        m[3][3] = 1;

        m[0][0] = c + g * vx * vx;
        m[1][1] = c + g * vy * vy;
        m[2][2] = c + g * vz * vz;

        m[0][1] = -vz + g * vx * vy;
        m[0][2] =  vy + g * vx * vz;

        m[1][0] =  vz + g * vy * vx;
        m[1][2] = -vx + g * vy * vz;

        m[2][0] = -vy + g * vz * vx;
        m[2][1] =  vx + g * vz * vy;
    }
};

#endif
