$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_shieldParams; // x: energy, y: time, z: distortion, w: thickness

float noise(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    float energy = u_shieldParams.x;
    float time = u_shieldParams.y;
    float distortion = u_shieldParams.z;
    float thickness = u_shieldParams.w;
    
    // Distance from edge (for shield boundary)
    vec2 centeredUV = v_texcoord0.xy - 0.5;
    float dist = length(centeredUV);
    
    // Create shield boundary
    float shieldEdge = abs(dist - 0.4);
    float shieldMask = 1.0 - smoothstep(0.0, thickness, shieldEdge);
    
    // Add electrical distortion
    vec2 distortedUV = centeredUV + sin(time * 8.0 + dist * 20.0) * distortion * 0.02;
    float electricNoise = noise(distortedUV * 50.0 + time * 5.0);
    
    // Create electrical arc pattern
    float angle = atan2(centeredUV.y, centeredUV.x);
    float arcPattern = sin(angle * 3.0 + time * 4.0 + electricNoise * 6.28) * 0.5 + 0.5;
    
    // Combine shield with electrical effects
    float finalAlpha = shieldMask * energy * (0.6 + arcPattern * 0.4);
    
    // Electric blue-white color with energy boost
    vec3 shieldColor = mix(vec3(0.3, 0.7, 1.0), vec3(1.0, 1.0, 1.0), energy);
    vec3 finalColor = shieldColor * (1.0 + energy * 2.0);
    
    gl_FragColor = vec4(finalColor, finalAlpha * v_color0.a);
}
