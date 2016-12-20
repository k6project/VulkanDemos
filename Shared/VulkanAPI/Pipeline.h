#pragma once

#include <vulkan/vulkan.h>

enum ShaderStages : std::uint32_t
{ 
    VERTEX_STAGE, FRAGMENT_STAGE, NUM_STAGES 
};

namespace VK
{

    struct PipelineInfo
    {
        static const PipelineInfo & GetDefaults();

        // For each entry set the shader module handle
        VkPipelineShaderStageCreateInfo ShaderStages[NUM_STAGES];

        // Bindings and attributes need to be provided (if vertex data is present)
        VkPipelineVertexInputStateCreateInfo VertexInput;

        // Specify vertex topology, and if needed, primitive restart (false by default)
        VkPipelineInputAssemblyStateCreateInfo InputAssembly;

        // Specify arrays of viewports and scissor rects (all 0 by default)
        VkPipelineViewportStateCreateInfo ViewportState;

        VkPipelineRasterizationStateCreateInfo Rasterization;

        VkPipelineMultisampleStateCreateInfo Multisampling;

        VkPipelineDepthStencilStateCreateInfo DepthStencil;

        VkPipelineColorBlendStateCreateInfo ColorBlending;

        VkPipelineDynamicStateCreateInfo DynamicState;

        VkPipelineLayoutCreateInfo PipelineLayout;

        VkGraphicsPipelineCreateInfo CreateInfo;

        void UpdateReferences();
    };

}
