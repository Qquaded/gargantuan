#include "gargantuan/render/PrimitiveMeshes.hpp"

#include <vector>

namespace gargantuan::render::PrimitiveMeshes {

Mesh Block(glm::vec4 rgba) {
    // GPU Programming Gone Too Far?
    // https://discord.com/channels/1303725891259596810/1303725892349984830/1527742812676685875
    return Mesh{std::vector<Vertex>{
                    Vertex{glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), rgba},
                    Vertex{glm::vec3(-1, 1, 1), glm::vec3(-1, 1, 1), rgba},
                    Vertex{glm::vec3(1, 1, -1), glm::vec3(1, 1, -1), rgba},
                    Vertex{glm::vec3(-1, 1, -1), glm::vec3(-1, 1, -1), rgba},
                    Vertex{glm::vec3(1, -1, 1), glm::vec3(1, -1, 1), rgba},
                    Vertex{glm::vec3(-1, -1, 1), glm::vec3(-1, -1, 1), rgba},
                    Vertex{glm::vec3(1, -1, -1), glm::vec3(1, -1, -1), rgba},
                    Vertex{glm::vec3(-1, -1, -1), glm::vec3(-1, -1, -1), rgba},
                },
                std::vector<uint32_t>{// front
                                      0, 1, 4, 1, 4, 5,
                                      // back
                                      2, 3, 6, 3, 6, 7,
                                      // left
                                      0, 2, 6, 0, 6, 4,
                                      // right
                                      1, 3, 7, 1, 7, 5,
                                      // top
                                      0, 1, 2, 1, 2, 3,
                                      // bottom
                                      4, 5, 6, 5, 6, 7

                }};
};

} // namespace gargantuan::render::PrimitiveMeshes
