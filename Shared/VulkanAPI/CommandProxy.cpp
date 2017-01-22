#include <Globals.h>

#include "CommandProxy.h"

#include "Device.h"

void VK::CommandProxy::BeginRenderPass(const VkRenderPassBeginInfo *info, VkSubpassContents contents)
{
    TargetDevice->vkCmdBeginRenderPass(CommandBuffer, info, contents);
}

void VK::CommandProxy::EndRenderPass()
{
    TargetDevice->vkCmdEndRenderPass(CommandBuffer);
}

void VK::CommandProxy::BindGraphicsPipeline(VkPipeline pipeline)
{
    TargetDevice->vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void VK::CommandProxy::BindComputePipeline(VkPipeline pipeline)
{
    TargetDevice->vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

void VK::CommandProxy::Draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t firstVertex, std::uint32_t firstInstace)
{
    TargetDevice->vkCmdDraw(CommandBuffer, numVertices, numInstances, firstVertex, firstInstace);
}
