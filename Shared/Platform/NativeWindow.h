#pragma once

#include <vulkan/vulkan.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#define MOUSE_MOVE_THRESHOLD 4

enum : std::uint32_t
{
    LBUTTON_DOWN = 1,  
    RBUTTON_DOWN = 2
};

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
