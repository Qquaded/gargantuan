// Has GPU programming gone too far?

#include "gargantuan/render/PrimitiveMeshes.hpp"

#include <vector>

namespace gargantuan::render::PrimitiveMeshes {

static constexpr glm::vec2 UV_00{0.0f, 1.0f};
static constexpr glm::vec2 UV_01{0.0f, 1.0f};
static constexpr glm::vec2 UV_10{1.0f, 0.0f};
static constexpr glm::vec2 UV_11{1.0f, 1.0f};

Mesh Cube(glm::vec4 rgba) {
    return Mesh{
        std::vector<Vertex>{
            Vertex{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, rgba, UV_00},
            Vertex{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, rgba, UV_01},
            Vertex{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, rgba, UV_11},
            Vertex{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, rgba, UV_10},

            Vertex{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, rgba, UV_00},
            Vertex{{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, rgba, UV_01},
            Vertex{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, rgba, UV_11},
            Vertex{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, rgba, UV_10},

            Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, rgba, UV_00},
            Vertex{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, rgba, UV_01},
            Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, rgba, UV_11},
            Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, rgba, UV_10},

            Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, rgba, UV_00},
            Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, rgba, UV_01},
            Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, rgba, UV_11},
            Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, rgba, UV_10},

            Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, rgba, UV_00},
            Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, rgba, UV_01},
            Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, rgba, UV_11},
            Vertex{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, rgba, UV_10},

            Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, rgba, UV_00},
            Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, rgba, UV_01},
            Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, rgba, UV_11},
            Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, rgba, UV_10},
        },
        std::vector<uint32_t>{
            0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,  8,  9,  10, 8,  10, 11,
            12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23,
        },
    };
};
Mesh Wedge(glm::vec4 rgba) {
    return Mesh{
        {
            Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, rgba, UV_00},
            Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, rgba, UV_10},
            Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, rgba, UV_11},
            Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, rgba, UV_01},

            Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, rgba, UV_00},
            Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, rgba, UV_10},
            Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, rgba, UV_11},
            Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, rgba, UV_01},

            Vertex{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, rgba, UV_00},
            Vertex{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, rgba, UV_10},
            Vertex{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, rgba, UV_01},

            Vertex{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, rgba, UV_00},
            Vertex{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, rgba, UV_10},
            Vertex{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, rgba, UV_01},

            Vertex{{-0.5f, 0.5f, -0.5f}, {0.707f, 0.707f, 0.707f}, rgba, UV_00},
            Vertex{{0.5f, 0.5f, -0.5f}, {0.707f, 0.707f, 0.707f}, rgba, UV_10},
            Vertex{{0.5f, -0.5f, 0.5f}, {0.707f, 0.707f, 0.707f}, rgba, UV_11},
            Vertex{{-0.5f, -0.5f, 0.5f}, {0.707f, 0.707f, 0.707f}, rgba, UV_01},
        },
        {
            0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6, 8, 9, 10, 11, 13, 12, 14, 16, 15, 14, 17, 16,
        },
    };
}

Mesh Sphere(glm::vec4 rgba) {
    // https://stackoverflow.com/a/47416720
    // https://github.com/PhoenixWhitefire/PhoenixEngine/blob/225a1e78f12b7743a4e4de86c0891d50404bf5a5/src/impl/asset/PrimitiveMeshes.cpp
    constexpr int HORIZONTAL_LINES = 15;
    constexpr int VERTICAL_LINES = 15;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (int y = 0; y <= HORIZONTAL_LINES; y++) {
        float v = (float)y / HORIZONTAL_LINES;
        float phi = v * M_PI;

        for (int x = 0; x <= VERTICAL_LINES; x++) {
            float u = (float)x / VERTICAL_LINES;
            float theta = u * 2.f * M_PI;

            glm::vec3 pos = glm::normalize(glm::vec3(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta)));

            vertices.push_back(Vertex{pos * 0.5f, pos, rgba, glm::vec2(u, 1.f - v)});
        }
    }

    for (int y = 0; y < HORIZONTAL_LINES; y++) {
        for (int x = 0; x < VERTICAL_LINES; x++) {
            uint32_t i0 = y * (VERTICAL_LINES + 1) + x;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = (y + 1) * (VERTICAL_LINES + 1) + x;
            uint32_t i3 = i2 + 1;

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i2);
        }
    }

    return Mesh{std::move(vertices), std::move(indices)};
}

} // namespace gargantuan::render::PrimitiveMeshes
