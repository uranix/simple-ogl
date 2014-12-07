#version 330

layout(triangles) in;

layout(triangle_strip, max_vertices=3) out;

in vec4 eyeCoord[];
in vec4 theNormal[];

out vec3 vertexNormal;
flat out vec3 faceNormal;

void main() {
	vec3 p1 = eyeCoord[1].xyz - eyeCoord[0].xyz;
	vec3 p2 = eyeCoord[2].xyz - eyeCoord[0].xyz;

	vec3 p1xp2 = cross(p1, p2);

	for (int i = 0; i < gl_in.length (); i++) {
		gl_Position = gl_in[i].gl_Position;
		vertexNormal.xyz = theNormal[i].xyz;
		faceNormal = p1xp2;
		EmitVertex();
	}

	EndPrimitive();
}
