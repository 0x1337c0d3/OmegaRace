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
    float coreAlpha = 1.0 - smoothstep(0.0, 0.4, distanceFromCenter);
    
    // Aggressive bloom effect - much stronger glow around the line
    float bloomFalloff = 1.0 - distanceFromCenter;
    float aggressiveBloomAlpha = bloomIntensity * 2.0 * bloomFalloff * bloomFalloff * bloomFalloff;
    
    // Combine core and aggressive bloom
    float totalAlpha = max(coreAlpha, aggressiveBloomAlpha);
    
    // Much more aggressive intensity boost
    totalAlpha = clamp(totalAlpha * 3.0, 0.0, 1.0);
    
    // Use color with aggressive brightness boost and bloom glow
    vec3 finalRGB = v_color0.rgb * (2.0 + bloomIntensity);
    float finalAlpha = totalAlpha;
    
    gl_FragColor = vec4(finalRGB, finalAlpha);
}
