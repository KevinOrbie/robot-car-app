#version 330 core

/* Input Data */
layout (location = 0) in vec3 vertex; 

/* Texture Data */
uniform sampler2D depthTexture;

/* Uniform Data */
uniform int image_width;
uniform int image_height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

/* Pipeline Data */
out vec2 imageCoord;


/* ------------------------ Functions ------------------------ */
// None


/* ----------------------- Entry Point ----------------------- */
void main()
{
    float u = float(gl_InstanceID % image_width) / float(image_width);
    float v = (floor(float(gl_InstanceID) / float(image_width)) / float(image_height));
    imageCoord = vec2(u, v);

    float depth = texture(depthTexture, imageCoord).x;
    vec3 point_pos = vec3(u, 1.0 - v, depth);

    gl_Position = projection * view * model * vec4(point_pos, 1.0);
}
