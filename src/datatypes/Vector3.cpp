#include "gargantuan/datatypes/Vector3.hpp"

namespace gargantuan {

template <> UserdataTag Vector3::This::GetUserdataTag() { return UserdataTag::Vector3; };
template <> std::string_view Vector3::This::GetUserdataType() { return "Vector3"; };

template <> const Vector3::UserdataProperties &Vector3::This::GetUserdataProperties() {
    static const Vector3::UserdataProperties PROPERTIES = {
        USERDATA_READONLY_PROP(Vector3, X, float),
        USERDATA_READONLY_PROP(Vector3, Y, float),
        USERDATA_READONLY_PROP(Vector3, Z, float),
    };
    return PROPERTIES;
};

template <> const Vector3::UserdataMethods &Vector3::This::GetUserdataMethods() {
    static const Vector3::UserdataMethods METHODS = {};
    return METHODS;
};

Vector3::Vector3() {}
Vector3::Vector3(glm::vec3 vec) : X(vec.x), Y(vec.y), Z(vec.z) {}
Vector3::Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

} // namespace gargantuan
