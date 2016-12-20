#pragma once

#include <vulkan/vulkan.h>

namespace VK
{
    class Instance;
    class Device;
}

struct NativeWindow;

extern bool InitApp(const NativeWindow &nativeWindow);

class Application
{

public:

    static void Shutdown();

    static void NextFrame();

protected:

    virtual bool Init(const NativeWindow &nativeWindow);

    virtual void RenderFrame();

    virtual void Destroy();

    bool InitSemaphores(std::uint32_t count);

    static std::unique_ptr<Application> AppInstance;

    std::vector<VkSemaphore> Semaphores;

    VkSurfaceKHR VKSurface = VK_NULL_HANDLE;

    VK::Instance *VKInstance;

    VK::Device *VKDevice;

    friend bool InitApp(const NativeWindow &);

};
