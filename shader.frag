#version 330

uniform vec4 mainColor;

out vec4 outputColor;
void main() {
	vec4 black = vec4(0, 0, 0, 1);
	outputColor = mix(mainColor, black, gl_FragCoord.z);
}
