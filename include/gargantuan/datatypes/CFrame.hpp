#pragma once

#include "gargantuan/scripting/Userdata.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace gargantuan {

struct CFrame : public Userdata<CFrame> {
  public:
    glm::vec3 Position = glm::vec3(0, 0, 0);
    glm::mat3 Rotation = glm::mat3();

    CFrame();
    CFrame(glm::vec3 position);
    CFrame(float x, float y, float z);
    CFrame(glm::vec3 position, glm::vec3 lookAtTarget);
    CFrame(
        float x, float y, float z, float r00, float r01, float r02, float r10, float r11, float r12, float r20,
        float r21, float r22
    );

    glm::vec3 GetRightVector();
    glm::vec3 GetUpVector();
    glm::vec3 GetLookVector();
    CFrame Orthonormalize();
    glm::quat ToQuaternion();

  private:
    static constexpr float CF_EPSILON = 1e-6;
    static constexpr glm::mat3x3 DEFAULT_ROTATION =
        glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));

    static glm::vec3 SafeUnit(glm::vec3 vec, glm::vec3 fallback);
    static glm::mat3 BuildLookRotation(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0, 1, 0));
};

USERDATA_STACKVALUE(CFrame);

} // namespace gargantuan
