#version 450

vec2 POSITIONS[3] = vec2[](
        vec2(0.0, 1.0),
        vec2(-1.0, -1.0),
        vec2(1.0, -1.0)
    );

vec3 COLORS[3] = vec3[](
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0)
    );

layout(location = 0) out vec3 fragmentColor;

void main() {
    gl_Position = vec4(POSITIONS[gl_VertexIndex], 0.0, 1.0);
    fragmentColor = COLORS[gl_VertexIndex];
}
