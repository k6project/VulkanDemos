#include <Globals.h>

#include "Buffer.h"

#include "Device.h"

VK::Buffer::operator const VkBuffer&() const
{
    return Handle;
}

void * VK::Buffer::Map() const
{
    void *result = nullptr;
    TargetDevice->vkMapMemory(TargetDevice->Handle, Memory, 0, Size, 0, &result);
    return result;
}

void VK::Buffer::Unmap() const
{
    TargetDevice->vkUnmapMemory(TargetDevice->Handle, Memory);
}
