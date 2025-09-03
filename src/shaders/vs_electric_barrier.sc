$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0

#include <bgfx_shader.sh>

void main()
{
    // Transform vertex position
    gl_Position = mul(u_modelViewProj, vec4(a_position.xy, 0.0, 1.0));
    
    // Pass through vertex color
    v_color0 = a_color0;
    
    // Pass through texture coordinates (will contain line coordinates)
    v_texcoord0 = a_texcoord0;
}
