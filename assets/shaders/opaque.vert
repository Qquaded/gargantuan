// #version 450

// layout(set = 0, binding = 0) uniform WorldUniforms {
//     mat4 ViewMatrix;
//     mat4 ProjectionMatrix;
//     vec3 LightDirection;
//     mat4 LightSpaceMatrix;
// } world;

// layout(set = 0, binding = 1) uniform PartUniforms {
//     mat4 ModelMatrix;
//     vec4 Rgba;
// } part;

// void main() {
//     vec4 worldPosition4 = part.ModelMatrix * vec4(vertexPosition, 1.0);
//     worldPosition = worldPosition4.xyz;

//     mat3 normalMatrix = transpose(inverse(mat3(part.ModelMatrix)));
//     fragmentNormal = normalize(normalMatrix * vertexNormal);

//     fragmentRgba = part.Rgba;
//     gl_Position = world.ProjectionMatrix * world.ViewMatrix * part.ModelMatrix * vec4(vertexPosition, 1.0);
// }
#version 450

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

layout(set = 0, binding = 0) uniform WorldUniforms {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec4 LightDirection;
    mat4 LightSpaceMatrix;
} world;

layout(set = 0, binding = 1) uniform PartUniforms {
    mat4 ModelMatrix;
    vec4 Rgba;
} part;

layout(location = 0) out vec3 fragmentNormal;
layout(location = 1) out vec4 fragmentRgba;
layout(location = 2) out vec3 worldPosition;

void main() {
    // NOTE: if u define ANY of the output variables before gl_Position, it
    // renders black. This shit tookw ay too long to debug
    gl_Position = world.ProjectionMatrix * world.ViewMatrix * part.ModelMatrix * vec4(vertexPosition, 1.0f);

    fragmentNormal = normalize(mat3(part.ModelMatrix) * vertexNormal);
    fragmentRgba = part.Rgba;
    worldPosition = vec3(part.ModelMatrix * vec4(vertexPosition, 1.0f));
}
