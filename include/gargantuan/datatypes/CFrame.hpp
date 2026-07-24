#pragma once

#include "gargantuan/datatypes/Vector3.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <lua.h>
#include <tuple>

namespace gargantuan {

// enum class RotationOrder : int { XYZ, XZY, YZX, YXZ, ZXY, ZYX };

struct CFrame : public Userdata<CFrame> {
  public:
    static constexpr float CF_EPSILON = 1e-6;
    static constexpr glm::mat3 DEFAULT_ROTATION = glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    typedef std::tuple<double, double, double, double, double, double, double, double, double, double, double, double>
        Components;

    glm::vec3 Position = {0, 0, 0};
    glm::mat3 Rotation = {};

    CFrame();
    CFrame(glm::vec3 position);
    CFrame(float x, float y, float z);
    CFrame(glm::vec3 position, glm::vec3 lookAtTarget);
    CFrame(glm::vec3 position, glm::mat3 rotation);
    CFrame(
        float x, float y, float z, float r00, float r01, float r02, float r10, float r11, float r12, float r20,
        float r21, float r22
    );

    static CFrame Angles(float x, float y, float z);
    static CFrame fromMatrix(glm::vec3 position, glm::vec3 right, glm::vec3 up, glm::vec3 look);
    static CFrame fromQuaternion(float x, float y, float z, float w, glm::vec3 position);

    glm::vec3 GetRightVector();
    glm::vec3 GetUpVector();
    glm::vec3 GetLookVector();

    CFrame Inverse();
    CFrame Lerp(CFrame goal, double alpha);
    CFrame Orthonormalize();
    // NOTE: XToY functions are supposedly tuples on Roblox, not gon do allat rn
    CFrame ToWorldSpace(CFrame cf);
    CFrame ToObjectSpace(CFrame cf);
    glm::vec3 PointToWorldSpace(glm::vec3 point);
    glm::vec3 PointToObjectSpace(glm::vec3 point);
    glm::vec3 VectorToWorldSpace(glm::vec3 point);
    glm::vec3 VectorToObjectSpace(glm::vec3 point);
    Components GetComponents();
    // std::tuple<double, double, double> ToEulerAngles(RotationOrder order);
    std::tuple<double, double, double> ToEulerAnglesXYZ();
    std::tuple<double, double, double> ToEulerAnglesYXZ();
    std::tuple<double, double, double> ToOrientation();
    std::tuple<glm::vec3, double> ToAxisAngle();
    CFrame FuzzyEq(CFrame other, double epsilon = 1e-5);
    double AngleBetween(CFrame other);
    glm::quat ToQuaternion();

    static int LAdd(lua_State *L, CFrame *self);
    static int LSubtract(lua_State *L, CFrame *self);
    static int LMultiply(lua_State *L, CFrame *self);
    static int LTostring(lua_State *L, CFrame *self);

    static glm::vec3 SafeUnit(glm::vec3 vec, glm::vec3 fallback);
    static glm::mat3 BuildLookRotation(glm::vec3 position, glm::vec3 target, glm::vec3 up = {0, 1, 0});
    static glm::mat3 MultiplyRotation(glm::mat3 lhs, glm::mat3 rhs);

    glm::vec3 operator*(glm::vec3 other) { return Position + (Rotation * other); };

    CFrame operator*(CFrame other) {
        glm::vec3 transformedPosition = Position + (Rotation * other.Position);
        glm::mat3 transformedRotation = Rotation * other.Rotation;
        return CFrame(transformedPosition, transformedRotation);
    };

    static std::string_view GetUserdataType();
    static UserdataTag GetUserdataTag();
    static const CFrame::UserdataProperties &GetUserdataProperties();
    static const CFrame::UserdataMethods &GetUserdataMethods();
};

USERDATA_STACKVALUE(CFrame);

} // namespace gargantuan
