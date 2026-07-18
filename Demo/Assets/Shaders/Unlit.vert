#version 410 core
layout(location = 0) in vec3 a_Pos;
layout(location = 2) in vec2 a_TexCoord;
uniform mat4 u_MVP;
out vec2 v_TexCoord;
void main() {
    gl_Position = u_MVP * vec4(a_Pos, 1.0);
    v_TexCoord = a_TexCoord;
}