#version 450

layout(set = 1, binding = 0) uniform PushUniforms {
    mat4 mvp;
    vec4 rgba;
} uniforms;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

layout(location = 0) out vec3 fragmentNormal;
layout(location = 1) out vec4 fragmentRgba;

void main() {
    gl_Position = uniforms.mvp * vec4(vertexPosition, 1.0f);
    fragmentNormal = vertexNormal;
    fragmentRgba = uniforms.rgba;
}
