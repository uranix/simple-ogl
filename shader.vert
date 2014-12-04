#version 330

layout(location = 0) in vec4 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 perspectiveMatrix;

void main() {
	vec4 modelCoord = modelMatrix * position;
	vec4 eyeCoord = viewMatrix * modelCoord;
	gl_Position = perspectiveMatrix * eyeCoord;
}
