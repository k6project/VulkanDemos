#pragma once

#include <vulkan.h>

#ifdef VK_USE_PLATFORM_WIN32_KHR
#define NATIVE_WINDOW_EXT_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace VK
{

    struct NativeWindow
    {
#ifdef VK_USE_PLATFORM_WIN32_KHR
#define NATIVE_WINDOW_EXT_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
        HWND Handle;
        RECT Bounds;
        MSG Message;
        bool KeepRunning;
#endif
        VkExtent2D Size;
    };

}
