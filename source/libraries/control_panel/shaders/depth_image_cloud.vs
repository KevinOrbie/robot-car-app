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

    float depth = 1.0 - texture(depthTexture, imageCoord).x;

    if (depth == 1.0) {
        gl_Position = vec4(2.0, 0.0, 0.0, 1.0);  // Outisde of Camera View Frustum, discarded
    } else {
        vec3 point_pos = vec3(0.00245f, (1.0 - v), u);
        point_pos.y = (point_pos.y - 0.5f) * 0.00135805717f;  // Rescale to [-0.5, 0.5]
        point_pos.z = (point_pos.z - 0.5f) * 0.00319290216f;  // Rescale to [-0.5, 0.5]
        
        depth = depth * length(point_pos) / 0.00245f;  // Get the depth orthogonal to the focal plane

        point_pos = normalize(point_pos);
        point_pos = point_pos * depth;

        gl_Position = projection * view * model * vec4(point_pos, 1.0);
    }
}
