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

layout (location = 0) in vec4 in_color;

layout (location = 0) out vec4 out_color;

bool grid(vec2 st, float interval, float width) {
    vec2 base = round(st / interval) * interval;
    vec2 distance = abs(st - base);
    return distance.x < width / 2 || distance.y < width / 2;
}

bool centerline(vec2 st, float width) {
    return abs(st.x) < width / 2 || abs(st.y) < width / 2;
}

void main() {
    // ((gl_FragCoord.xy - ((camera.window * camera.dpi_scalar) / 2) - (camera.center.xy * camera.dpi_scalar)) / camera.resolution);
    vec4 ndc = vec4((gl_FragCoord.xy + camera.window * camera.dpi / 2) / (camera.window * camera.dpi), 0.0, 1.0);
    vec2 st = (inverse(camera.projection) * ndc).xy;
    vec4 color = vec4(1.0, 1.0, 1.0, 1.0);

    if (centerline(st, 0.2)) {
        color.rgb = vec3(0.0);
    } else if (grid(st, 5, 0.1)) {
        color.rgb = vec3(0.6);
    } else if (grid(st, 1, 0.05)) {
        color.rgb = vec3(0.9);
    }

    out_color = color;
}