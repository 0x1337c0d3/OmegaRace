#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct FragmentInput 
{
    float4 position [[position]];
    float4 color;
};

fragment float4 fs_main(FragmentInput in [[stage_in]])
{
    return in.color;
}
