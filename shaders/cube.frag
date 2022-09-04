#version 330 core
out vec4 FragColor;
in vec3 vertex_color;

void main()
{
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = vec4(vertex_color, 1.0);
}