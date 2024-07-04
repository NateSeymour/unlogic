#version 400 core

layout (location = 0) out vec4 color;

in vec4 vertex_color_f;

void main()
{
    color = vertex_color_f;
}