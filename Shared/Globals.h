#pragma once

#include <string>
#include <vector>
#include <cstdint>

#define VK_NO_PROTOTYPES
#define VK_NONE 0xffffffffu

#ifdef _DEBUG
#define VK_DEBUG_ON
#endif

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
