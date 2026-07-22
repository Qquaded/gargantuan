#pragma once

#include "gargantuan/datatypes/Vector3.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace gargantuan {

struct CFrame : public Userdata<CFrame> {
  public:
    Vector3 Position = Vector3(0, 0, 0);
    glm::mat3 Rotation = glm::mat3();

    CFrame();
    CFrame(Vector3 position);
    CFrame(float x, float y, float z);
    CFrame(Vector3 position, Vector3 lookAtTarget);
    CFrame(Vector3 position, glm::mat3 rotation);
    CFrame(
        float x, float y, float z, float r00, float r01, float r02, float r10, float r11, float r12, float r20,
        float r21, float r22
    );

    Vector3 GetRightVector();
    Vector3 GetUpVector();
    Vector3 GetLookVector();
    CFrame Orthonormalize();
    glm::quat ToQuaternion();

  private:
    static constexpr float CF_EPSILON = 1e-6;
    static constexpr glm::mat3x3 DEFAULT_ROTATION =
        glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));

    static Vector3 SafeUnit(Vector3 vec, Vector3 fallback);
    static glm::mat3 BuildLookRotation(Vector3 position, Vector3 target, Vector3 up = Vector3(0, 1, 0));
};

USERDATA_STACKVALUE(CFrame);

} // namespace gargantuan
