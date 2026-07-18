#version 410 core
in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_WorldPos;
in mat3 v_TBN;

// StandardMaterial uniforms
uniform sampler2D u_MainTex;
uniform sampler2D u_NormalMap;
uniform vec4 u_Color = vec4(1.0);
uniform float u_Metallic = 0.0;
uniform float u_Smoothness = 0.5;

// Lighting
uniform vec4 u_AmbientColor = vec4(0.3, 0.3, 0.35, 1.0);
uniform vec3 u_LightDir = vec3(0, 1, 0);
uniform vec4 u_LightColor = vec4(1, 1, 1, 1);
uniform vec3 u_CameraPos = vec3(0, 0, 0);

out vec4 fragColor;

void main() {
    // Albedo
    vec4 albedo = texture(u_MainTex, v_TexCoord) * u_Color;
    
    // Normal mapping: sample and transform from tangent space to world space
    vec3 normalMap = texture(u_NormalMap, v_TexCoord).rgb * 2.0 - 1.0;
    vec3 normal = normalize(v_TBN * normalMap);
    
    // Diffuse (Lambertian)
    vec3 lightDir = normalize(u_LightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = albedo.rgb * u_LightColor.rgb * NdotL;
    
    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(u_CameraPos - v_WorldPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfDir), 0.0);
    float specPower = u_Smoothness * 128.0;
    float spec = pow(NdotH, specPower) * u_Smoothness;
    vec3 specular = u_LightColor.rgb * spec;
    
    // Ambient
    vec3 ambient = albedo.rgb * u_AmbientColor.rgb;
    
    // Final color
    vec3 finalColor = ambient + diffuse + specular;
    fragColor = vec4(finalColor, albedo.a);
}