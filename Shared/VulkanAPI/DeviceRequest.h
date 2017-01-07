#pragma once

#include <vulkan/vulkan.h>

namespace VK
{
    
    // Have a int array of TOTAL_QUEUES size, can be set to - 1, 0, or 1
        // If -1: no queue of that type is needed, will be VK_NULL_HANDLE
        // If 0: will fill this slot with another compatible queue, or create new one if none found (No error)
        // If 1: same as 0, but produces an error if there was no compatible queue at all
        // If 2: will create a separate queue for this slot, even if compatible ones are present
        // 
    struct DeviceRequest
    {
        std::uint32_t QueueFlagBits = 0u;
        VkSurfaceKHR TargetSurface = VK_NULL_HANDLE;
        VkPhysicalDeviceFeatures Features = {};
        std::vector<const char *> Extensions;

        typedef std::vector<VkQueueFamilyProperties> FamiliesList;
        bool FindQueues(const FamiliesList &families, std::function<bool(std::uint32_t)> canPresent) const;
    };

}
