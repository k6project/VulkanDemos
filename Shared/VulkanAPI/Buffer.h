#pragma once

#include <vulkan/vulkan.h>

namespace VK
{

    class Device;

    struct BufferRequest
    {
        std::size_t Size = 0;
        VkBufferUsageFlags Usage = 0;
        std::uint32_t Access = 0;
        bool IsShared = false;
    };

    class Buffer
    {

    public:

        enum Type 
        {
            VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            INDEX  = VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        };

        operator const VkBuffer &() const;

        void * Map() const;

        void Unmap() const;

    private:

        VkBuffer Handle = VK_NULL_HANDLE;

        VkDeviceMemory Memory = VK_NULL_HANDLE;
        
        Device *TargetDevice = nullptr;

        std::size_t Size = 0;

        friend class VK::Device;
    
    };

}
