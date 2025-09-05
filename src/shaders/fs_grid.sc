$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_gridParams; // x: gridSize, y: lineWidth, z: glowIntensity, w: time
uniform vec4 u_gridPlayerPos; // x: playerX (0-1), y: playerY (0-1), z: distortRadius, w: distortStrength

void main()
{
    float gridSize = u_gridParams.x;
    float lineWidth = u_gridParams.y;
    float glowIntensity = u_gridParams.z;
    float time = u_gridParams.w;
    
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
    
    // Add subtle pulse animation
    float pulse = 0.8 + 0.2 * sin(time * 2.0);
    
    float totalGlow = gridValue * glowIntensity * pulse;
    
    vec3 finalColor = v_color0.rgb * totalGlow;
    float finalAlpha = totalGlow * v_color0.a;
    
    gl_FragColor = vec4(finalColor, finalAlpha);
}
