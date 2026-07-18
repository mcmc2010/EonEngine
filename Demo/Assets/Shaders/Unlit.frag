#version 410 core
in vec2 v_TexCoord;
uniform sampler2D u_MainTex;
out vec4 fragColor;
void main() {
    fragColor = texture(u_MainTex, v_TexCoord);
}