#version 330

layout(triangles) in;

layout(triangle_strip, max_vertices=3) out;

in vec4 eyeCoord[];
in vec4 theNormal[];

out vec3 vertexNormal;
uniform int smoothNormals;

void main() {
	vec3 p1xp2;

	if (smoothNormals == 0) {
		vec3 p1 = eyeCoord[1].xyz - eyeCoord[0].xyz;
		vec3 p2 = eyeCoord[2].xyz - eyeCoord[0].xyz;

		p1xp2 = cross(p1, p2);
	}

	for (int i = 0; i < gl_in.length (); i++) {
		gl_Position = gl_in[i].gl_Position;
		vertexNormal.xyz = smoothNormals == 1 ? theNormal[i].xyz : p1xp2;
		EmitVertex();
	}

	EndPrimitive();
}
