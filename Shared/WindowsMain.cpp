#include <Globals.h>

#include <Vulkan/Device.h>
#include <Vulkan/Instance.h>
#include <Vulkan/NativeWindow.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "VKClearScreen.h"

static VKClearScreen app;

LRESULT WINAPI WndProc(HWND _wnd, UINT _msg, WPARAM _w, LPARAM _l)
{
    LRESULT retVal = 0;
    switch (_msg)
    {
    case WM_PAINT:
        app.Render(0.f);
        break;
    case WM_SIZE:
        //SC::Renderer::Get()->ResizeView(LOWORD(_l), HIWORD(_l));
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    default:
        retVal = DefWindowProc(_wnd, _msg, _w, _l);
        break;
    }
    return retVal;
}

bool CreateNativeWindow(VK::NativeWindow &window)
{
    WNDCLASS windowClass;
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = (WNDPROC)WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = "VulkanDemo";
    if ( !RegisterClassA(&windowClass) )
    {
        return false;
    }

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    int halfWidth = GetSystemMetrics(SM_CXSCREEN) >> 1;
    int halfHeight = GetSystemMetrics(SM_CYSCREEN) >> 1;
    window.Size.width = halfWidth + (halfWidth >> 1);
    window.Size.height = halfHeight + (halfHeight >> 1);
    SetRect(&window.Bounds, 0, 0, window.Size.width, window.Size.height);
    AdjustWindowRect(&window.Bounds, style, FALSE);
    int rows = window.Bounds.bottom - window.Bounds.top;
    int cols = window.Bounds.right - window.Bounds.left;
    int left = (GetSystemMetrics(SM_CXSCREEN) - cols) >> 1;
    int top = (GetSystemMetrics(SM_CYSCREEN) - rows) >> 1;
    window.Handle = CreateWindowA("VulkanDemo", "VkClearScreen", style, left, top, cols, rows, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!window.Handle)
    {
        return false;
    }

    return true;
}

void RunNativeEventLoop(VK::NativeWindow &window)
{
    window.KeepRunning = true;
    ShowWindow(window.Handle, SW_SHOWDEFAULT);
    while (window.KeepRunning)
    {
        while (PeekMessage(&window.Message, window.Handle, 0, 0, PM_REMOVE))
        {
            if (window.Message.message != WM_QUIT)
            {
                TranslateMessage(&window.Message);
                DispatchMessage(&window.Message);
                continue;
            }
            window.KeepRunning = false;
            break;
        }
    }
}

int WINAPI WinMain(HINSTANCE _inst, HINSTANCE _prev, LPSTR _cmd, int _show)
{
    VK::NativeWindow window;
    VK::Instance vkInstance;
    if (vkInstance.Init("VulkanDemo") && CreateNativeWindow(window))
    {
        VK::Device vkDevice;
        VkSurfaceKHR vkSurface;

        if (vkInstance.CreateSurface(window, vkSurface))
        {
            VK::DeviceRequest devRequest;
            devRequest.TargetSurface = vkSurface;
            devRequest.QueueFlagBits = VK_QUEUE_GRAPHICS_BIT;
            if (vkInstance.CreateDevice(devRequest, vkDevice))
            {
                VK::SwapchainConfig swapchainConf;
                swapchainConf.Extent = window.Size;
                swapchainConf.PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                swapchainConf.SurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
                if (vkDevice.InitSwapchain(swapchainConf))
                {
                    app.Init(vkDevice);
                    RunNativeEventLoop(window);
                    app.Shutdown();
                }
            }
        }

        vkDevice.Destroy();
        vkInstance.Destroy(vkSurface);
    }
    vkInstance.Destroy();
    return 0;
}