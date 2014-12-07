#include "Engine.h"
#include "Renderer.h"
#include "Matrix.h"

#include "Mesh.h"
#include "Box.h"

#include "tinyfiledialogs.h"

#include <iostream>
#include <memory>
#include <cstdlib>
#include <cstdio>

#ifdef __FREEGLUT_STD_H__
# include <GL/freeglut_ext.h>
#else
void glutLeaveMainLoop() {
    exit(0);
}
#endif

const char *Engine::name() {
    return "Mesh viewer";
}

void Engine::keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
        case 'q':
        case 'Q':
            glutLeaveMainLoop();
            break;
        case 'n':
        case 'N':
            interpnorm = !interpnorm;
            break;
        case 'w':
        case 'W':
            wireframe = !wireframe;
            break;
        case 'c':
        case 'C':
            cull = !cull;
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
        case 'L':
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

void Engine::reshape(int w, int h) {
    viewWidth = w;
    viewHeight = h;
}

void Engine::zoom(int inc) {
    zoomFactor += inc;
}

void Engine::dragging(int dx, int dy) {
    float w = static_cast<float>(viewWidth);
    float h = static_cast<float>(viewHeight);

    float x1 = (startx - 0.5f * w) / h;
    float y1 = (0.5f * h - starty) / h;
    float z1 = 1;
    float x2 = x1 + dx / h;
    float y2 = y1 - dy / h;
    float z2 = 1;
    float x3 = 0, y3 = 0, z3 = 1;

    Matrix first(RotateAxis(x1, y1, z1, x3, y3, z3));
    Matrix second(RotateAxis(x3, y3, z3, x2, y2, z2));

    Matrix rot = IdentityMatrix();

    int speedup = 3;
    for (int i = 0; i < speedup; i++) {
        rot.multWithRight(first);
        rot.multWithLeft(second);
    }

    rotMatrix.multWithLeft(rot);
}

Engine::Engine() : rotMatrix(IdentityMatrix()) {
    zoomFactor = 0;
    wireframe = false;
    interpnorm = true;
    maxLevels = 15;

    viewWidth = viewHeight = 1;

    glGenVertexArrays(1, &modelVao);
    glGenVertexArrays(1, &wireVao);

    glGenBuffers(1, &modelVbo);
    glGenBuffers(1, &modelIbo);

    glGenBuffers(1, &treeVbo);
    glGenBuffers(1, &treeIbo);
}

void Engine::loadMesh() {
    const char *filters[] = {"*.ply", "*.PLY"};
    const char *fn = tinyfd_openFileDialog("Load PLY file", "", 2, filters, 0);

    if (!fn)
        return;

    try {
        mesh = std::move(std::unique_ptr<PLYMesh>(new PLYMesh(fn)));
        m = std::move(std::unique_ptr<TriMesh>(new TriMesh(*mesh)));
    } catch (std::exception &e) {
        std::cerr << "Loading mesh failed: " << e.what() << std::endl;
        return;
    }

    const std::vector<Point> &vertexData = m->vertsWithNormals();
    const std::vector<Face> &faceData = m->faces();

    int numVertices = vertexData.size() / 2;

    std::vector<std::vector<Face> > faceTree((1 << maxLevels) - 1);

    faceTree[0] = faceData;

    std::vector<AABB> tree(faceTree.size());
    Point center = mesh->center();

    for (size_t i = 0; i < faceTree.size(); i++) {
        AABB box;
        for (auto f = faceTree[i].begin(); f != faceTree[i].end(); f++) {
            box.add(Point(vertexData[f->v1], center));
            box.add(Point(vertexData[f->v2], center));
            box.add(Point(vertexData[f->v3], center));
        }

        tree[i] = box;

        size_t ileft = 2 * i + 1;
        size_t iright = 2 * i + 2;

        if (ileft >= faceTree.size())
            continue;
        for (auto f = faceTree[i].begin(); f != faceTree[i].end(); f++) {
            if (box.hasOnLeft (*f, vertexData, center))
                faceTree[ileft ].push_back(*f);
            if (box.hasOnRight(*f, vertexData, center))
                faceTree[iright].push_back(*f);
        }
    }

    radius = tree[0].radius();

    glBindVertexArray(modelVao);
    glBindBuffer(GL_ARRAY_BUFFER, modelVbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * faceData.size() * sizeof(GLuint), faceData.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, /*sz*/3, GL_FLOAT, /*normalize*/GL_FALSE, /*stride*/0, /*offset*/0);
    glVertexAttribPointer(1, /*sz*/3, GL_FLOAT, /*normalize*/GL_FALSE, /*stride*/0, /*offset*/(GLvoid *)(numVertices * 3 * sizeof(float)));

    glBindVertexArray(wireVao);
    std::vector<float> boxData(8 * 3 * tree.size());
    std::vector<GLuint> treeIdx;
    GLuint boxIdx[4 * 6] = {
        0, 1, 2, 3, 0, 3, 1, 2,
        7, 6, 5, 4, 4, 7, 5, 6,
        1, 5, 0, 4, 3, 7, 2, 6};
    for (size_t i = 0; i < tree.size(); i++) {
        tree[i].writeVertex(&boxData[8 * 3 * i]);
        treeIdx.insert(treeIdx.end(), boxIdx, boxIdx + 4 * 6);
        for (int j = 0; j < 4 * 6; j++)
            boxIdx[j] += 8;
    }

    glBindBuffer(GL_ARRAY_BUFFER, treeVbo);
    glBufferData(GL_ARRAY_BUFFER, boxData.size() * sizeof(float), boxData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, treeIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, treeIdx.size() * sizeof(GLuint), treeIdx.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, /*sz*/3, GL_FLOAT, /*normalize*/GL_FALSE, /*stride*/0, /*offset*/0);
    glDisableVertexAttribArray(1);
    glVertexAttrib4f(1, 1.f, 0.f, 0.f, 1.f); /* default normal for boxes */

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Matrix Engine::getViewMatrix() {
    float sf = exp(0.05f * zoomFactor) / radius;
    Matrix tmpMatrix((IdentityMatrix()));

    tmpMatrix.multWithLeft(rotMatrix);
    tmpMatrix.multWithLeft(Scale(sf));
    tmpMatrix.multWithLeft(Translate(Point(0.f, 0.f, -2.f)));

    return tmpMatrix;
}

void Engine::drawModel(Renderer &r) {
    r.useModelShader();
    r.setPerspective();

    r.setViewMatrix(getViewMatrix());

    glBindVertexArray(modelVao);
    glBindBuffer(GL_ARRAY_BUFFER, modelVbo);

    Matrix translateToCenter(Translate(-mesh->center()));
    Matrix mm(translateToCenter);
    r.setColor(.8f, .75f, .5f, 1.f);
    r.setLightIntens(.5f);
    r.setModelMatrix(mm);

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (cull)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    r.smoothNormals(interpnorm);

    glDrawElements(GL_TRIANGLES, m->faces().size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Engine::drawBoxes(Renderer &r) {
    r.useBoxesShader();
    r.setPerspective();

    r.setViewMatrix(getViewMatrix());

    glBindVertexArray(wireVao);
    glBindBuffer(GL_ARRAY_BUFFER, treeVbo);
    r.setModelMatrix(IdentityMatrix());
    r.setColor(0, 0, 0, 1);
    r.setLightIntens(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int beg = (1 << level) - 1;
    int end = beg + (1 << level);

    glDisable(GL_CULL_FACE);
    glDrawElements(GL_LINES, 4 * 6 * (end - beg), GL_UNSIGNED_INT, (GLvoid *)(4 * 6 * beg * sizeof(GLuint)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Engine::showScene(Renderer &r) {
    if (!m)
        return;

    drawModel(r);
    drawBoxes(r);
}

void putLine(float xkey, float xval, float yline, const std::string &key, const std::string &val) {
    glRasterPos2f(xkey, yline);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)key.c_str());
    glRasterPos2f(xval, yline);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)val.c_str());
}

void Engine::showOverlay(Renderer &r) {
    glColor4f(0, 0, 0, .8f);

    float widthpx = 400.f;
    float heightpx = 240.f;

    glBegin(GL_QUADS);
    glVertex2f(10.f, 10.f);
    glVertex2f(10.f, heightpx + 10.f);
    glVertex2f(10.f + widthpx, heightpx + 10.f);
    glVertex2f(10.f + widthpx, 10.f);
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    char buf[128];
    sprintf(buf, "%.2f", r.getFps());

    float x1 = 20.f;
    float x2 = 120.f;
    float y = heightpx - 10.f;

    putLine(x1, x2, y, "fps:", buf);
    y -= 20.f;
    putLine(x1, x2, y, "mesh:", mesh ? mesh->filename() : std::string("No mesh loaded"));
    y -= 20.f;
    long long numVertices = mesh ? mesh->numVertices() : 0;
    putLine(x1, x2, y, "vertex count:", std::to_string(numVertices));
    y -= 20.f;
    long long numFaces = mesh ? mesh->numFaces() : 0;
    putLine(x1, x2, y, "face count:", std::to_string(numFaces));
    y -= 20.f;
    putLine(x1, x2, y, "tree level:", std::to_string(static_cast<long long>(level)));
    y -= 20.f;
    putLine(x1, x2, y, "face culling:", cull ? "on" : "off");
    y -= 20.f;
    putLine(x1, x2, y, "wireframe:", wireframe ? "on" : "off");
    y -= 20.f;
    putLine(x1, x2, y, "smooth normals:", interpnorm ? "on" : "off");
    y -= 30.f;
    putLine(x1, x1, y, "", "Drag to rotate model, rotate wheel to zoom");
    y -= 20.f;
    putLine(x1, x1, y, "", "Esc, Q : quit,  +/-: change level,  L: load mesh");
    y -= 20.f;
    putLine(x1, x1, y, "", "Togglers: W : wireframe mode,  C: face culling, N: normal smoothing");
}
