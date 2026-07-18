#include "gargantuan/render/PrimitiveMeshes.hpp"
#include <ext/vector_float4.hpp>

namespace gargantuan::render::PrimitiveMeshes {

Mesh Block(glm::vec4 rgba) {
    // GPU Programming Gone Too Far?
    // https://discord.com/channels/1303725891259596810/1303725892349984830/1527742812676685875
    static const Vertex vertices[]{
        // Front
        Vertex{glm::vec3(1, 1, 1), glm::vec3(0, 0, 1), rgba},
        Vertex{glm::vec3(-1, 1, 1), glm::vec3(0, 0, 1), rgba},
        Vertex{glm::vec3(1, -1, 1), glm::vec3(0, 0, 1), rgba},
        Vertex{glm::vec3(-1, 1, 1), glm::vec3(0, 0, 1), rgba},
        Vertex{glm::vec3(1, -1, 1), glm::vec3(0, 0, 1), rgba},
        Vertex{glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1), rgba},
        // Back
        Vertex{glm::vec3(1, 1, -1), glm::vec3(0, 0, -1), rgba},
        Vertex{glm::vec3(-1, 1, -1), glm::vec3(0, 0, -1), rgba},
        Vertex{glm::vec3(1, -1, -1), glm::vec3(0, 0, -1), rgba},
        Vertex{glm::vec3(-1, 1, -1), glm::vec3(0, 0, -1), rgba},
        Vertex{glm::vec3(-1, -1, -1), glm::vec3(0, 0, -1), rgba},
        Vertex{glm::vec3(1, -1, -1), glm::vec3(0, 0, -1), rgba},
        // Left
        Vertex{glm::vec3(1, 1, 1), glm::vec3(-1, 0, 0), rgba},
        Vertex{glm::vec3(1, 1, -1), glm::vec3(-1, 0, 0), rgba},
        Vertex{glm::vec3(1, -1, 1), glm::vec3(-1, 0, 0), rgba},
        Vertex{glm::vec3(1, 1, -1), glm::vec3(-1, 0, 0), rgba},
        Vertex{glm::vec3(1, -1, -1), glm::vec3(-1, 0, 0), rgba},
        Vertex{glm::vec3(1, -1, 1), glm::vec3(-1, 0, 0), rgba},
        // Right
        Vertex{glm::vec3(-1, 1, -1), glm::vec3(1, 0, 0), rgba},
        Vertex{glm::vec3(-1, 1, 1), glm::vec3(1, 0, 0), rgba},
        Vertex{glm::vec3(-1, -1, -1), glm::vec3(1, 0, 0), rgba},
        Vertex{glm::vec3(-1, 1, 1), glm::vec3(1, 0, 0), rgba},
        Vertex{glm::vec3(-1, -1, -1), glm::vec3(1, 0, 0), rgba},
        Vertex{glm::vec3(-1, -1, 1), glm::vec3(1, 0, 0), rgba},
        // Top
        Vertex{glm::vec3(1, 1, -1), glm::vec3(0, 1, 0), rgba},
        Vertex{glm::vec3(-1, 1, -1), glm::vec3(0, 1, 0), rgba},
        Vertex{glm::vec3(1, 1, 1), glm::vec3(0, 1, 0), rgba},
        Vertex{glm::vec3(-1, 1, -1), glm::vec3(0, 1, 0), rgba},
        Vertex{glm::vec3(1, 1, 1), glm::vec3(0, 1, 0), rgba},
        Vertex{glm::vec3(-1, 1, 1), glm::vec3(0, 1, 0), rgba},
        // Bottom
        Vertex{glm::vec3(1, -1, -1), glm::vec3(0, -1, 0), rgba},
        Vertex{glm::vec3(-1, -1, -1), glm::vec3(0, -1, 0), rgba},
        Vertex{glm::vec3(1, -1, 1), glm::vec3(0, -1, 0), rgba},
        Vertex{glm::vec3(-1, -1, -1), glm::vec3(0, -1, 0), rgba},
        Vertex{glm::vec3(1, -1, 1), glm::vec3(0, -1, 0), rgba},
        Vertex{glm::vec3(-1, -1, 1), glm::vec3(0, -1, 0), rgba},
    };

    return Mesh{vertices, 6 * 6};
};

} // namespace gargantuan::render::PrimitiveMeshes
