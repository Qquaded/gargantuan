#pragma once

#include <vector>
#include <vulkan/vulkan.h>

// https://vkguide.dev/docs/new_chapter_3/building_pipeline/

class VkPipeline
{

public:
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo multisampler;

    VkPipelineLayout layout;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    VkPipelineRenderingCreateInfo render;
    VkFormat colorAttachmentFormat;

    VkPipeline()
    {
        Clear();
    }

    void Clear();
    VkPipeline Build(VkDevice device);
};
