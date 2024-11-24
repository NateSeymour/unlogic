#version 450

#define M_PI 3.1415926535897932384626433832795

layout (binding = 0) uniform Camera {
    float dpi;
    float fov;

    vec3 location;
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
    float dist = camera.location.z;
    float alpha = (180.f - camera.fov) / 2.f;
    float base = (dist / tan(alpha * (M_PI / 180.f))) * 2;
    float aspect = camera.window.x / camera.window.y;

    vec2 world = vec2(base * aspect, base);

    vec2 st = (gl_FragCoord.xy / camera.window) * world;
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