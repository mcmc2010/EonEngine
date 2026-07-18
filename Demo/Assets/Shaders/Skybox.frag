#version 410 core
in vec3 vTexCoord;
uniform samplerCube u_Cubemap;
uniform vec3 u_HorizonColor = vec3(0.7, 0.75, 0.8);
uniform float u_HorizonExponent = 2.0;
uniform float u_HorizonIntensity = 0.8;
out vec4 fragColor;

void main() {
    vec4 skyColor = texture(u_Cubemap, vTexCoord);
    vec3 dir = normalize(vTexCoord);
    
    // Horizon gradient (upper hemisphere only)
    float horizonFactor = pow(1.0 - max(0.0, dir.y), u_HorizonExponent);
    vec3 finalColor = mix(skyColor.rgb, u_HorizonColor, horizonFactor * u_HorizonIntensity);
    
    fragColor = vec4(finalColor, 1.0);
}