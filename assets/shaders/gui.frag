#version 450

layout(location = 0) in vec2 FragmentUV;
layout(location = 1) in vec4 FragmentBackground;
layout(location = 2) flat in int FragmentTextureIndex;

layout(location = 0) out vec4 OutputColor;

layout(set = 1, binding = 0) uniform GuiUniforms {
    vec2 ViewportSize;
} uniforms;

void main() {
    OutputColor = FragmentBackground;
}
