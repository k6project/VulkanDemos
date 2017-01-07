#pragma once

#include <vulkan/vulkan.h>

namespace VK
{

    class Device;

    class Buffer
    {
        VkBuffer Handle = VK_NULL_HANDLE;
        VkDeviceMemory Memory = VK_NULL_HANDLE;
        friend class VK::Device;
    };

}
