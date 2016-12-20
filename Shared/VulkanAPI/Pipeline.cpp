#include <Globals.h>

#include "Pipeline.h"

namespace VK
{
    static void InitDefaults(VK::PipelineInfo &instance)
    {
        for (std::uint32_t i = 0; i < NUM_STAGES; i++)
        {
            instance.ShaderStages[i].pName = "main";
            instance.ShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            switch (i)
            {
            case VERTEX_STAGE:
                instance.ShaderStages[i].stage = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case FRAGMENT_STAGE:
                instance.ShaderStages[i].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            }
        }
        instance.VertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        instance.InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        instance.InputAssembly.primitiveRestartEnable = VK_FALSE;
        instance.ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        instance.Rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        instance.Rasterization.depthClampEnable = VK_FALSE;
        instance.Rasterization.rasterizerDiscardEnable = VK_FALSE;
        instance.Rasterization.polygonMode = VK_POLYGON_MODE_FILL;
        instance.Rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
        instance.Rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        instance.Rasterization.depthBiasEnable = VK_FALSE;
        instance.Rasterization.lineWidth = 1.f;
        instance.Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        instance.Multisampling.sampleShadingEnable = VK_FALSE;
        instance.Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        instance.Multisampling.minSampleShading = 1.f;
        instance.Multisampling.alphaToCoverageEnable = VK_FALSE;
        instance.Multisampling.alphaToOneEnable = VK_FALSE;
        instance.DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        instance.ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        instance.ColorBlending.logicOpEnable = VK_FALSE;
        instance.DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        instance.PipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        instance.CreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        instance.CreateInfo.stageCount = NUM_STAGES;
        instance.CreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        instance.CreateInfo.basePipelineIndex = -1;
    }
}

const VK::PipelineInfo & VK::PipelineInfo::GetDefaults()
{
    static VK::PipelineInfo instance = {};
    if (instance.CreateInfo.sType != VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO)
    {
        InitDefaults(instance);
    }
    return instance;
}

void VK::PipelineInfo::UpdateReferences()
{
    CreateInfo.pVertexInputState = &VertexInput;
    CreateInfo.pInputAssemblyState = &InputAssembly;
    CreateInfo.pViewportState = &ViewportState;
    CreateInfo.pRasterizationState = &Rasterization;
    CreateInfo.pMultisampleState = &Multisampling;
    if (CreateInfo.pDepthStencilState != nullptr)
    {
        CreateInfo.pDepthStencilState = &DepthStencil;
    }
    CreateInfo.pColorBlendState = &ColorBlending;
    if (CreateInfo.pDynamicState != nullptr) 
    {
        CreateInfo.pDynamicState = &DynamicState;
    }
    CreateInfo.pStages = ShaderStages;
}
