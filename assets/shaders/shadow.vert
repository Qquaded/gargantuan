#version 450

layout(set = 0, binding = 0) uniform LightUnforms {
    mat4 LightSpaceMatrix;
} light;

layout(set = 0, binding = 1) uniform PartUniforms {
    mat4 ModelMatrix;
} part;

layout(location = 0) in vec3 vertexPosition;

void main()
{
    gl_Position = light.LightSpaceMatrix * part.ModelMatrix * vec4(vertexPosition, 1.0);
}
