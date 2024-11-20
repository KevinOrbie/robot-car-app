#version 330 core

// Input Data
layout (location = 0) in vec3 vertex_pos; 
layout (location = 1) in vec3 vertex_norm; 

// Uniform Data
uniform mat4 model;
uniform mat3 normal;  // Model Matrix for normals, as described here: https://learnopengl.com/Lighting/Basic-Lighting
uniform mat4 view;
uniform mat4 projection;

// Pipeline Data
out vec3 frag_pos;
out vec3 frag_norm;

/* ------------------------ Functions ------------------------ */
// None


/* ----------------------- Entry Point ----------------------- */
void main()
{
    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);

    frag_pos = (model * vec4(vertex_pos, 1.0)).xyz;
    frag_norm = normal * vertex_norm;
}
