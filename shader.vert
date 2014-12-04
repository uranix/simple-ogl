#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

smooth out vec4 theColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

void main() {
	vec4 modelCoord = modelMatrix * position;
	vec4 eyeCoord = viewMatrix * modelCoord;
	gl_Position = perspectiveMatrix * eyeCoord;
	theColor = color;
}
