#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

uniform mat4 modelView;
uniform mat4 normalMatrix;
uniform mat4 projMatrix;

out vec4 theNormal;
out vec4 eyeCoord;

void main() {
    eyeCoord = modelView * position;
    gl_Position = projMatrix * eyeCoord;
    theNormal = normalMatrix * normal;
}
