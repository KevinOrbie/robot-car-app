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
    FragColor = vec4(texture(screenTexture, TexCoord).xxx, 1.0);
}
