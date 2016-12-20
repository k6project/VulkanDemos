#include <Globals.h>

#include "Application.h"

#include <VulkanAPI/Device.h>
#include <VulkanAPI/Instance.h>
#include <Platform/NativeWindow.h>

std::unique_ptr<Application> Application::AppInstance;

void Application::Shutdown()
{
    if (AppInstance)
    {
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

bool Application::Init(const NativeWindow &nativeWindow)
{
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

bool Application::InitSemaphores(std::uint32_t count)
{
    Semaphores.resize(count);
    return VKDevice->CreateSemaphores(Semaphores);
}
