$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_electricParams; // x: time, y: pulse_speed, z: thickness, w: fade_time

// Constants for electric barrier effect
#define PI 3.14159265359
#define TWO_PI 6.28318530718

// High-quality noise functions for electric distortion
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

// Fractal noise for more complex electric patterns
float fbm(vec2 p) {
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
    // Extract electric barrier parameters
    float time = u_electricParams.x;
    float pulseSpeed = u_electricParams.y;
    float thickness = u_electricParams.z;
    float fadeTime = u_electricParams.w;
    
    // Get line coordinates from texture coordinates
    float lineProgress = v_texcoord0.x;    // 0.0 to 1.0 along line  
    float perpDistance = v_texcoord0.y;    // -1.0 to 1.0 perpendicular to line (will be set by CPU)
    
    // === ELECTRIC CORE PULSING ===
    // Create main pulsing intensity (matching original 15.0 pulse speed)
    float mainPulse = sin(time * pulseSpeed) * 0.5 + 0.5; // 0 to 1
    
    // Add secondary faster pulse for electric variation
    float fastPulse = sin(time * pulseSpeed * 2.5) * 0.3 + 0.7; // 0.4 to 1.0
    
    // Combine pulses for complex electric rhythm
    float combinedPulse = mainPulse * fastPulse;
    
    // === ELECTRIC NOISE AND DISTORTION ===
    // Create electric noise along the line
    vec2 noiseCoord = vec2(lineProgress * 8.0, time * 3.0);
    float electricNoise = fbm(noiseCoord);
    
    // Add perpendicular noise for width variation
    vec2 perpNoiseCoord = vec2(lineProgress * 12.0, perpDistance * 4.0 + time * 2.0);
    float perpNoise = noise(perpNoiseCoord) * 0.3;
    
    // === ELECTRIC ARC SIMULATION ===
    // Create random arc-like distortions along the line
    float arcFreq = 20.0;
    float arcPhase = lineProgress * arcFreq + time * 5.0;
    float arcNoise = sin(arcPhase) * noise(vec2(arcPhase * 0.1, time * 0.5));
    
    // Combine all noise sources
    float totalDistortion = electricNoise * 0.4 + perpNoise + arcNoise * 0.2;
    
    // === LINE CORE INTENSITY ===
    // Calculate distance from line center with noise distortion
    float distortedPerpDistance = abs(perpDistance) + totalDistortion * 0.1;
    
    // Create sharp electric core
    float coreIntensity = 1.0 - smoothstep(0.0, 0.2, distortedPerpDistance);
    
    // Create electric glow falloff
    float glowIntensity = 1.0 - smoothstep(0.0, 0.8, distortedPerpDistance);
    
    // === ELECTRIC SPARKS ===
    // Create random spark points along the line
    float sparkFreq = 15.0;
    float sparkPhase = lineProgress * sparkFreq + time * 8.0;
    float sparkTrigger = smoothstep(0.8, 1.0, noise(vec2(sparkPhase, time * 1.5)));
    
    // Spark intensity with random positioning
    float sparkIntensity = sparkTrigger * (1.0 - smoothstep(0.0, 0.4, abs(perpDistance)));
    
    // === COLOR CALCULATION ===
    // Electric blue/cyan colors with pulsing intensity (matching original)
    vec3 electricCore = vec3(
        0.2 + combinedPulse * 0.6,    // Red: 50-200 range (mapped to 0.2-0.8)
        0.6 + combinedPulse * 0.4,    // Green: 150-255 range (mapped to 0.6-1.0)
        1.0                           // Blue: Always full
    );
    
    vec3 electricGlow = vec3(
        0.4 + combinedPulse * 0.4,    // Red: 100-200 range (mapped to 0.4-0.8)
        0.78 + combinedPulse * 0.22,  // Green: 200-255 range (mapped to 0.78-1.0)
        1.0                           // Blue: Always full
    );
    
    // === COMBINE EFFECTS ===
    // Combine core and glow with spark overlay
    vec3 finalColor = electricCore * coreIntensity + electricGlow * glowIntensity * 0.6;
    
    // Add spark highlights
    finalColor += vec3(1.0, 1.0, 1.0) * sparkIntensity * 0.8;
    
    // Apply pulsing to overall brightness
    finalColor *= (0.7 + combinedPulse * 0.3);
    
    // === FADE OUT OVER TIME ===
    // Apply fade based on remaining time (matching original 0.5s duration)
    float fadeAlpha = smoothstep(0.0, 0.5, fadeTime);
    
    // Calculate final intensity
    float finalIntensity = (coreIntensity + glowIntensity * 0.6 + sparkIntensity) * fadeAlpha;
    
    // Apply vertex color and final modifications
    finalColor *= v_color0.rgb;
    float finalAlpha = finalIntensity * v_color0.a;
    
    // Ensure minimum visibility for the electric effect
    finalAlpha = max(finalAlpha, 0.1 * fadeAlpha);
    
    gl_FragColor = vec4(finalColor, finalAlpha);
}
