#version 330

uniform vec4 mainColor;
uniform float lightIntens;
uniform int shadePhong;
uniform float specularity;

in vec3 vertexNormal;

out vec4 outputColor;
void main() {
    vec4 clear = vec4(1, 1, 1, 1);

    vec3 norm = shadePhong == 1 ? normalize(vertexNormal) : vertexNormal;

    vec3 lightdir = normalize(vec3(1, -1, 1));

    vec3 lightColor = vec3(1, 1, 1);

    float diffuse = clamp(dot(lightdir, norm), 0, 1);

    float alpha = 10.0f;
    float specular = pow(clamp(2 * dot(lightdir, norm) * norm.z - lightdir.z, 0, 1), alpha);

    vec3 light = lightColor * mix(diffuse, specular, specularity);

    outputColor.xyz = mix(mainColor.xyz, light, lightIntens);
    outputColor.w = 1;
}
