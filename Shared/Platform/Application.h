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

    static void StartRenderThread();

    static bool IsRenderThreadRunning();

    static void MoveInputPointerTo(int x, int y, std::uint32_t flags);

protected:

    virtual bool Init(const NativeWindow &nativeWindow);

    virtual void RenderFrame();

    virtual void Destroy();

    virtual void OnInputPointerMove(VkOffset2D delta, std::uint32_t flags);

    bool InitSemaphores(std::uint32_t count);

    static std::unique_ptr<Application> AppInstance;

    std::vector<VkSemaphore> Semaphores;

    VkSurfaceKHR VKSurface = VK_NULL_HANDLE;

    VK::Instance *VKInstance;

    VK::Device *VKDevice;

    VkOffset2D InputPointer;

    friend bool InitApp(const NativeWindow &);

private:

    static std::thread RenderThread;

    static bool Running;

};
