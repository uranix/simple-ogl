#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <GL/glew.h>
#include <GL/glut.h>

#include "Matrix.h"

struct Renderer;

struct Engine {
    bool buttonPressed;
    int startx, starty;
    int zoomFactor;
    Matrix rotMatrix;

    float cx, cy, cz; /* model center */
    GLuint modelVao;
    GLuint wireVao;
    GLuint modelVbo;
    GLuint modelIbo;

    int numVertices, numElements;

    Engine();
    void loadMesh();
    Matrix getViewMatrix();
    void showScene(Renderer &r);
    void keyboard(unsigned char key, int x, int y);
    void click(int button, int state, int x, int y);
    void motion(int x, int y);
    void dragging(int dx, int dy);
    void stopDragging(int dx, int dy);
    void zoom(int inc);
    static const char *name();
};

#endif
