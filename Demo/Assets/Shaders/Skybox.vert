#version 410 core
uniform mat4 uVP;
out vec3 vTexCoord;

void main() {
    // Fullscreen triangle (3 vertices, no VBO needed)
    vec2 positions[3] = vec2[](
        vec2(-1, -1),
        vec2( 3, -1),
        vec2(-1,  3)
    );

    vec2 pos = positions[gl_VertexID];
    gl_Position = vec4(pos, 1.0, 1.0);

    // Reconstruct cubemap direction
    mat4 invVP = inverse(uVP);
    vec4 worldPos = invVP * vec4(pos, 1.0, 1.0);
    vTexCoord = worldPos.xyz / worldPos.w;
}