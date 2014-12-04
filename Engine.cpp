#include "Engine.h"
#include "Renderer.h"
#include "Matrix.h"

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
/*            GLenum polyMode = glGet(GL_POLYGON_MODE);
            if (polyMode == GL_FILL)
                polyMode = GL_LINE;
            else if (polyMode = GL_LINE)
                polyMode = GL_FILL;
            glPolygonMode(polyMode);*/
            break;
        case '+':
            std::cout << "Refine" << std::endl;
            break;
        case '-':
            std::cout << "Coarser" << std::endl;
            break;
        case ' ':
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

    glGenVertexArrays(1, &modelVao);
    glGenVertexArrays(1, &wireVao);

    glGenBuffers(1, &modelVbo);
    glGenBuffers(1, &modelIbo);

    numVertices = numElements = 0;
}

#include "dataset.h"

void Engine::loadMesh() {
    numVertices = sizeof(vertexData) / 3 / sizeof(float) / 3;
    numElements = sizeof(indexData) / sizeof(indexData[0]);

    std::cout << "Loaded mesh with " << numVertices << " vertices and " << numElements << " indices" << std::endl;

    glBindVertexArray(modelVao);
    glBindBuffer(GL_ARRAY_BUFFER, modelVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, /*sz*/3, GL_FLOAT, /*normalize*/GL_FALSE, /*stride*/0, /*offset*/0);
    glVertexAttribPointer(1, /*sz*/3, GL_FLOAT, /*normalize*/GL_FALSE, /*stride*/0, /*offset*/(GLvoid *)(numVertices * 3 * sizeof(float)));

    glBindVertexArray(wireVao);
    glBindBuffer(GL_ARRAY_BUFFER, modelVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, /*sz*/3, GL_FLOAT, /*normalize*/GL_FALSE, /*stride*/0, /*offset*/0);
    glVertexAttribPointer(1, /*sz*/3, GL_FLOAT, /*normalize*/GL_FALSE, /*stride*/0, /*offset*/(GLvoid *)(2 * numVertices * 3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    cx = 0, cy = 0, cz = 0;
    for (int i = 0; i < numVertices; i++) {
        cx += vertexData[3 * i + 0];
        cy += vertexData[3 * i + 1];
        cz += vertexData[3 * i + 2];
    }
    cx /= numVertices;
    cy /= numVertices;
    cz /= numVertices;
}

Matrix Engine::getViewMatrix() {
    float sf = exp(0.05 * zoomFactor);
    Matrix tmpMatrix((IdentityMatrix()));

    tmpMatrix.multWithLeft(rotMatrix);
    tmpMatrix.multWithLeft(Scale(sf, sf, sf));
    tmpMatrix.multWithLeft(Translate(0, 0, -2));

    return tmpMatrix;
}

void Engine::showScene(Renderer &r) {
    r.setViewMatrix(getViewMatrix());

    glBindBuffer(GL_ARRAY_BUFFER, modelVbo);

    Matrix translateToCenter(Translate(-cx, -cy, -cz));
    Matrix mm1(translateToCenter);
    mm1.multWithLeft(Translate(-1.5f, 0, 0));
    r.setModelMatrix(mm1);

    glBindVertexArray(modelVao);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

    Matrix mm2(translateToCenter);
    mm2.multWithLeft(Translate(1.5f, 0, 0));
    r.setModelMatrix(mm2);

//    glBindVertexArray(wireVao);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
