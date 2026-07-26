#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <cstdlib>
#include <lua.h>
#include <lualib.h>
#include <sstream>

namespace gargantuan {

Vector2::Vector2(float x, float y) : Value(x, y) {};
Vector2::Vector2(glm::vec2 vec) : Value(vec) {};

float Vector2::GetX() const { return Value.x; };
float Vector2::GetY() const { return Value.y; };
float Vector2::GetMagnitude() const { return glm::length(Value); };
Vector2 Vector2::GetUnit() const { return glm::normalize(Value); };

float Vector2::Cross(const Vector2 &other) const { return (GetX() * other.GetY()) - (GetY() * GetX()); };
Vector2 Vector2::Abs() const { return glm::abs(Value); };
Vector2 Vector2::Ceil() const { return glm::ceil(Value); };
Vector2 Vector2::Floor() const { return glm::floor(Value); };
Vector2 Vector2::Sign() const { return glm::sign(Value); };
float Vector2::Angle(const Vector2 &other, bool isSigned) const {
    float angle = atan2(Cross(other), Dot(other));
    return isSigned ? angle : abs(angle);
};
float Vector2::Dot(const Vector2 &other) const { return GetX() * other.GetX() + GetY() * other.GetY(); };
Vector2 Vector2::Lerp(const Vector2 &goal, float alpha) const { return Value + (goal.Value - Value) * alpha; };
Vector2 Vector2::Max(const Vector2 &other) const { return glm::max(Value, other.Value); };
Vector2 Vector2::Min(const Vector2 &other) const { return glm::min(Value, other.Value); };
Vector2 Vector2::FuzzyEq(const Vector2 &other, float epsilon) const {
    return glm::abs(Value.x - this->Value.x) <= epsilon && glm::abs(Value.y - this->Value.y) <= epsilon;
};

int Vector2::LTostring(lua_State *L, Vector2 *self) {
    std::ostringstream ss;
    ss << self->GetX() << ", " << self->GetY();
    std::string str = ss.str();
    lua_pushlstring(L, str.c_str(), str.size());
    return 1;
}

int Vector2::LAdd(lua_State *L, Vector2 *self) {
    Vector2 other = StackValue<Vector2>::From(L, -1);
    StackValue<Vector2>::Push(L, self->Value + other.Value);
    return 1;
}

int Vector2::LSub(lua_State *L, Vector2 *self) {
    Vector2 other = StackValue<Vector2>::From(L, -1);
    StackValue<Vector2>::Push(L, self->Value - other.Value);
    return 1;
}

int Vector2::LMul(lua_State *L, Vector2 *self) {
    if (lua_isnumber(L, -1)) {
        float other = lua_tonumber(L, -1);
        StackValue<Vector2>::Push(L, self->Value * other);
    } else if (StackValue<Vector2>::Is(L, -1)) {
        Vector2 other = StackValue<Vector2>::From(L, -1);
        StackValue<Vector2>::Push(L, self->Value * other.Value);
    } else {
        luaL_typeerror(L, 2, "Vector2 or number");
        return 0;
    }
    return 1;
}

int Vector2::LDiv(lua_State *L, Vector2 *self) {
    if (lua_isnumber(L, -1)) {
        float other = lua_tonumber(L, -1);
        StackValue<Vector2>::Push(L, self->Value / other);
    } else if (StackValue<Vector2>::Is(L, -1)) {
        Vector2 other = StackValue<Vector2>::From(L, -1);
        StackValue<Vector2>::Push(L, self->Value / other.Value);
    } else {
        luaL_typeerror(L, 2, "Vector2 or number");
        return 0;
    }
    return 1;
}

std::string_view Vector2::GetUserdataType() { return "Vector2"; };
UserdataTag Vector2::GetUserdataTag() { return UserdataTag::Vector2; };
const Vector2::UserdataProperties &Vector2::GetUserdataProperties() {
    static const UserdataProperties PROPERTIES{
        {"X", {Method::Wrap<&Vector2::GetX>().Call, nullptr}},
        {"Y", {Method::Wrap<&Vector2::GetY>().Call, nullptr}},
        {"Magnitude", {Method::Wrap<&Vector2::GetMagnitude>().Call, nullptr}},
        {"Unit", {Method::Wrap<&Vector2::GetUnit>().Call, nullptr}},
    };
    return PROPERTIES;
};

const Vector2::UserdataMethods &Vector2::GetUserdataMethods() {
    static const UserdataMethods METHODS{
        USERDATA_METHOD(Vector2, Cross), USERDATA_METHOD(Vector2, Abs),     USERDATA_METHOD(Vector2, Ceil),
        USERDATA_METHOD(Vector2, Floor), USERDATA_METHOD(Vector2, Sign),    USERDATA_METHOD(Vector2, Angle),
        USERDATA_METHOD(Vector2, Dot),   USERDATA_METHOD(Vector2, Lerp),    USERDATA_METHOD(Vector2, Max),
        USERDATA_METHOD(Vector2, Min),   USERDATA_METHOD(Vector2, FuzzyEq), {"__tostring", {Vector2::LTostring}},
        {"__add", {Vector2::LAdd}},      {"__sub", {Vector2::LSub}},        {"__mul", {Vector2::LMul}},
        {"__div", {Vector2::LDiv}},
    };
    return METHODS;
};

} // namespace gargantuan
