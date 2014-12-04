#ifndef __ENGINEFACADE_H__
#define __ENGINEFACADE_H__

#include "Matrix.h"

struct Engine;
struct Renderer;

class EngineFacede {
    static Engine &instance();
public:
    static void init();
    static Matrix getViewMatrix();
    static void showScene(Renderer &);
    static void keyboard(unsigned char key, int x, int y);
    static void click(int button, int state, int x, int y);
    static void motion(int x, int y);
    static const char *name();
};

#endif
