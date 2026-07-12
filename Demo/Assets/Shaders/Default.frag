#version 410 core
in vec2 vTexCoord;
in vec3 vNormal;

// StandardMaterial uniforms
uniform sampler2D u_MainTex;
uniform sampler2D u_BumpMap;
uniform vec4 u_Color = vec4(1.0);
uniform float u_Metallic = 0.0;
uniform float u_Smoothness = 0.5;

// Lighting
uniform vec4 u_AmbientColor = vec4(0.3, 0.3, 0.35, 1.0);
uniform vec3 u_LightDir = vec3(0, 1, 0);
uniform vec4 u_LightColor = vec4(1, 1, 1, 1);

out vec4 fragColor;

void main() {
    // Albedo
    vec4 albedo = texture(u_MainTex, vTexCoord) * u_Color;
    
    // Simple directional light
    vec3 normal = normalize(vNormal);
    float NdotL = max(dot(normal, normalize(u_LightDir)), 0.0);
    
    vec3 diffuse = albedo.rgb * u_LightColor.rgb * NdotL;
    vec3 ambient = albedo.rgb * u_AmbientColor.rgb;
    
    vec3 finalColor = ambient + diffuse;
    fragColor = vec4(finalColor, albedo.a);
}