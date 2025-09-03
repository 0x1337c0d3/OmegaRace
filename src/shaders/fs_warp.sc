$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_warpParams; // x: time, y: intensity, z: waveFreq, w: ringCount

// Constants for the warp effect
#define PI 3.14159265359
#define TWO_PI 6.28318530718

// Noise function for added distortion
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

void main()
{
    // Extract warp parameters
    float time = u_warpParams.x;
    float intensity = u_warpParams.y;
    float waveFreq = u_warpParams.z;
    float ringCount = u_warpParams.w;
    
    // Center UV coordinates (-0.5 to 0.5)
    vec2 uv = v_texcoord0 - 0.5;
    
    // Calculate distance from center and angle
    float dist = length(uv);
    float angle = atan2(uv.y, uv.x);
    
    // Screen aspect ratio adjustment for proper circular rings
    float aspect = 1024.0 / 768.0;
    vec2 aspectUV = vec2(uv.x * aspect, uv.y);
    float aspectDist = length(aspectUV);
    
    // === CONCENTRIC RINGS ===
    float ringPhase = aspectDist * ringCount - time * 2.0;
    float ringWave = sin(ringPhase) * 0.5 + 0.5;
    
    // Ring intensity based on distance from ring center with enhanced fading
    float ringDistance = fract(ringPhase / TWO_PI);
    float ringAlpha = 1.0 - smoothstep(0.0, 0.4, abs(ringDistance - 0.5) * 2.0);
    
    // Enhanced ring fade-in/out based on distance and intensity
    float ringDistanceFade = smoothstep(0.8, 0.2, aspectDist); // Fade out toward edges
    ringAlpha *= ringDistanceFade;
    
    // Animated ring brightness - more subtle with better fading
    float ringBrightness = ringWave * ringAlpha * intensity * 0.7;
    
    // === RADIAL SPOKES ===
    float spokeCount = 16.0;
    float spokePhase = angle * spokeCount / TWO_PI + time;
    float spokeWave = sin(spokePhase * PI) * 0.5 + 0.5;
    
    // Spoke intensity diminishes with distance from center
    float spokeIntensity = spokeWave * (1.0 - smoothstep(0.1, 0.8, aspectDist)) * intensity * 0.4;
    
    // === RADIAL WAVE DISTORTION ===
    float waveDistortion = sin(aspectDist * 20.0 * waveFreq - time * 3.0) * 0.03 * intensity;
    float distortedDist = aspectDist + waveDistortion;
    
    // Secondary wave pattern - more subtle
    float secondaryWave = sin(distortedDist * 15.0 - time * 4.0 + angle * 3.0) * 0.5 + 0.5;
    float secondaryIntensity = secondaryWave * intensity * 0.25;
    
    // === NOISE AND TURBULENCE ===
    vec2 noiseUV = uv * 8.0 + time * 0.1;
    float noiseValue = noise(noiseUV) * 0.15;
    
    // === PULSING CENTER ===
    float centerPulse = 1.0 - smoothstep(0.0, 0.15, aspectDist);
    float pulseIntensity = centerPulse * (0.6 + 0.15 * sin(time * 5.0)) * intensity;
    
    // === COLOR CYCLING ===
    // Monochromatic approach - subtle white/blue tones only
    float intensity_fade = time * 0.2 + aspectDist * 0.5;
    
    // Clean, minimal color palette - mostly white with subtle blue tint
    vec3 neonColor = vec3(0.9, 0.95, 1.0); // Clean white with slight blue tint
    
    // No saturation boost - keep it clean and minimal
    
    // === COMBINE ALL EFFECTS ===
    float totalIntensity = ringBrightness + spokeIntensity + secondaryIntensity + pulseIntensity + noiseValue;
    
    // Apply intensity with exponential falloff for dramatic effect
    totalIntensity = pow(clamp(totalIntensity, 0.0, 1.0), 0.7);
    
    // Edge fade to prevent harsh cutoffs at screen borders
    float edgeFade = 1.0 - smoothstep(0.6, 1.0, aspectDist);
    totalIntensity *= edgeFade;
    
    // === TEMPORAL FADE IN/OUT ===
    // Simple fade based on warp intensity parameter (should be 0.0 to 1.0)
    // Make sure effect is visible when intensity > 0
    float warpFade = smoothstep(0.0, 0.1, intensity); // Simple fade-in only
    totalIntensity *= warpFade;
    
    // Gentle time-based breathing for organic feel
    float timeFade = sin(time * 1.5) * 0.05 + 0.95; // Very subtle pulsing
    totalIntensity *= timeFade;
    
    // === FINAL COLOR OUTPUT ===
    vec3 finalColor = neonColor * totalIntensity * v_color0.rgb;
    float finalAlpha = totalIntensity * v_color0.a;
    
    // Simple fade for smooth entry (only fade-in, no fade-out to break effect)
    float globalFade = smoothstep(0.0, 0.05, intensity); // Very quick fade-in
    finalColor *= globalFade;
    finalAlpha *= globalFade;
    
    // Subtle brightness for understated warp effect
    finalColor *= 1.0;
    
    gl_FragColor = vec4(finalColor, finalAlpha);
}
