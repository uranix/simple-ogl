#include "Renderer.h"
#include "EngineFacede.h"
#include "Matrix.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

std::string loadFileToString(const std::string &filename) {
    std::fstream f(filename, std::ios::in);
    if (!f) {
        std::cerr << "Could not open file `" << filename << "'" << std::endl;
        exit(0);
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

GLuint compileShader(GLenum eShaderType, const std::string &shaderFile) {
    GLuint shader = glCreateShader(eShaderType);
    std::string fileString(loadFileToString(shaderFile));
    const char *fileData = fileString.c_str();

    glShaderSource(shader, 1, &fileData, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(infoLogLength + 1, 0);
        glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog.data());

        std::string shaderType;
        switch(eShaderType) {
            case GL_VERTEX_SHADER:
                shaderType = "Vertex";
                break;
            case GL_GEOMETRY_SHADER:
                shaderType = "Geometry";
                break;
            case GL_FRAGMENT_SHADER:
                shaderType = "Fragment";
                break;
        }

        std::cerr << shaderType << " shader failed to compile with message " << infoLog.data() << std::endl;
        exit(0);
    }

    return shader;
}

GLuint linkShaders(std::vector<GLuint> &shaders) {
    GLuint program = glCreateProgram();

    for (auto shader = shaders.begin(); shader != shaders.end(); shader++)
        glAttachShader(program, *shader);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(infoLogLength + 1, 0);
        glGetProgramInfoLog(program, infoLogLength, NULL, infoLog.data());

        std::cerr << "Shader program failed to link with message " << infoLog.data() << std::endl;
        exit(0);
    }

    for (auto shader = shaders.begin(); shader != shaders.end(); shader++)
        glDetachShader(program, *shader);

    return program;
}

GLuint buildShaderProgramForModel() {
    std::vector<GLuint> shaders;

    shaders.push_back(compileShader(GL_VERTEX_SHADER, "transform.vert"));
    shaders.push_back(compileShader(GL_GEOMETRY_SHADER, "triangles.geom"));
    shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "light.frag"));

    return linkShaders(shaders);
}

GLuint buildShaderProgramForBoxes() {
    std::vector<GLuint> shaders;

    shaders.push_back(compileShader(GL_VERTEX_SHADER, "transform.vert"));
    shaders.push_back(compileShader(GL_GEOMETRY_SHADER, "lines.geom"));
    shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "light.frag"));

    return linkShaders(shaders);
}

Renderer::Renderer() : model(IdentityMatrix()), view(IdentityMatrix()) {
    modelProgram = buildShaderProgramForModel();
    boxesProgram = buildShaderProgramForBoxes();

    fps = 0;
    frames = 0;
    prevTime = 0;

    maxFps = 120;

    viewWidth = viewHeight = 1;
}

void Renderer::useModelShader() {
    glUseProgram(modelProgram);
}

void Renderer::useBoxesShader() {
    glUseProgram(boxesProgram);
}

GLuint program() {
    int prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    return prog;
}

float Renderer::getFps() const {
    return fps;
}

int Renderer::countFpsAndReturnWait() {
    frames++;

    float now = static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
    float elaps = 1e-3f * (now - prevTime);

    if (frames >= 20) {
        fps = frames / elaps;
        prevTime = now;
        frames = 0;
    }

    float waitTill = frames / maxFps;
    if (elaps < waitTill) {
        int ms = static_cast<int>(1e3f * (waitTill - elaps));
        return ms;
    }
    return 0;
}

void Renderer::setModelMatrix(const Matrix &m) {
    model = m;
    updateModelView();
}

void Renderer::setViewMatrix(const Matrix &m) {
    view = m;
    updateModelView();
}

void Renderer::updateModelView() {
    Matrix tmp(model);
    tmp.multWithLeft(view);
    GLint modelView = glGetUniformLocation(program(), "modelView");
    glUniformMatrix4fv(modelView, /*num*/1, /*row major*/GL_TRUE, tmp.data());

    tmp.inverse();
    tmp.transpose();
    GLint normalMatrix = glGetUniformLocation(program(), "normalMatrix");
    glUniformMatrix4fv(normalMatrix, 1, GL_TRUE, tmp.data());
}

void Renderer::setPerspective() {
    PerspectiveMatrix m(0.5f, 4.5f, 30, viewWidth / viewHeight);
    GLint projMatrix = glGetUniformLocation(program(), "projMatrix");
    glUniformMatrix4fv(projMatrix, /*num*/1, /*row major*/GL_TRUE, m.data());
}

void Renderer::setOrtho() {
    OrthoMatrix m(0.5f, 4.5f, 1.0f, viewWidth / viewHeight);
    GLint projMatrix = glGetUniformLocation(program(), "projMatrix");
    glUniformMatrix4fv(projMatrix, /*num*/1, /*row major*/GL_TRUE, m.data());
}

void Renderer::setColor(float r, float g, float b, float a) {
    GLint mainColor = glGetUniformLocation(program(), "mainColor");
    glUniform4f(mainColor, r, g, b, a);
}

void Renderer::smoothNormals(bool v) {
    GLint smoothNormals = glGetUniformLocation(program(), "smoothNormals");
    glUniform1i(smoothNormals, v ? 1 : 0);
}

void Renderer::shadePhong(bool v) {
    GLint shadePhong = glGetUniformLocation(program(), "shadePhong");
    glUniform1i(shadePhong, v ? 1 : 0);
}

void Renderer::reshape(GLint w, GLint h) {
    viewWidth = static_cast<float>(w);
    viewHeight = static_cast<float>(h);
}

void Renderer::setLightIntens(float v) {
    GLint lightIntens = glGetUniformLocation(program(), "lightIntens");
    glUniform1f(lightIntens, v);
}
