#include "gargantuan/render/PrimitiveMeshes.hpp"
#include <ext/vector_float4.hpp>

namespace gargantuan::render::PrimitiveMeshes {

constexpr auto BALL_GAME = glm::vec4(1.0, 0.0, 0.0, 1.0);
constexpr auto ORANGE = glm::vec4(1.0, 0.5, 0.0, 1.0);
constexpr auto YELLOW = glm::vec4(1.0, 1.0, 0.0, 1.0);
constexpr auto GREEN = glm::vec4(0.0, 1.0, 0.0, 1.0);
constexpr auto AQUA = glm::vec4(0.0, 1.0, 1.0, 1.0);
constexpr auto SETH = glm::vec4(0.75, 0.0, 1.0, 1.0);

Mesh Block(glm::vec4 rgba) {
    // GPU Programming Gone Too Far?
    // https://discord.com/channels/1303725891259596810/1303725892349984830/1527742812676685875
    static const Vertex vertices[]{
        // Front
        Vertex{glm::vec3(1, 1, 1), BALL_GAME},
        Vertex{glm::vec3(-1, 1, 1), BALL_GAME},
        Vertex{glm::vec3(1, -1, 1), BALL_GAME},
        Vertex{glm::vec3(-1, 1, 1), BALL_GAME},
        Vertex{glm::vec3(1, -1, 1), BALL_GAME},
        Vertex{glm::vec3(-1, -1, 1), BALL_GAME},
        // Back
        Vertex{glm::vec3(1, 1, -1), ORANGE},
        Vertex{glm::vec3(-1, 1, -1), ORANGE},
        Vertex{glm::vec3(1, -1, -1), ORANGE},
        Vertex{glm::vec3(-1, 1, -1), ORANGE},
        Vertex{glm::vec3(-1, -1, -1), ORANGE},
        Vertex{glm::vec3(1, -1, -1), ORANGE},
        // Left
        Vertex{glm::vec3(1, 1, 1), YELLOW},
        Vertex{glm::vec3(1, 1, -1), YELLOW},
        Vertex{glm::vec3(1, -1, 1), YELLOW},
        Vertex{glm::vec3(1, 1, -1), YELLOW},
        Vertex{glm::vec3(1, -1, -1), YELLOW},
        Vertex{glm::vec3(1, -1, 1), YELLOW},
        // Right
        Vertex{glm::vec3(-1, 1, -1), GREEN},
        Vertex{glm::vec3(-1, 1, 1), GREEN},
        Vertex{glm::vec3(-1, -1, -1), GREEN},
        Vertex{glm::vec3(-1, 1, 1), GREEN},
        Vertex{glm::vec3(-1, -1, -1), GREEN},
        Vertex{glm::vec3(-1, -1, 1), GREEN},
        // Top
        Vertex{glm::vec3(1, 1, -1), AQUA},
        Vertex{glm::vec3(-1, 1, -1), AQUA},
        Vertex{glm::vec3(1, 1, 1), AQUA},
        Vertex{glm::vec3(-1, 1, -1), AQUA},
        Vertex{glm::vec3(1, 1, 1), AQUA},
        Vertex{glm::vec3(-1, 1, 1), AQUA},
        // Bottom
        Vertex{glm::vec3(1, -1, -1), SETH},
        Vertex{glm::vec3(-1, -1, -1), SETH},
        Vertex{glm::vec3(1, -1, 1), SETH},
        Vertex{glm::vec3(-1, -1, -1), SETH},
        Vertex{glm::vec3(1, -1, 1), SETH},
        Vertex{glm::vec3(-1, -1, 1), SETH},
    };

    return Mesh{vertices, 6 * 6};
};

} // namespace gargantuan::render::PrimitiveMeshes
