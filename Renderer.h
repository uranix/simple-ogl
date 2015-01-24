#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <GL/glew.h>
#include <GL/glut.h>

#include "Matrix.h"

struct Renderer {
    GLuint modelProgram;
    GLuint boxesProgram;

    Matrix model;
    Matrix view;

    float prevTime;
    int frames;
    float fps;
    float maxFps;

    float viewWidth, viewHeight;

    Renderer();
    void useModelShader();
    void useBoxesShader();
    void updateModelView();
    void setModelMatrix(const Matrix &m);
    void setViewMatrix(const Matrix &m);
    void setColor(float r, float g, float b, float a);
    void setLightIntens(float v);
    void smoothNormals(bool v);
    void shadePhong(bool v);
    void setPerspective();
    void setOrtho();
    void reshape(int x, int y);
    int countFpsAndReturnWait();
    float getFps() const;
};

#endif
