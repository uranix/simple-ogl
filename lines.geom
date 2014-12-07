#version 330

layout(lines) in;

layout(line_strip, max_vertices=2) out;

in vec4 theNormal [];

out vec3 vertexNormal;
flat out vec3 faceNormal;

void main() {
	for (int i = 0; i < gl_in.length (); i++) {
		gl_Position = gl_in[i].gl_Position;
		vertexNormal.xyz = vec3(1, 0, 0);
		faceNormal.xyz = vec3(1, 0, 0);
		EmitVertex();
	}

	EndPrimitive();
}
