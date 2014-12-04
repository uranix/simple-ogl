#include "Renderer.h"
#include "Matrix.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

std::string loadFileToString(const std::string &filename) {
    std::fstream f(filename, std::ios::in);
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

    for (auto shader : shaders)
        glAttachShader(program, shader);

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

    for (auto shader : shaders)
        glDetachShader(program, shader);

    return program;
}

GLuint buildShaderProgram() {
    std::vector<GLuint> shaders;

    shaders.push_back(compileShader(GL_VERTEX_SHADER, "shader.vert"));
    shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "shader.frag"));

    return linkShaders(shaders);
}

Renderer::Renderer() {
    program = buildShaderProgram();
    fps = 0;
    frames = 0;
    prevTime = 0;

    maxFps = 120;

    viewWidth = viewHeight = 1;
/*
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
*/
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0, 1);
}

void Renderer::setModelMatrix(const Matrix &m) {
    GLuint modelMatrix = glGetUniformLocation(program, "modelMatrix");
    glUniformMatrix4fv(modelMatrix, /*num*/1, /*row major*/GL_TRUE, m.data());
}

void Renderer::setViewMatrix(const Matrix &m) {
    GLuint viewMatrix = glGetUniformLocation(program, "viewMatrix");
    glUniformMatrix4fv(viewMatrix, /*num*/1, /*row major*/GL_TRUE, m.data());
}

void Renderer::setProjection() {
    GLuint perspectiveMatrix = glGetUniformLocation(program, "perspectiveMatrix");
    PerspectiveMatrix m(0.5f, 4.5f, 30, viewWidth / viewHeight);
    glUniformMatrix4fv(perspectiveMatrix, /*num*/1, /*row major*/GL_TRUE, m.data());
}

void Renderer::setColor(float r, float g, float b, float a) {
    GLuint mainColor = glGetUniformLocation(program, "mainColor");
    glUniform4f(mainColor, r, g, b, a);
}

void Renderer::reshape(GLint w, GLint h) {
    viewWidth = w;
    viewHeight = h;
}
