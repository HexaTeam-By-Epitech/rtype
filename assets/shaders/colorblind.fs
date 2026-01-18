#version 330

// Input from vertex shader
in vec2 fragTexCoord;
in vec4 fragColor;

// Output
out vec4 finalColor;

// Uniforms
uniform sampler2D texture0;
uniform int filterType;  // 0=None, 1=Protanopia, 2=Deuteranopia, 3=Tritanopia, 4=Monochromacy

// Color transformation matrices for different types of color blindness
// These matrices simulate how colors appear to people with different types of color vision deficiency

// Protanopia (red-blind) - reduced sensitivity to red light
mat3 protanopiaMatrix = mat3(
    0.567, 0.433, 0.000,
    0.558, 0.442, 0.000,
    0.000, 0.242, 0.758
);

// Deuteranopia (green-blind) - reduced sensitivity to green light
mat3 deuteranopiaMatrix = mat3(
    0.625, 0.375, 0.000,
    0.700, 0.300, 0.000,
    0.000, 0.300, 0.700
);

// Tritanopia (blue-blind) - reduced sensitivity to blue light
mat3 tritanopiaMatrix = mat3(
    0.950, 0.050, 0.000,
    0.000, 0.433, 0.567,
    0.000, 0.475, 0.525
);

// Monochromacy (complete color blindness) - grayscale using luminance weights
vec3 monochromaticWeights = vec3(0.299, 0.587, 0.114);

void main()
{
    // Sample the texture
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 color = texelColor.rgb;
    
    if (filterType == 1) {
        // Protanopia
        color = protanopiaMatrix * color;
    } else if (filterType == 2) {
        // Deuteranopia
        color = deuteranopiaMatrix * color;
    } else if (filterType == 3) {
        // Tritanopia
        color = tritanopiaMatrix * color;
    } else if (filterType == 4) {
        // Monochromacy (grayscale)
        float gray = dot(color, monochromaticWeights);
        color = vec3(gray);
    }
    // filterType == 0: No transformation, use original color
    
    finalColor = vec4(color, texelColor.a) * fragColor;
}
