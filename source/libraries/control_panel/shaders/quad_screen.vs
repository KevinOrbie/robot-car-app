#version 330 core

// Input Data
layout (location = 0) in vec3 quadPos; 
layout (location = 1) in vec2 quadTexCoord;

// Uniform Data
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Pipeline Data
out vec2 TexCoord;

/* ------------------------ Functions ------------------------ */
// None


/* ----------------------- Entry Point ----------------------- */
void main()
{   
    gl_Position = projection * view * model * vec4(quadPos, 1.0);
    TexCoord = quadTexCoord;
}
