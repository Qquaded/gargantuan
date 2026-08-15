#include "gargantuan/classes/GuiObject.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/render/RenderPass.hpp"
#include "gargantuan/render/RenderPrimitives.hpp"
#include "gargantuan/render/Renderer.hpp"

#include <SDL3/SDL.h>
#include <memory>

namespace gargantuan {
	class GuiPass final : public RenderPass {
	  public:
		static constexpr const char *LABEL = "Gui";

		struct alignas(16) GuiUniforms {
			glm::vec2 ViewportSize;
		};

		FileShader Shader{
			.VertexFilepath = GetShaderPath("gui.vert"),
			.VertexUniformBufferCount = 1,
			.FragmentFilepath = GetShaderPath("gui.frag"),
			.FragmentUniformBufferCount = 0,
		};

		GuiPass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) {
			Shader.Init(gpu);

			SDL_GPUGraphicsPipelineCreateInfo info{};
			info.vertex_shader = Shader.VertexShader;
			info.fragment_shader = Shader.FragmentShader;

			info.vertex_input_state.vertex_attributes = UIVertex::Attributes->data();
			info.vertex_input_state.num_vertex_attributes = static_cast<Uint32>(UIVertex::Attributes->size());
			info.vertex_input_state.vertex_buffer_descriptions = UIVertex::BufferDescriptions->data();
			info.vertex_input_state.num_vertex_buffers = static_cast<Uint32>(UIVertex::BufferDescriptions->size());

			info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
			info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
			info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
			info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

			info.depth_stencil_state.enable_depth_test = false;
			info.depth_stencil_state.enable_depth_write = false;

			SDL_GPUColorTargetDescription colorTarget{};
			{
				colorTarget.format = swapchainFormat;
				colorTarget.blend_state.enable_blend = true;
				colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
				colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
				colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
				colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
				colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
				colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
			}
			info.target_info.num_color_targets = 1;
			info.target_info.color_target_descriptions = &colorTarget;

			info.target_info.depth_stencil_format = RENDERER_DEPTH_FORMAT;
			info.target_info.has_depth_stencil_target = false;

			Pipeline = SDL_CreateGPUGraphicsPipeline(gpu, &info);
		};

		SDL_GPURenderPass *Draw(SDL_GPUDevice *gpu, FrameContext &context) override {
			std::vector<UIVertex> vertices;
			std::vector<uint32_t> indices;

			for (auto &layer : context.Layers) {
				PushChildGuiObjects(layer, vertices, indices);
			}

			if (vertices.empty()) return nullptr;

			SDL_GPUBuffer *vertexBuffer, *indexBuffer;
			UploadBuffers(gpu, context, vertices, indices, vertexBuffer, indexBuffer);

			SDL_GPUColorTargetInfo colorTarget = {
				.texture = context.SwapchainTexture,
				.load_op = SDL_GPU_LOADOP_LOAD,
				.store_op = SDL_GPU_STOREOP_STORE,
			};

			SDL_GPURenderPass *pass = SDL_BeginGPURenderPass(context.Commands, &colorTarget, 1, nullptr);
			SDL_BindGPUGraphicsPipeline(pass, Pipeline);

			GuiUniforms uniforms{
				.ViewportSize = {
					static_cast<float>(context.Width),
					static_cast<float>(context.Height),
				}
			};
			SDL_PushGPUVertexUniformData(context.Commands, 0, &uniforms, sizeof(GuiUniforms));

			SDL_GPUBufferBinding vertexBinding{.buffer = vertexBuffer, .offset = 0};
			SDL_BindGPUVertexBuffers(pass, 0, &vertexBinding, 1);

			SDL_GPUBufferBinding indexBinding{.buffer = indexBuffer, .offset = 0};
			SDL_BindGPUIndexBuffer(pass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

			SDL_DrawGPUIndexedPrimitives(pass, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

			SDL_ReleaseGPUBuffer(gpu, vertexBuffer);
			SDL_ReleaseGPUBuffer(gpu, indexBuffer);

			return pass;
		};

	  private:
		void PushChildGuiObjects(
			std::shared_ptr<Instance> parent, std::vector<UIVertex> &vertices, std::vector<uint32_t> &indices
		) {
			for (auto &child : parent->GetChildren()) {
				if (auto childGui = std::dynamic_pointer_cast<GuiObject>(child)) {
					PushGuiObject(childGui, vertices, indices);
				}
			}
		}

		void PushGuiObject(
			std::shared_ptr<GuiObject> object, std::vector<UIVertex> &vertices, std::vector<uint32_t> &indices
		) {
			if (!object->GetVisible()) return;

			Rect bounds = object->CalculateAbsoluteBounds();
			uint32_t baseIndex = static_cast<uint32_t>(vertices.size());

			glm::vec2 min = bounds.Min, max = bounds.Max;
			glm::vec2 size = bounds.GetSize();
			glm::vec4 background = {
				(glm::vec3)object->GetBackgroundColor3(),
				1.0f - object->GetBackgroundTransparency(),
			};
			float rotation = object->GetRotation();
			int textureIndex = UI_SOLID_COLOR_INDEX;

			vertices.push_back(
				UIVertex{
					.AbsolutePosition = min,
					.AbsoluteSize = size,
					.UV = {0.0f, 0.0f},
					.Background = background,
					.Rotation = rotation,
					.TextureIndex = textureIndex,
				}
			);

			vertices.push_back(
				UIVertex{
					.AbsolutePosition = {max.x, min.y},
					.AbsoluteSize = size,
					.UV = {1.0f, 0.0f},
					.Background = background,
					.Rotation = rotation,
					.TextureIndex = textureIndex,
				}
			);

			vertices.push_back(
				UIVertex{
					.AbsolutePosition = {min.x, max.y},
					.AbsoluteSize = size,
					.UV = {0.0f, 1.0f},
					.Background = background,
					.Rotation = rotation,
					.TextureIndex = textureIndex,
				}
			);

			vertices.push_back(
				UIVertex{
					.AbsolutePosition = max,
					.AbsoluteSize = size,
					.UV = {1.0f, 1.0f},
					.Background = background,
					.Rotation = rotation,
					.TextureIndex = textureIndex,
				}
			);

			indices.push_back(baseIndex + 0);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 1);

			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);

			PushChildGuiObjects(object, vertices, indices);
		}

		void UploadBuffers(
			SDL_GPUDevice *gpu,
			FrameContext &context,
			std::vector<UIVertex> &vertices,
			std::vector<uint32_t> &indices,
			SDL_GPUBuffer *&vertexBuffer,
			SDL_GPUBuffer *&indexBuffer
		) {
			uint32_t vertexBufferSize = static_cast<uint32_t>(vertices.size() * sizeof(UIVertex));
			uint32_t indexBufferSize = static_cast<uint32_t>(indices.size() * sizeof(uint32_t));

			SDL_GPUBufferCreateInfo vertexBufferInfo{.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = vertexBufferSize};
			SDL_GPUBufferCreateInfo indexBufferInfo{.usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = indexBufferSize};
			vertexBuffer = SDL_CreateGPUBuffer(gpu, &vertexBufferInfo);
			indexBuffer = SDL_CreateGPUBuffer(gpu, &indexBufferInfo);

			SDL_GPUTransferBufferCreateInfo tBufferInfo{
				.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
				.size = vertexBufferSize + indexBufferSize,
			};
			SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(gpu, &tBufferInfo);

			uint8_t *mapped = static_cast<uint8_t *>(SDL_MapGPUTransferBuffer(gpu, transferBuffer, false));
			std::memcpy(mapped, vertices.data(), vertexBufferSize);
			std::memcpy(mapped + vertexBufferSize, indices.data(), indexBufferSize);
			SDL_UnmapGPUTransferBuffer(gpu, transferBuffer);

			SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(context.Commands);

			SDL_GPUTransferBufferLocation vertexSource{.transfer_buffer = transferBuffer, .offset = 0};
			SDL_GPUBufferRegion vertexDestination{.buffer = vertexBuffer, .offset = 0, .size = vertexBufferSize};
			SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, false);

			SDL_GPUTransferBufferLocation indexSource{.transfer_buffer = transferBuffer, .offset = vertexBufferSize};
			SDL_GPUBufferRegion indexDestination{.buffer = indexBuffer, .offset = 0, .size = indexBufferSize};
			SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, false);

			SDL_EndGPUCopyPass(copyPass);
			SDL_ReleaseGPUTransferBuffer(gpu, transferBuffer);
		}
	};

	std::unique_ptr<RenderPass> CreateGuiPass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) {
		return std::make_unique<GuiPass>(gpu, swapchainFormat);
	}
}
