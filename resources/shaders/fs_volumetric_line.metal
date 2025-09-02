#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct FragmentInput 
{
    float4 position [[position]];
    float4 color;
    float2 texcoord;
    float2 lineCenter;
};

struct Uniforms 
{
    float4x4 u_modelViewProj;
    float4   u_bloomParams; // x: bloomIntensity, y: bloomRadius, z: bloomThreshold, w: lineThickness
};

fragment float4 fs_main(FragmentInput in [[stage_in]],
                       constant Uniforms& uniforms [[buffer(0)]])
{
    // Extract bloom parameters
    float bloomIntensity = uniforms.u_bloomParams.x;
    float bloomRadius = uniforms.u_bloomParams.y;
    float bloomThreshold = uniforms.u_bloomParams.z;
    float lineThickness = uniforms.u_bloomParams.w;
    
    // Calculate distance from line center
    // For a line rendered as a quad, we use the cross-line distance (perpendicular to line direction)
    float distanceFromCenter = abs(in.texcoord.y - 0.5) * 2.0; // Normalize to 0-1 range
    
    // Core line intensity - sharp falloff at line edges
    float coreRadius = lineThickness * 0.5;
    float coreAlpha = 1.0 - smoothstep(0.0, coreRadius, distanceFromCenter);
    
    // Bloom effect - exponential falloff for soft glow
    float bloomDistance = distanceFromCenter / bloomRadius;
    float bloomAlpha = bloomIntensity * exp(-bloomDistance * bloomDistance * 4.0);
    
    // Combine core and bloom with proper energy conservation
    float totalAlpha = coreAlpha + bloomAlpha * (1.0 - coreAlpha);
    totalAlpha = clamp(totalAlpha, 0.0, 1.0);
    
    // Apply brightness boost for lines above bloom threshold
    float brightness = max(in.color.r, max(in.color.g, in.color.b));
    float bloomBoost = 1.0 + (brightness > bloomThreshold ? bloomIntensity * 0.5 : 0.0);
    
    // Calculate final color with bloom
    float3 finalRGB = in.color.rgb * bloomBoost;
    float finalAlpha = totalAlpha * in.color.a;
    
    // Add extra glow for very bright colors
    if (brightness > bloomThreshold) {
        float glowFactor = (brightness - bloomThreshold) / (1.0 - bloomThreshold);
        finalRGB += finalRGB * glowFactor * bloomIntensity * 0.3;
    }
    
    return float4(finalRGB, finalAlpha);
}
