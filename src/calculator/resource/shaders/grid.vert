#version 450

layout (binding = 0) uniform Camera {
    float dpi;
    float fov;

    vec2 location;
    vec2 window;

    mat4 model;
    mat4 view;
    mat4 projection;
} camera;

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec4 in_color;

layout (location = 0) out vec4 out_color;

void main() {
    gl_Position = vec4(in_position, 0.0, 1.0);
    out_color = in_color;
}
