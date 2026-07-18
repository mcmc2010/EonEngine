#version 410 core
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_WorldPos;
out mat3 v_TBN;

void main() {
    vec4 worldPos = u_Model * vec4(a_Pos, 1.0);
    gl_Position = u_MVP * vec4(a_Pos, 1.0);
    v_TexCoord = a_TexCoord;
    v_WorldPos = worldPos.xyz;
    
    // Normal: use normal matrix (transpose of inverse)
    vec3 N = normalize(u_NormalMatrix * a_Normal);
    
    // Tangent: use model matrix 3x3 (NOT normal matrix)
    vec3 T = normalize(mat3(u_Model) * a_Tangent);
    
    // Gram-Schmidt orthogonalization
    T = normalize(T - dot(T, N) * N);
    
    // Bitangent
    vec3 B = cross(N, T);
    
    // TBN matrix: tangent space -> world space
    v_TBN = mat3(T, B, N);
    v_Normal = N;
}