#version 330 core

// Input Data
layout (location = 0) in vec3 vertex_pos; 

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
    gl_Position = projection * view * vec4(vertex_pos, 1.0);
}
