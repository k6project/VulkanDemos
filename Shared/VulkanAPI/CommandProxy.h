#pragma once

#include <vulkan/vulkan.h>

namespace VK
{

    class Device;

    class CommandProxy
    {

    public:

        void BeginRenderPass(const VkRenderPassBeginInfo *info, VkSubpassContents contents);

        void EndRenderPass();

        void BindGraphicsPipeline(VkPipeline pipeline);

        void Draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t firstVertex, std::uint32_t firstInstace);

    private:

        VkCommandBuffer CommandBuffer;

        Device *TargetDevice;

        friend class Device;

    };

}
