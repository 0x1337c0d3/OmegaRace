#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Bloom parameters
uniform float bloomIntensity = 1.0;
uniform float bloomRadius = 2.0;
uniform float bloomThreshold = 0.8;

void main()
{
    // Sample the texture
    vec4 texelColor = texture(texture0, fragTexCoord);
    
    // Calculate distance from line center (you'd need to pass this from vertex shader)
    // For now, we'll use a simple distance calculation
    vec2 center = vec2(0.5, 0.5);
    float distance = length(fragTexCoord - center);
    
    // Core line
    float coreAlpha = 1.0 - smoothstep(0.0, 0.1, distance);
    
    // Bloom effect - exponential falloff
    float bloomAlpha = bloomIntensity * exp(-distance * distance / (bloomRadius * bloomRadius));
    
    // Combine core and bloom
    float totalAlpha = coreAlpha + bloomAlpha;
    totalAlpha = clamp(totalAlpha, 0.0, 1.0);
    
    // Apply color and alpha
    vec3 finalRGB = texelColor.rgb * fragColor.rgb * colDiffuse.rgb;
    finalColor = vec4(finalRGB, totalAlpha * texelColor.a * fragColor.a * colDiffuse.a);
}
