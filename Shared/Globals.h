#pragma once

#include <memory>
#include <thread>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#define VK_NO_PROTOTYPES
#define VK_NONE 0xffffffffu

#ifdef _DEBUG
#define VK_DEBUG_ON
#endif

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NATIVE_WINDOW_EXT_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif

#define APP_MAIN(ClsName) \
extern bool InitApp(const NativeWindow &nativeWindow) \
{\
    Application *app = new ClsName();\
    return app->Init(nativeWindow);\
}
