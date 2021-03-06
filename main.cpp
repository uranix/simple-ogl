#include "RendererFacede.h"
#include "EngineFacede.h"

#include <iostream>

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(EngineFacede::name());

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW failed to initialize. Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
    if (!GLEW_VERSION_3_3) {
        std::cerr << "OpenGL 3.3 is not supported" << std::endl;
        return 1;
    }
    std::cout << "Using GLEW version " << glewGetString(GLEW_VERSION) << std::endl;

    RendererFacede::init();
    EngineFacede::init();

    glutDisplayFunc(RendererFacede::display);
    glutReshapeFunc(RendererFacede::reshape);

    glutKeyboardFunc(EngineFacede::keyboard);
    glutMouseFunc(EngineFacede::click);
    glutMotionFunc(EngineFacede::motion);

    glutMainLoop();

    return 0;
}
