#version 410 core
layout(location = 0) in vec3 aPos;
uniform mat4 uVP;
out vec3 vTexCoord;
void main() {
    vec4 pos = uVP * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
    vTexCoord = aPos;
}