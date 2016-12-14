#pragma once

#include <vulkan.h>

namespace VK
{

    class Device;

    class CommandProxy
    {

    public:

        void BeginRenderPass(const VkRenderPassBeginInfo *info, VkSubpassContents contents);

        void EndRenderPass();

    private:

        VkCommandBuffer CommandBuffer;

        Device *TargetDevice;

        friend class Device;

    };

}
