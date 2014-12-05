#ifndef __RENDERERFACEDE_H__
#define __RENDERERFACEDE_H__

#include "Renderer.h"
#include "EngineFacede.h"

#ifdef _WINDOWS
# include <windows.h>
#else
# include <thread>
inline void Sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
#endif

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
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glDepthRange(0, 1);

        glUseProgram(instance().program);

        instance().setPerspective();
//        instance().setOrtho();
        EngineFacede::showScene(instance());
        glUseProgram(0);

        /* Render overlay with legacy API */
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, instance().viewWidth, 0, instance().viewHeight, 1, -1);
        EngineFacede::showOverlay(instance());

        glutSwapBuffers();
        int ms = instance().countFpsAndReturnWait();

        Sleep(ms);

        glutPostRedisplay();
    }
    static void reshape(int width, int height) {
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);
        instance().reshape(width, height);
        EngineFacede::reshape(width, height);
    }
};

#endif
