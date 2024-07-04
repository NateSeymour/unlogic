#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 vertex_color;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(pos.xy, 1.0, 1.0);
}