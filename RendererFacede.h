#ifndef __RENDERERFACEDE_H__
#define __RENDERERFACEDE_H__

#include "Renderer.h"
#include "EngineFacede.h"

class RendererFacede {
    static Renderer &instance() {
        static Renderer This;
        return This;
    }
public:
    static void init() {
        instance();
    }
    static void display() {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(instance().program);

        instance().setProjection();
        EngineFacede::showScene(instance());
        glUseProgram(0);

//        glFlush();
        glutSwapBuffers();
//        glFinish();

        glutPostRedisplay();
    }
    static void reshape(int width, int height) {
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);
        instance().reshape(width, height);
    }
};

#endif
