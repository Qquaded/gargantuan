#version 450

layout(set = 1, binding = 0) uniform PushUniforms {
    mat4 modelViewProjection;
} uniforms;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inRgba;

layout(location = 0) out vec3 fragmentNormal;
layout(location = 1) out vec4 fragmentRgba;

void main() {
    gl_Position = uniforms.modelViewProjection * vec4(inPosition, 1.0f);
    fragmentNormal = inNormal;
    fragmentRgba = inRgba;
}
