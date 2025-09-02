#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VertexInput 
{
    float2 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
};

struct VertexOutput 
{
    float4 position [[position]];
    float4 color;
};

struct Uniforms 
{
    float4x4 u_modelViewProj;
};

vertex VertexOutput vs_main(VertexInput in [[stage_in]], 
                            constant Uniforms& uniforms [[buffer(0)]])
{
    VertexOutput out;
    out.position = uniforms.u_modelViewProj * float4(in.position, 0.0, 1.0);
    out.color = in.color;
    return out;
}
