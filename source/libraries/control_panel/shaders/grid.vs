#version 330 core

// Inputs
layout (location = 0) in vec3 aPos; 

uniform float time;
uniform mat4 view;
uniform mat4 projection;

// Outputs
out vec3 v;  // Vertex Positions


/* ======================= Entry Point ======================= */
void main()
{
    v = aPos;

    vec4 vertex_position = vec4(aPos, 1.0);
    gl_Position = projection * view * vertex_position;
}
