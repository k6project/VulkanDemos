#include <Globals.h>

#include "Application.h"

#include <VulkanAPI/Device.h>
#include <VulkanAPI/Instance.h>
#include <VulkanAPI/DeviceRequest.h>
#include <Platform/NativeWindow.h>

std::unique_ptr<Application> Application::AppInstance;
std::thread Application::RenderThread;
bool Application::Running = false;

void Application::Shutdown()
{
    if (AppInstance)
    {
        Running = false;
        RenderThread.join();
        AppInstance->Destroy();
        AppInstance.reset();
    }
}

void Application::NextFrame()
{
    if (AppInstance)
    {
        AppInstance->RenderFrame();
    }
}

void Application::StartRenderThread()
{
    Running = true;
    std::thread newThread([]()
    {
        while (Application::IsRenderThreadRunning())
        {
            Application::NextFrame();
        }
    });
    std::swap(RenderThread, newThread);
}

bool Application::IsRenderThreadRunning()
{
    return Running;
}

void Application::MoveInputPointerTo(int x, int y, std::uint32_t flags)
{
    if (AppInstance)
    {
        VkOffset2D newPos = { x, y };
        VkOffset2D oldPos = AppInstance->InputPointer;
        if (oldPos.x == -1 && oldPos.y == -1)
        {
            AppInstance->InputPointer = newPos;
        }
        else
        {
            bool change = false;
            VkOffset2D delta = { (oldPos.x - x), (oldPos.y - y) };
            if (abs(delta.x) > MOUSE_MOVE_THRESHOLD)
            {
                AppInstance->InputPointer.x = x;
                change = true;
            }
            else
            {
                delta.x = 0;
            }
            if (abs(delta.y) > MOUSE_MOVE_THRESHOLD)
            {
                AppInstance->InputPointer.y = y;
                change = true;
            }
            else
            {
                delta.y = 0;
            }
            if (change)
            {
                AppInstance->OnInputPointerMove(delta, flags);
            }
        }
    }
}

bool Application::Init(const NativeWindow &nativeWindow)
{
    InputPointer = { -1, -1 };
    VKInstance = new VK::Instance;
    if (VKInstance->Init("VulkanDemo"))
    {
        VKDevice= new VK::Device;

        if (VKInstance->CreateSurface(nativeWindow, VKSurface))
        {
            VK::DeviceRequest devRequest;
            devRequest.TargetSurface = VKSurface;
            devRequest.QueueFlagBits = VK_QUEUE_GRAPHICS_BIT;
            if (VKInstance->CreateDevice(devRequest, *VKDevice))
            {
                VK::SwapchainConfig swapchainConf;
                swapchainConf.Extent = nativeWindow.Size;
                swapchainConf.PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                swapchainConf.SurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
                if (VKDevice->InitSwapchain(swapchainConf))
                {
                    AppInstance.reset(this);
                    return true;
                }
            }
        }
    }
    return false;
}

void Application::RenderFrame()
{
    //By default does nothing, re-implement in subclasses
}

void Application::Destroy()
{
    VKDevice->Destroy(Semaphores);
    VKDevice->Destroy();
    VKInstance->Destroy(VKSurface);
    VKInstance->Destroy();
    delete VKDevice;
    delete VKInstance;
}

void Application::OnInputPointerMove(VkOffset2D delta, std::uint32_t flags)
{
    //By default does nothing, re-implement in subclasses
}

bool Application::InitSemaphores(std::uint32_t count)
{
    Semaphores.resize(count);
    return VKDevice->CreateSemaphores(Semaphores);
}
