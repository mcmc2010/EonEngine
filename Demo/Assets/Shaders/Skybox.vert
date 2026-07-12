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
    gl_Position = vec4(pos, 1.0, 1.0); // z=1.0 = far plane in NDC

    // Reconstruct cubemap direction from screen position
    mat4 invVP = inverse(uVP);
    vec4 worldPos = invVP * vec4(pos, 1.0, 1.0); // z=1.0 for far plane
    vTexCoord = normalize(worldPos.xyz);
}