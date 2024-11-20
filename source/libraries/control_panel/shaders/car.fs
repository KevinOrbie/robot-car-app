#version 330 core

// Pipeline Data
in vec3 frag_pos;
in vec3 frag_norm;

// Output Color
out vec4 FragColor;


/* ------------------------ Functions ------------------------ */
// None


/* ----------------------- Entry Point ----------------------- */
void main()
{
    vec3 object_color = vec3(0.5, 0.5, 0.5);
    vec3 light_color = vec3(0.5, 0.5, 0.5);

    /* Set Ambient Light */
    float ambient = 0.5;

    /* Calculate Diffusion Light */
    vec3 light_pos = vec3(5.0, 5.0, 5.0);
    vec3 norm = normalize(frag_norm);
    vec3 light_direction = normalize(light_pos - frag_pos);

    float diff = max(dot(norm, light_direction), 0.0);  // Orthogonal = max light
    vec3 diffuse = diff * light_color;

    /* Combine all effects */
    vec3 result = (ambient + diffuse) * object_color;
    FragColor = vec4(result, 1.0);
}
