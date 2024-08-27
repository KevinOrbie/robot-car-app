#version 330 core
in vec3 vertexPos;

out vec4 FragColor;

void main()
{
    FragColor = vec4((vertexPos.y * 0.4) + 0.1, (vertexPos.y * 0.7) + 0.1, (vertexPos.y * 0.3) + 0.1, 1.0);
}
