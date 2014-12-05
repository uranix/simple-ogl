#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Matrix.h"

#include <vector>

struct Renderer;

struct Engine {
    bool buttonPressed;
    int startx, starty;
    int viewWidth, viewHeight;
    int zoomFactor;
    bool wireframe;
    bool cull;

    Matrix rotMatrix;
    int level;
    int maxLevels;
    float radius;

    std::string meshfile;
    float cx, cy, cz; /* model center */
    GLuint modelVao;
    GLuint wireVao;
    GLuint modelVbo;
    GLuint modelIbo;
    GLuint treeVbo;
    GLuint treeIbo;

    int numVertices, numElements;

    Engine();
    void loadMesh();
    Matrix getViewMatrix();
    void showScene(Renderer &r);
    void showOverlay(Renderer &r);
    void keyboard(unsigned char key, int x, int y);
    void click(int button, int state, int x, int y);
    void motion(int x, int y);
    void reshape(int w, int h);
    void dragging(int dx, int dy);
    void stopDragging(int dx, int dy);
    void zoom(int inc);
    static const char *name();
};

#endif
