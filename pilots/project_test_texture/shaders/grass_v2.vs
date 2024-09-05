#version 330 core
precision highp float;

layout (location = 0) in vec3 aPos; 

out vec3 vertexPos;

uniform float time;
uniform mat4 model; // Remove
uniform mat4 view;
uniform mat4 projection;


/* ------------------------ Functions ------------------------ */
// Generate a pseudo-random value in range [0.0, 1.0] from a given vector.
float random(vec2 st) {
    float scale = 43758.5453123;
    vec2 directionality = vec2(12.9898, 78.233);
    return fract(sin(dot(st, directionality)) * scale);
}

// Create a 2D rotation matrix.
mat2 rotate2d(float angle){
    return mat2(cos(angle),-sin(angle),
                sin(angle),cos(angle));
}


/* ----------------------- Entry Point ----------------------- */
void main()
{
    // Pass along variables
    vertexPos = aPos;

    // Set Constants
    int width = 500;
    float spacer = 0.08;

    // Prepare Variables
    vec4 vtx_pos = vec4(aPos, 1.0);
    vec2 grid_index = vec2(0.0, 0.0);
    grid_index.x = floor(float(gl_InstanceID) / float(width));
    grid_index.y = float(gl_InstanceID % width);  // NOTE: mod() function in glsl always uses floats (after OpenGL 3.0), thus use % instead.
    grid_index *= spacer;
    float rng = random(grid_index);

    /* Apply Model Matrix Transformation */
    // Rotate (around y-axis (2D))
    vtx_pos.xz = rotate2d(rng * 360.0) * vtx_pos.xz;
    // Scale
    vtx_pos *= rng * 0.2 + 0.7;
    // Translate
    vtx_pos.xz += grid_index + vec2((rng - 0.5) * spacer);

    // Add Wind
    vtx_pos.x = vtx_pos.x + rng * vtx_pos.y * sin(time * 6.28 * 0.20 + 6.28 + rng * 3.14) * 0.05;
    vtx_pos.z = vtx_pos.z + rng * vtx_pos.y * sin(time * 6.28 * 0.20 + 6.28 + rng * 3.14) * 0.05;

    gl_Position = projection * view * vtx_pos;
}
