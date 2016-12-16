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
