$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_gridParams; // x: gridSize, y: lineWidth, z: glowIntensity, w: time
uniform vec4 u_gridPlayerPos; // x: playerX (0-1), y: playerY (0-1), z: distortRadius, w: distortStrength
uniform vec4 u_warpParams; // x: warpIntensity (0.0 = normal, 1.0 = full surge), y: unused, z: unused, w: unused

void main()
{
    float gridSize = u_gridParams.x;
    float lineWidth = u_gridParams.y;
    float glowIntensity = u_gridParams.z;
    float time = u_gridParams.w;
    
    float warpIntensity = u_warpParams.x; // 0.0 = normal, 1.0 = full electrical surge
    
    vec2 playerPos = u_gridPlayerPos.xy;
    float distortRadius = u_gridPlayerPos.z;
    float distortStrength = u_gridPlayerPos.w;
    
    // Apply grid distortion around player position (classic Geometry Wars effect)
    vec2 gridUV = v_texcoord0.xy;
    
    // Calculate distance from current pixel to player
    vec2 toPlayer = gridUV - playerPos;
    float distToPlayer = length(toPlayer * vec2(800.0, 600.0)); // Scale by screen dimensions
    
    // Apply radial distortion around player or wave distortion during dissolution
    if (distToPlayer < distortRadius && distortStrength > 0.0) {
        float distortFactor = 1.0 - (distToPlayer / distortRadius);
        distortFactor = distortFactor * distortFactor; // Smoother falloff
        
        // For wave distortion (during dissolution), create vertical wave effect
        if (distortRadius > 200.0) { // Wave mode (large radius indicates wave)
            // Create vertical wave distortion with horizontal propagation
            float waveX = gridUV.x - playerPos.x; // Distance from wave center
            float waveInfluence = exp(-abs(waveX) * 8.0); // Sharp wave front
            
            // Create vertical oscillation that varies with Y position
            float waveOffset = sin(gridUV.y * 15.0 + time * 6.0) * waveInfluence * distortStrength * 0.2;
            gridUV.x += waveOffset;
            
            // Add radial expansion from wave center
            vec2 radialDistortion = normalize(toPlayer) * waveInfluence * distortStrength * 0.15;
            gridUV += radialDistortion;
        } else {
            // Normal player distortion
            vec2 distortion = normalize(toPlayer) * distortFactor * distortStrength * 0.1;
            gridUV += distortion;
        }
    }
    
    // Create grid pattern with distorted UV coordinates
    vec2 gridCoords = gridUV * gridSize;
    vec2 gridFrac = fract(gridCoords);
    
    // Create grid lines using step functions
    float lineX = step(gridFrac.x, lineWidth) + step(1.0 - lineWidth, gridFrac.x);
    float lineY = step(gridFrac.y, lineWidth) + step(1.0 - lineWidth, gridFrac.y);
    
    float gridValue = clamp(lineX + lineY, 0.0, 1.0);
    
    // Add subtle electrical wave effect with dramatic surge capability
    float surgeMultiplier = 1.0 + warpIntensity * 2.0; // Normal = 1.0, Surge = 9.0
    float timeMultiplier = 1.0 + warpIntensity * 4.0;  // Gentler speed increase during surge (was 15.0)
    float surgedTime = time * timeMultiplier;
    
    // Create multiple wave patterns with surge-enhanced speeds and amplitudes
    float wave1 = sin(gridUV.x * 8.0 + surgedTime * 1.2) * (0.15 * surgeMultiplier);
    float wave2 = sin(gridUV.y * 12.0 - surgedTime * 0.8) * (0.1 * surgeMultiplier);
    float wave3 = sin((gridUV.x + gridUV.y) * 6.0 + surgedTime * 1.5) * (0.08 * surgeMultiplier);
    
    // Enhanced electrical noise during surge - slower sparkle effect
    float electricalNoise = sin(gridUV.x * 40.0 + surgedTime * 1.5) * sin(gridUV.y * 30.0 + surgedTime * 1.0) * (0.05 * surgeMultiplier);
    
    // Add additional sparkle surge effects - much slower for sparkle feel
    float surgeEffect = 0.0;
    if (warpIntensity > 0.1) {
        surgeEffect = sin(gridUV.x * 100.0 + surgedTime * 6.0) * sin(gridUV.y * 80.0 + surgedTime * 4.0) * warpIntensity * 0.3;
        surgeEffect += sin((gridUV.x + gridUV.y) * 60.0 + surgedTime * 5.0) * warpIntensity * 0.2;
    }
    
    // Combine waves with surge effects
    float electricalWave = (wave1 + wave2 + wave3 + electricalNoise + surgeEffect);
    
    // During surge, allow much higher intensity ranges
    float minIntensity = 0.5 - (warpIntensity * 0.3); // Can get darker during surge
    float maxIntensity = 1.3 + (warpIntensity * 2.0); // Much brighter during surge
    electricalWave = clamp(1.0 + electricalWave, minIntensity, maxIntensity);
    
    // Apply the electrical wave more strongly to the grid lines
    float totalGlow = gridValue * glowIntensity * electricalWave;
    
    vec3 finalColor = v_color0.rgb * totalGlow;
    float finalAlpha = totalGlow * v_color0.a;
    
    gl_FragColor = vec4(finalColor, finalAlpha);
}
