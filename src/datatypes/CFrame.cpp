#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <glm/glm.hpp>
#include <lua.h>

namespace gargantuan {

template <> UserdataTag CFrame::This::GetUserdataTag() { return UserdataTag::CFrame; };
template <> std::string_view CFrame::This::GetUserdataType() { return "CFrame"; };

template <> const CFrame::UserdataProperties &CFrame::This::GetUserdataProperties() {
    static const CFrame::UserdataProperties PROPERTIES = {
        USERDATA_READONLY_PROP(CFrame, Position, gargantuan::Vector3),
        {
            "Rotation",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    StackValue<CFrame>::Push(L, CFrame(Vector3(0, 0, 0), cframe->Rotation));
                    return 1;
                },
                nullptr,
            },
        },
        {
            "X",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    lua_pushnumber(L, cframe->Position.X);
                    return 1;
                },
                nullptr,
            },
        },
        {
            "Y",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    lua_pushnumber(L, cframe->Position.X);
                    return 1;
                },
                nullptr,
            },
        },
        {
            "Z",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    lua_pushnumber(L, cframe->Position.X);
                    return 1;
                },
                nullptr,
            },
        },
        {
            "RightVector",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    StackValue<CFrame>::Push(L, cframe->GetRightVector());
                    return 1;
                },
                nullptr,
            },
        },
        {
            "UpVector",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    StackValue<CFrame>::Push(L, cframe->GetUpVector());
                    return 1;
                },
                nullptr,
            },
        },
        {
            "LookVector",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    StackValue<CFrame>::Push(L, cframe->GetLookVector());
                    return 1;
                },
                nullptr,
            },
        },
        {
            "XVector",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    StackValue<CFrame>::Push(L, cframe->GetRightVector());
                    return 1;
                },
                nullptr,
            },
        },
        {
            "YVector",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    StackValue<CFrame>::Push(L, cframe->GetUpVector());
                    return 1;
                },
                nullptr,
            },
        },
        {
            "ZVector",
            {
                [](lua_State *L, CFrame *cframe) -> int {
                    StackValue<CFrame>::Push(L, cframe->GetLookVector());
                    return 1;
                },
                nullptr,
            },
        }
    };
    return PROPERTIES;
};

template <> const CFrame::UserdataMethods &CFrame::This::GetUserdataMethods() {
    static const CFrame::UserdataMethods METHODS = {};
    return METHODS;
};

Vector3 CFrame::SafeUnit(Vector3 vec, Vector3 fallback) {
    auto magSq = vec.X * vec.X + vec.Y * vec.Y + vec.Z * vec.Z;
    if (magSq <= CF_EPSILON * CF_EPSILON) {
        return fallback;
    }

    auto mag = glm::sqrt(magSq);
    return Vector3(vec.X / mag, vec.Y / mag, vec.Z / mag);
}

glm::mat3 CFrame::BuildLookRotation(Vector3 position, Vector3 target, Vector3 up) {
    glm::vec3 positionGlm = position;
    glm::vec3 targetGlm = target;
    glm::vec3 upGlm = up;

    glm::vec3 z = SafeUnit(position - target, Vector3(0, 0, 1));
    if (glm::abs(glm::dot(upGlm, z) > 0.999)) {
        up = Vector3(0, 0, 1);
    };

    glm::vec3 x = SafeUnit(glm::cross(upGlm, z), glm::vec3(1, 0, 0));
    glm::vec3 y = SafeUnit(glm::cross(z, x), glm::vec3(0, 1, 0));

    return glm::mat3(glm::vec3(x.x, y.x, z.x), glm::vec3(x.y, y.y, z.y), glm::vec3(x.z, y.z, z.z));
}

CFrame::CFrame() : Position(0.0f, 0.0f, 0.0f), Rotation(CFrame::DEFAULT_ROTATION) {};
CFrame::CFrame(Vector3 position) : Position(position), Rotation(CFrame::DEFAULT_ROTATION) {};
CFrame::CFrame(float x, float y, float z) : Position(x, y, z), Rotation(CFrame::DEFAULT_ROTATION) {};
CFrame::CFrame(Vector3 position, Vector3 target) : Position(position), Rotation(BuildLookRotation(position, target)) {};
CFrame::CFrame(Vector3 position, glm::mat3 rotation) : Position(position), Rotation(rotation) {};
CFrame::CFrame(
    float x, float y, float z, float r00, float r01, float r02, float r10, float r11, float r12, float r20, float r21,
    float r22
)
    : Position(x, y, z), Rotation(r00, r01, r02, r10, r11, r12, r20, r21, r22) {};

Vector3 CFrame::GetRightVector() { return Vector3(Rotation[0][0], Rotation[1][0], Rotation[2][0]); }
Vector3 CFrame::GetUpVector() { return Vector3(Rotation[0][1], Rotation[1][1], Rotation[2][1]); }
Vector3 CFrame::GetLookVector() { return Vector3(-Rotation[0][2], -Rotation[1][2], -Rotation[2][2]); }

CFrame CFrame::Orthonormalize() {
    glm::vec3 x = GetRightVector();
    glm::vec3 y = GetUpVector();

    x = glm::normalize(x);
    y = glm::normalize(y - x * glm::dot(x, y));
    glm::vec3 z = glm::cross(x, y);

    return CFrame(Position.X, Position.Y, Position.Z, x.x, y.x, z.x, x.y, y.y, z.y, x.z, y.z, z.z);
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

} // namespace gargantuan
