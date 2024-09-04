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
    FragColor = texture(screenTexture, TexCoord); 
    // FragColor = vec4(TexCoord.x, 0.0, TexCoord.y, 1.0); 
}
