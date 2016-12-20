#pragma once

#include <vulkan/vulkan.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

struct NativeWindow
{
#ifdef _WIN32
    HWND Handle;
    RECT Bounds;
    MSG Message;
    bool KeepRunning;
#endif
    VkExtent2D Size;
};
