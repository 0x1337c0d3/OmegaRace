$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_warpParams; // x: time, y: intensity, z: unused, w: unused

void main()
{
    // Extract parameters
    float time = u_warpParams.x;
    float intensity = u_warpParams.y;
    
    // Simple fade effect: fade to black then fade back to transparent
    // intensity goes from 0.5 to 1.0 to 0.5 over the transition
    
    float fadeAmount;
    if (intensity < 0.75) {
        // First half: fade from transparent to black (0.5 -> 0.75)
        fadeAmount = (intensity - 0.5) * 4.0; // Maps 0.5-0.75 to 0.0-1.0
    } else {
        // Second half: fade from black to transparent (0.75 -> 1.0+)
        fadeAmount = 1.0 - ((intensity - 0.75) * 4.0); // Maps 0.75-1.0 to 1.0-0.0
    }
    
    // Clamp fadeAmount to valid range
    fadeAmount = clamp(fadeAmount, 0.0, 1.0);
    
    // Simple black color with alpha based on fade
    vec3 blackColor = vec3(0.0, 0.0, 0.0);
    float alpha = fadeAmount;
    
    gl_FragColor = vec4(blackColor, alpha);
}
