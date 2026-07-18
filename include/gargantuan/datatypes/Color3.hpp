#pragma once

#include <glm/glm.hpp>

namespace gargantuan::datatypes {

class Color3 {
  public:
    Color3(float r, float g, float b);
    Color3();

    float R;
    float G;
    float B;

    static Color3 fromRGB(float r, float g, float b);
    static Color3 fromHSV(float h, float s, float v);

    operator glm::vec3() const { return {R, G, B}; }
};

} // namespace gargantuan::datatypes
