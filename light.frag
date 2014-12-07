#version 330

uniform vec4 mainColor;
uniform float lightIntens;
uniform int smoothNormals;

     in vec3 vertexNormal;
flat in vec3 faceNormal;

out vec4 outputColor;
void main() {
    vec4 clear = vec4(1, 1, 1, 1);

    vec3 norm = normalize(smoothNormals == 1 ? vertexNormal : faceNormal);

    vec3 lightdir = vec3(-1, 1, 0);

    vec3 lightColor = vec3(1, 1, 1);
    vec3 light = lightColor * clamp(dot(lightdir, norm), 0, 1);

    outputColor.xyz = mix(mainColor.xyz, light, lightIntens);
    outputColor.w = 1;
}
