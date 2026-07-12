#version 410 core
in vec2 vTexCoord;
uniform sampler2D u_MainTex;
out vec4 fragColor;
void main() {
    fragColor = texture(u_MainTex, vTexCoord);
}