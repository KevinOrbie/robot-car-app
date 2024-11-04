#version 330 core

// Inputs
layout (location = 0) in vec3 aPos; 

uniform mat4 view;
uniform mat4 projection;


/* ======================= Entry Point ======================= */
void main()
{
    vec4 vertex_position = vec4(aPos, 1.0);
    vertex_position.y += 0.001;
    gl_Position = projection * view * vertex_position;
}
