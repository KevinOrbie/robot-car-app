#version 330 core

// Pipeline Data
in vec2 TexCoord;

// Texture Data
uniform sampler2D screenTexture;

// Output Color
out vec4 FragColor;


/* ------------------------ Functions ------------------------ */
// None


/* ----------------------- Entry Point ----------------------- */
void main()
{
    /* Display YUV directly */
    // FragColor = vec4(texture(screenTexture, TexCoord).xxx, 1.0);
    // FragColor = vec4(texture(screenTexture, TexCoord).yyy, 1.0);
    // FragColor = vec4(texture(screenTexture, TexCoord).zzz, 1.0);

    // Convert YUV to RGB
    vec4 texColor = texture(screenTexture, TexCoord);
    texColor = vec4(texColor.x * 255 - 16, texColor.y * 255 - 128, texColor.z * 255 - 128, 1.0);
    FragColor = vec4(
        1.164 * texColor.x                      + 1.596 * texColor.z, 
        1.164 * texColor.x - 0.392 * texColor.y - 0.813 * texColor.z, 
        1.164 * texColor.x + 2.017 * texColor.y,
        255.0
    ) / 255;
}
