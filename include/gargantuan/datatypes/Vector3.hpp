// (In Igblon voice)
// I am well aware there is a Luau vector type
// I will be ignoring this information for now
//
// "uuh huu its less performant" Go make an engine urself then give feedback
// (looks at ross general)
//
// Plus im not bothered to implement more of this for now
// Proper vec3 impl will be with the vector builtin, this is lit to get a demo
// of gargantuan up
//
// TODO: Use Luau vector builtin

#include "gargantuan/scripting/Userdata.hpp"

#include <glm/glm.hpp>

namespace gargantuan {

struct Vector3 : Userdata<Vector3> {
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    Vector3();
    Vector3(glm::vec3);
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);

    operator glm::vec3() const { return {X, Y, Z}; }
    Vector3 operator-(const Vector3 &other) const { return {X - other.X, Y - other.Y, Z - other.Z}; }
};

USERDATA_STACKVALUE(Vector3)

} // namespace gargantuan
