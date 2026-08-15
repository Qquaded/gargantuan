#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <glm/glm.hpp>

namespace gargantuan {
	struct Vertex {
	  public:
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 UV;

		static std::array<SDL_GPUVertexBufferDescription, 1> BufferDescriptions[];
		static std::array<SDL_GPUVertexAttribute, 3> Attributes[];
	};

	static constexpr int UI_SOLID_COLOR_INDEX = -1;
	struct UIVertex {
	  public:
		glm::vec2 AbsolutePosition;
		glm::vec2 AbsoluteSize;
		glm::vec2 UV;
		glm::vec4 Color = {0.0f, 0.0f, 0.0f, 0.0f};
		float Rotation = 0;
		int TextureIndex = UI_SOLID_COLOR_INDEX;

		static std::array<SDL_GPUVertexBufferDescription, 1> BufferDescriptions[];
		static std::array<SDL_GPUVertexAttribute, 5> Attributes[];
	};

	struct Mesh {
	  public:
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};
}; // namespace gargantuan
