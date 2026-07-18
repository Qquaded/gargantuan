#include "gargantuan/datatypes/CFrame.hpp"

#include <glm/glm.hpp>

namespace gargantuan::datatypes {

glm::vec3 CFrame::SafeUnit(glm::vec3 vec, glm::vec3 fallback) {
    auto magSq = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    if (magSq <= CF_EPSILON * CF_EPSILON) {
        return fallback;
    }

    auto mag = glm::sqrt(magSq);
    return glm::vec3(vec.x / mag, vec.y / mag, vec.z / mag);
}

glm::mat3 CFrame::BuildLookRotation(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    auto z = SafeUnit(position - target, glm::vec3(0, 0, 1));
    if (glm::abs(glm::dot(up, z) > 0.999)) {
        up = glm::vec3(0, 0, 1);
    };

    auto x = SafeUnit(glm::cross(up, z), glm::vec3(1, 0, 0));
    auto y = SafeUnit(glm::cross(z, x), glm::vec3(0, 1, 0));

    return glm::mat3(glm::vec3(x.x, y.x, z.x), glm::vec3(x.y, y.y, z.y), glm::vec3(x.z, y.z, z.z));
}

CFrame::CFrame() : Position(0.0f, 0.0f, 0.0f), Rotation(CFrame::DEFAULT_ROTATION) {};
CFrame::CFrame(glm::vec3 position) : Position(position), Rotation(CFrame::DEFAULT_ROTATION) {};
CFrame::CFrame(float x, float y, float z) : Position(x, y, z), Rotation(CFrame::DEFAULT_ROTATION) {};
CFrame::CFrame(glm::vec3 position, glm::vec3 target)
    : Position(position), Rotation(BuildLookRotation(position, target)) {};
CFrame::CFrame(float x, float y, float z, float r00, float r01, float r02, float r10, float r11, float r12, float r20,
               float r21, float r22)
    : Position(x, y, z), Rotation(r00, r01, r02, r10, r11, r12, r20, r21, r22) {};

glm::vec3 CFrame::GetRightVector() { return glm::vec3(Rotation[0][0], Rotation[1][0], Rotation[2][0]); }
glm::vec3 CFrame::GetUpVector() { return glm::vec3(Rotation[0][1], Rotation[1][1], Rotation[2][1]); }
glm::vec3 CFrame::GetLookVector() { return glm::vec3(-Rotation[0][2], -Rotation[1][2], -Rotation[2][2]); }

CFrame CFrame::Orthonormalize() {
    auto x = GetRightVector();
    auto y = GetUpVector();

    x = glm::normalize(x);
    y = glm::normalize(y - x * glm::dot(x, y));
    auto z = glm::cross(x, y);

    return CFrame(Position.x, Position.y, Position.z, x.x, y.x, z.x, x.y, y.y, z.y, x.z, y.z, z.z);
}

glm::quat CFrame::ToQuaternion() {
    auto cf = Orthonormalize();
    auto r = cf.Rotation;

    auto trace = r[0][0] + r[1][1] + r[2][2];

    float s, w, x, y, z;

    if (trace > 0) {
        s = glm::sqrt(trace + 1.0) * 2;
        w = 0.25 * s;
        x = (r[2][1] - r[1][2]) / s;
        y = (r[0][2] - r[2][0]) / s;
        z = (r[1][0] - r[0][1]) / s;
    } else if (r[0][0] > r[1][1] && r[0][0] > r[2][2]) {
        s = glm::sqrt(1.0 + r[0][0] - r[1][1] - r[2][2]) * 2;
        w = (r[2][1] - r[1][2]) / s;
        x = 0.25 * s;
        y = (r[0][1] + r[1][0]) / s;
        z = (r[0][2] + r[2][0]) / s;
    } else if (r[1][1] > r[2][2]) {
        s = glm::sqrt(1.0 + r[1][1] - r[0][0] - r[2][2]) * 2;
        w = (r[0][2] - r[2][0]) / s;
        x = (r[0][1] + r[1][0]) / s;
        y = 0.25 * s;
        z = (r[1][2] + r[2][1]) / s;
    } else {
        s = glm::sqrt(1.0 + r[2][2] - r[0][0] - r[1][1]) * 2;
        w = (r[1][0] - r[0][1]) / s;
        x = (r[0][2] + r[2][0]) / s;
        y = (r[1][2] + r[2][1]) / s;
        z = 0.25 * s;
    };

    return glm::quat(x, y, z, w);
}

} // namespace gargantuan::datatypes
