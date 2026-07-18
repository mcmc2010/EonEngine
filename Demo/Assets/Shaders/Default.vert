#version 410 core
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;  // 预计算的法线矩阵

out vec2 v_TexCoord;
out vec3 v_Normal;

void main() {
    gl_Position = u_MVP * vec4(a_Pos, 1.0);
    v_TexCoord = a_TexCoord;
    v_Normal = u_NormalMatrix * a_Normal;  // 直接使用预计算矩阵
}