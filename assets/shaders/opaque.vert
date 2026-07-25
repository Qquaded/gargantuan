#version 450

layout(set = 0, binding = 0) uniform CameraUniforms {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
} camera;

layout(set = 0, binding = 1) uniform PartUniforms {
    mat4 ModelMatrix;
    vec4 Rgba;
} part;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

layout(location = 0) out vec3 fragmentNormal;
layout(location = 1) out vec4 fragmentRgba;

void main() {
    gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * part.ModelMatrix * vec4(vertexPosition, 1.0f);
    fragmentNormal = vertexNormal;
    fragmentRgba = part.Rgba;
}
