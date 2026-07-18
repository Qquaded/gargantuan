#version 450

layout(location = 0) in vec3 fragmentNormal;
layout(location = 1) in vec3 fragmentColor;
layout(location = 0) out vec4 outputColor;

// TODO: proper lighting infra
void main() {
    vec3 lightDirection = normalize(vec3(0.5, 1.0, 0.75));
    vec3 normal = normalize(fragmentNormal);
    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    float ambientFactor = 0.15;
    float totalLight = ambientFactor + diffuseFactor;

    outputColor = vec4(fragmentColor.rgb * totalLight, 1.0);
}
