#version 410 core
in vec3 vTexCoord;
uniform samplerCube u_Cubemap;
out vec4 fragColor;
void main() {
    fragColor = texture(u_Cubemap, vTexCoord);
}