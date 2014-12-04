#include "Engine.h"
#include "Renderer.h"
#include "Matrix.h"

#include "Mesh.h"

#include "tinyfiledialogs.h"

#include <iostream>
#include <cstdlib>

#ifdef __FREEGLUT_STD_H__
# include <GL/freeglut_ext.h>
#else
void glutLeaveMainLoop() {
    exit(0);
}
#endif

const char *Engine::name() {
    return "OpenGL";
}

void Engine::keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
        case 'q':
            glutLeaveMainLoop();
            break;
        case 'w':
            wireframe = !wireframe;
            break;
        case '+':
            level++;
            if (level >= maxLevels)
                level = maxLevels - 1;
            break;
        case '-':
            level--;
            if (level < 0)
                level = 0;
            break;
        case 'l':
            loadMesh();
            break;
    }
}

enum MouseButtons {
    LEFT_MOUSE_BUTTON = GLUT_LEFT_BUTTON,
    MIDDLE_MOUSE_BUTTON = GLUT_MIDDLE_BUTTON,
    RIGHT_MOUSE_BUTTON = GLUT_RIGHT_BUTTON,
    WHEEL_UP = 3,
    WHEEL_DOWN = 4
};

void Engine::click(int button, int state, int x, int y) {
    if (button == LEFT_MOUSE_BUTTON) {
        buttonPressed = state == GLUT_DOWN;
        if (buttonPressed) {
            startx = x;
            starty = y;
        }
    }
    /* Whell generates a pair of UP & DOWN events, ignore DOWN */
    if (button == WHEEL_UP && state == GLUT_UP)
        zoom(1);
    if (button == WHEEL_DOWN && state == GLUT_DOWN)
        zoom(-1);
}

void Engine::motion(int x, int y) {
    if (buttonPressed) {
        dragging(x - startx, y - starty);
        startx = x;
        starty = y;
    }
}

void Engine::zoom(int inc) {
    zoomFactor += inc;
}

void Engine::dragging(int dx, int dy) {
    float phi = 0.2f * dx;
    float theta = 0.2f * dy;
    rotMatrix.multWithLeft(RotateMatrix(phi, theta));
}

Engine::Engine() : rotMatrix(IdentityMatrix()) {
    zoomFactor = 0;
    wireframe = false;
    maxLevels = 16;

    glGenVertexArrays(1, &modelVao);
    glGenVertexArrays(1, &wireVao);

    glGenBuffers(1, &modelVbo);
    glGenBuffers(1, &modelIbo);

    glGenBuffers(1, &treeVbo);
    glGenBuffers(1, &treeIbo);

    numVertices = numElements = 0;
}

void Engine::loadMesh() {
    const char *filters[] = {"*.ply", "*.PLY"};
//    const char *fn = tinyfd_openFileDialog("Load PLY file", "", 2, filters, 0);
    const char *fn = "/home/uranix/ogl/teapot.ply";

    if (!fn)
        return;

    Mesh m(fn);

    numVertices = m.verts().size();
    numElements = m.faces().size() * 3;

    cx = 0, cy = 0, cz = 0;

    const std::vector<Point> &vertexData = m.verts();
    const std::vector<Face> &faceData = m.faces();

    for (int i = 0; i < numVertices; i++) {
        cx += vertexData[i].x;
        cy += vertexData[i].y;
        cz += vertexData[i].z;
    }
    cx /= numVertices;
    cy /= numVertices;
    cz /= numVertices;

    std::vector<AABB> tree;

    std::vector<std::vector<Point> > vertices((1 << maxLevels) - 1);
    tree.resize(vertices.size());

    for (int i = 0; i < numVertices; i++) {
        float x = vertexData[i].x - cx;
        float y = vertexData[i].y - cy;
        float z = vertexData[i].z - cz;
        vertices[0].push_back(Point(x, y, z));
    }

    for (size_t i = 0; i < vertices.size(); i++) {
        for (const auto &p : vertices[i])
            tree[i].add(p.x, p.y, p.z);

        size_t ileft = 2 * i + 1;
        size_t iright = 2 * i + 2;
        if (ileft >= vertices.size())
            continue;
        for (const auto &p : vertices[i]) {
            if (tree[i].isInLeft(p.x, p.y, p.z))
                vertices[ileft].push_back(p);
            else
                vertices[iright].push_back(p);
        }
    }

    radius = tree[0].radius();
    float add = radius * 0.01 / (1 << level);
    for (auto &t : tree)
        t.inflate(add);

    std::cout << "Loaded mesh with " << numVertices << " vertices and " << numElements << " indices" << std::endl;

    glBindVertexArray(modelVao);
    glBindBuffer(GL_ARRAY_BUFFER, modelVbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * faceData.size() * sizeof(GLuint), faceData.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, /*sz*/3, GL_FLOAT, /*normalize*/GL_FALSE, /*stride*/0, /*offset*/0);

#if 0
    glBindVertexArray(wireVao);
    std::vector<float> boxData(8 * 3 * tree.size());
    std::vector<GLuint> treeIdx;
    GLuint boxIdx[4 * 4] = {0, 1, 2, 3, 7, 6, 5, 4, 1, 0, 4, 5, 6, 7, 3, 2};
    for (size_t i = 0; i < tree.size(); i++) {
        tree[i].writeVertex(&boxData[8 * 3 * i]);
        treeIdx.insert(treeIdx.end(), boxIdx, boxIdx + 4 * 4);
        for (int j = 0; j < 16; j++)
            boxIdx[j] += 8;
    }

    glBindBuffer(GL_ARRAY_BUFFER, treeVbo);
    glBufferData(GL_ARRAY_BUFFER, boxData.size() * sizeof(float), boxData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, treeIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, treeIdx.size() * sizeof(GLuint), treeIdx.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, /*sz*/3, GL_FLOAT, /*normalize*/GL_FALSE, /*stride*/0, /*offset*/0);

#endif
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Matrix Engine::getViewMatrix() {
    float sf = exp(0.05 * zoomFactor) / radius;
    Matrix tmpMatrix((IdentityMatrix()));

    tmpMatrix.multWithLeft(rotMatrix);
    tmpMatrix.multWithLeft(Scale(sf));
    tmpMatrix.multWithLeft(Translate(0, 0, -2));

    return tmpMatrix;
}

void Engine::showScene(Renderer &r) {
    if (numVertices == 0)
        return;

    r.setViewMatrix(getViewMatrix());

    glBindVertexArray(modelVao);
    glBindBuffer(GL_ARRAY_BUFFER, modelVbo);

    Matrix translateToCenter(Translate(-cx, -cy, -cz));
    Matrix mm(translateToCenter);
    r.setColor(0, 1, 0, 1);
    r.setModelMatrix(mm);

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
    std::cout << "Error: " << glGetError() << std::endl;

    glBindVertexArray(0);
#if 0
    return;

    r.setModelMatrix(IdentityMatrix());
    r.setColor(0, 0, 1, 1);
    glBindVertexArray(wireVao);
    glBindBuffer(GL_ARRAY_BUFFER, treeVbo);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int beg = (1 << level) - 1;
    int end = beg + (1 << level);

    glDrawElements(GL_QUADS, 16 * (end - beg), GL_UNSIGNED_INT, (GLvoid *)(16 * beg * sizeof(GLuint)));

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}
