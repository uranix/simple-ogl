#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <GL/glew.h>
#include <GL/glut.h>

#include "Matrix.h"

struct Renderer {
    GLuint program;
    float prevTime;
    int frames;
    float fps;
    float maxFps;

    float viewWidth, viewHeight;

    Renderer();
    void setModelMatrix(const Matrix &m);
    void setViewMatrix(const Matrix &m);
    void setProjection();
    void reshape(int x, int y);
    int countFps();
};

#endif
