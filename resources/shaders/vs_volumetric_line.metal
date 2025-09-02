#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VertexInput 
{
    float2 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
    float2 texcoord [[attribute(2)]]; // UV coordinates for distance calculation
};

struct VertexOutput 
{
    float4 position [[position]];
    float4 color;
    float2 texcoord;
    float2 lineCenter; // Center of the line for distance calculations
};

struct Uniforms 
{
    float4x4 u_modelViewProj;
    float4   u_bloomParams; // x: bloomIntensity, y: bloomRadius, z: bloomThreshold, w: lineThickness
};

vertex VertexOutput vs_main(VertexInput in [[stage_in]], 
                            constant Uniforms& uniforms [[buffer(0)]])
{
    VertexOutput out;
    out.position = uniforms.u_modelViewProj * float4(in.position, 0.0, 1.0);
    out.color = in.color;
    out.texcoord = in.texcoord;
    
    // Calculate line center based on UV coordinates
    // For a line quad, center should be at (0.5, 0.5) in texture space
    out.lineCenter = float2(0.5, 0.5);
    
    return out;
}
