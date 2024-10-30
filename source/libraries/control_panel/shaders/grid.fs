#version 330 core

// Inputs
in vec3 v;  // Vertex Positions

uniform float tick_distance;
uniform float linewidth;

// Outputs
out vec4 FragColor;


/* ======================= Entry Point ======================= */
void main()
{
    vec4 color = vec4(0.2, 0.2, 0.2, 0.1);

    // Draw Grid Minor
    if ((fract(v.x / tick_distance) < linewidth/2.0) || 
        (fract(v.z / tick_distance) < linewidth/2.0) || 
        (fract(v.x / tick_distance) > (1.0 - linewidth/2.0)) || 
        (fract(v.z / tick_distance) > (1.0 - linewidth/2.0))) {
        color = vec4(0.4, 0.4, 0.4, 1.0);
    }

    // Draw Grid Major
    if ((fract(v.x / tick_distance / 5) < linewidth/0.5/5.0) || 
        (fract(v.z / tick_distance / 5) < linewidth/0.5/5.0) || 
        (fract(v.x / tick_distance / 5) > (1.0 - linewidth/0.5/5.0)) || 
        (fract(v.z / tick_distance / 5) > (1.0 - linewidth/0.5/5.0))) {
        color = vec4(0.7, 0.7, 0.7, 1.0);
    }

    // Draw Axis
    if (abs(v.x / tick_distance) < linewidth/0.5) {
        // Blue Z-Axis
        color = vec4(0.0, 0.0, 1.0, 1.0);
    }

    if (abs(v.z / tick_distance) < linewidth/0.5) {
        // Red X-Axis
        color = vec4(1.0, 0.0, 0.0, 1.0);
    }

    // Draw Origin
    color += (1.0 - step(0.05, sqrt(v.x * v.x + v.z * v.z)));

    FragColor = color;
}
