#version 410 core
in vec3 vTexCoord;
uniform samplerCube u_Cubemap;
uniform vec3 u_HorizonColor = vec3(0.7, 0.75, 0.8); // Horizon color
uniform float u_HorizonExponent = 2.0;               // Horizon falloff
out vec4 fragColor;

void main() {
    vec4 skyColor = texture(u_Cubemap, vTexCoord);
    vec3 dir = normalize(vTexCoord);
    
    // Gradient from sky to horizon based on vertical direction
    float horizonFactor = pow(1.0 - abs(dir.y), u_HorizonExponent);
    vec3 finalColor = mix(skyColor.rgb, u_HorizonColor, horizonFactor * 0.5);
    
    fragColor = vec4(finalColor, 1.0);
}