#include "EngineFacede.h"
#include "Engine.h"

Engine &EngineFacede::instance() {
    static Engine This;
    return This;
}
void EngineFacede::init() {
    instance();
}
Matrix EngineFacede::getViewMatrix() {
    return instance().getViewMatrix();
}
void EngineFacede::showScene(Renderer &r) {
    instance().showScene(r);
}
void EngineFacede::keyboard(unsigned char key, int x, int y) {
    instance().keyboard(key, x, y);
}
void EngineFacede::click(int button, int state, int x, int y) {
    instance().click(button, state, x, y);
}
void EngineFacede::motion(int x, int y) {
    instance().motion(x, y);
}
const char *EngineFacede::name() {
    return Engine::name();
}
