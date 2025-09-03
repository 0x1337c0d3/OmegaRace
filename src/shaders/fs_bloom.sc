$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_bloomParams; // x: threshold, y: intensity, z: radius, w: samples

void main()
{
    float threshold = u_bloomParams.x;
    float intensity = u_bloomParams.y;
    float radius = u_bloomParams.z;
    
    // Simple bloom effect for post-processing
    vec3 originalColor = v_color0.rgb;
    
    // Check if pixel is bright enough for bloom
    float luminance = dot(originalColor, vec3(0.299, 0.587, 0.114));
    
    if (luminance > threshold) {
        // Apply bloom effect
        float bloomFactor = (luminance - threshold) / (1.0 - threshold);
        vec3 bloomColor = originalColor * intensity * bloomFactor;
        
        gl_FragColor = vec4(originalColor + bloomColor, v_color0.a);
    } else {
        gl_FragColor = v_color0;
    }
}
