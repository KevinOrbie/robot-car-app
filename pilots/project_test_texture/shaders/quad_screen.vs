#version 330 core

// Input Data
layout (location = 0) in vec3 quadPos; 
layout (location = 1) in vec2 quadTexCoord;

// Uniform Data
// uniform float time;
// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

// Pipeline Data
out vec2 TexCoord;

/* ------------------------ Functions ------------------------ */
// None


/* ----------------------- Entry Point ----------------------- */
void main()
{   
    gl_Position = vec4(quadPos, 1.0);
    TexCoord = quadTexCoord;
}
