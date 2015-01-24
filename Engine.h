#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Matrix.h"
#include "Mesh.h"

#include <vector>
#include <memory>

struct Renderer;

struct Engine {
    bool buttonPressed;
    int startx, starty;
    int viewWidth, viewHeight;
    int zoomFactor;
    bool wireframe;
    bool cull;
    enum {
        GOURAUD, PHONG, FLAT
    } shading;
    float specularity;

    Matrix rotMatrix;
    int level;
    int maxLevels;
    float radius;

    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<TriMesh> m;

    GLuint modelVao;
    GLuint wireVao;
    GLuint modelVbo;
    GLuint modelIbo;
    GLuint treeVbo;
    GLuint treeIbo;

    Engine();
    void refine();
    void saveMesh();
    void loadMesh();
    void buildTree();
    Matrix getViewMatrix();
    void showScene(Renderer &r);
    void drawModel(Renderer &r);
    void drawBoxes(Renderer &r);
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
