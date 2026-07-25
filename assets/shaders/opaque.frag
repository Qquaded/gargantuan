#version 450

layout(location = 0) in vec3 fragmentNormal;
layout(location = 1) in vec4 fragmentRgba;
layout(location = 2) in vec3 worldPosition;

layout(location = 0) out vec4 outputRgba;

layout(set = 0, binding = 0) uniform WorldUniforms {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec3 LightDirection;
    mat4 LightSpaceMatrix;
} world;

layout(set = 0, binding = 2) uniform sampler2DShadow shadowMap;

void main() {
    // Shadows
    vec3 normal = normalize(fragmentNormal);
    vec4 lightSpacePosition = world.LightSpaceMatrix * vec4(worldPosition, 1.0);
    vec3 projection = lightSpacePosition.xyz / lightSpacePosition.w;

    vec2 uv = projection.xy * vec2(0.5, -0.5) + vec2(0.5);

    float shadow = 1.0f;
    if (uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0 && projection.z >= 0.0 && projection.z <= 1.0) {
        float nDotL = max(dot(normal, world.LightDirection), 0.0);
        float bias = clamp(0.0015 * tan(acos(nDotL)), 0.0003, 0.004);
        float depth = projection.z - bias;

        vec2 texelSize = 1 / vec2(2048.0, 2048.0);

        float shadow = 0.0f;

        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                vec2 offset = vec2(x, y) * texelSize;
                shadow = texture(shadowMap, vec3(uv + offset, depth));
            }
        }
    }

    // Shading
    float diffuseFactor = max(dot(normal, world.LightDirection), 0.0) * shadow;
    float ambientFactor = 0.15;
    float totalLight = ambientFactor + diffuseFactor;

    outputRgba = vec4(fragmentRgba.rgb * totalLight, fragmentRgba.a);
}
