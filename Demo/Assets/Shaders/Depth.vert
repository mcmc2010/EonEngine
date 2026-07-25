#version 410 core
layout(location = 0) in vec3 a_Pos;

uniform mat4 u_LightVP;
uniform mat4 u_Model;

void main() {
    gl_Position = u_LightVP * u_Model * vec4(a_Pos, 1.0);
}