$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_vaporParams; // x: time, y: noise_scale, z: turbulence, w: fade_factor

// Simple noise function for smoky effect
float noise(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// Fractal noise for more complex patterns
float fractalNoise(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < 4; i++) {
        value += amplitude * noise(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    
    return value;
}

void main()
{
    float time = u_vaporParams.x;
    float noiseScale = u_vaporParams.y;
    float turbulence = u_vaporParams.z;
    float fadeFactor = u_vaporParams.w;
    
    // Get position along the trail (0.0 = start, 1.0 = end)
    float trailPosition = v_texcoord0.x;
    
    // Distance from center of trail width
    float distanceFromCenter = abs(v_texcoord0.y - 0.5) * 2.0;
    
    // Create animated noise for smoky movement - make it much more visible
    vec2 noiseCoord = v_texcoord0 * noiseScale * 5.0 + vec2(time * 0.3, time * 0.2);
    float smokeNoise = fractalNoise(noiseCoord);
    
    // Add turbulence based on trail position (more turbulent towards the end)
    float turbulenceAmount = trailPosition * turbulence;
    smokeNoise += fractalNoise(noiseCoord * 2.0 + time * 0.5) * turbulenceAmount;
    
    // Create much more visible edges with stronger noise influence
    float edgeFade = 1.0 - smoothstep(0.1, 0.8, distanceFromCenter + smokeNoise * 0.2);
    
    // Stronger fade trail based on position (newer parts more opaque)
    float ageFade = 1.0 - smoothstep(0.0, 0.8, trailPosition * fadeFactor * 0.5);
    
    // Create stronger pulsing effect for more dynamic smoke
    float pulse = 0.7 + 0.5 * sin(time * 3.0 + trailPosition * 4.0);
    
    // Much stronger alpha values for visibility
    float totalAlpha = edgeFade * ageFade * pulse * v_color0.a * 3.0;
    totalAlpha = clamp(totalAlpha, 0.0, 1.0);
    
    // Preserve the base orange color with minimal variation
    vec3 smokeColor = v_color0.rgb;
    float colorVariation = smokeNoise * 0.05; // Reduced from 0.2 to keep more orange
    smokeColor = mix(smokeColor, smokeColor * 1.1, colorVariation); // Reduced brightness variation
    
    // Subtle core brightness that preserves orange color
    float coreBrightness = 1.0 + (1.0 - distanceFromCenter) * 0.3; // Much more subtle
    smokeColor *= coreBrightness;
    
    gl_FragColor = vec4(smokeColor, totalAlpha);
}
