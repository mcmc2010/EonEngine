#version 410 core
uniform mat4 u_InverseVP;
out vec3 vTexCoord;

void main() {
    // Fullscreen triangle (3 vertices, no VBO needed)
    vec2 positions[3] = vec2[](
        vec2(-1, -1),
        vec2( 3, -1),
        vec2(-1,  3)
    );

    vec2 pos = positions[gl_VertexID];
    gl_Position = vec4(pos, 1.0, 1.0); // z=1.0 far plane

    // Reconstruct cubemap direction using inverse VP (no translation)
    vec4 worldPos = u_InverseVP * vec4(pos, 1.0, 1.0);
    vTexCoord = worldPos.xyz; // No need to divide by w (far plane)
}