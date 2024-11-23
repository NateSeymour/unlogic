#version 450

layout (location = 0) in vec4 in_color;

layout (location = 0) out vec4 out_color;

void main() {
    if (mod(gl_FragCoord.x, 100) < 3 || mod(gl_FragCoord.y, 100) < 3)
    {
        out_color = vec4(0, 0, 0, 1);
    }
    else
    {
        out_color = vec4(1, 1, 1, 1);
    }
}