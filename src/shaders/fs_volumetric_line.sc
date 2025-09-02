$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_bloomParams; // x: bloomIntensity, y: bloomRadius, z: bloomThreshold, w: lineThickness

void main()
{
    // Extract bloom parameters
    float bloomIntensity = u_bloomParams.x;
    float bloomRadius = u_bloomParams.y;
    float bloomThreshold = u_bloomParams.z;
    float lineThickness = u_bloomParams.w;
    
    // Calculate distance from line center (0.0 = center, 1.0 = edge)
    // v_texcoord0.y goes from 0.0 to 1.0 across the line width
    float distanceFromCenter = abs(v_texcoord0.y - 0.5) * 2.0;
    
    // Core line - solid center with reasonable falloff
    float coreAlpha = 1.0 - smoothstep(0.0, 0.6, distanceFromCenter);
    
    // Bloom effect - softer glow around the line
    float bloomFalloff = 1.0 - distanceFromCenter;
    float bloomAlpha = bloomIntensity * bloomFalloff * bloomFalloff;
    
    // Combine core and bloom
    float totalAlpha = max(coreAlpha, bloomAlpha);
    
    // Moderate intensity boost - not too overwhelming
    totalAlpha = clamp(totalAlpha * 1.5, 0.0, 1.0);
    
    // Use color with moderate brightness boost
    vec3 finalRGB = v_color0.rgb * 1.2;
    float finalAlpha = totalAlpha;
    
    gl_FragColor = vec4(finalRGB, finalAlpha);
}
