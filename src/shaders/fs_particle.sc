$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_particleParams; // x: intensity, y: fadeType, z: time, w: size

void main()
{
    float intensity = u_particleParams.x;
    float fadeType = u_particleParams.y;
    float time = u_particleParams.z;
    float size = u_particleParams.w;
    
    // Distance from center (for circular particles)
    vec2 centeredUV = v_texcoord0.xy - 0.5;
    float dist = length(centeredUV);
    
    // Create radial gradient for particle core
    float coreAlpha = 1.0 - smoothstep(0.0, 0.2, dist);
    
    // Create aggressive bright center with strong falloff for bloom effect
    float glowAlpha = 1.0 / (1.0 + dist * dist * 4.0);
    
    // Combine core and aggressive glow
    float totalAlpha = max(coreAlpha, glowAlpha * 0.8) * intensity;
    
    // Add electric spark effect for explosions
    if (fadeType > 0.5) {
        float angle = atan2(centeredUV.y, centeredUV.x);
        float sparkNoise = sin(angle * 6.0 + time * 10.0) * 0.5 + 0.5;
        totalAlpha *= (0.7 + sparkNoise * 0.3);
    }
    
    // Aggressive color boost for strong neon bloom effect
    vec3 boostedColor = v_color0.rgb * (2.5 + intensity * 1.5);
    
    gl_FragColor = vec4(boostedColor, totalAlpha * v_color0.a);
}
