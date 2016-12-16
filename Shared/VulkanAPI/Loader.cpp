#include <Globals.h>

#include <vulkan/vulkan.h>

#if defined(VK_USE_PLATFORM_WIN32_KHR)

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
typedef HMODULE DllHandle;
#define LoadVulkanDLL() LoadLibrary("vulkan-1.dll")
#define DllProcAddr GetProcAddress

#else

#include <dlfcn.h>
typedef void *DllHandle;
#define LoadVulkanDLL() dlopen("libvulkan.so", RTLD_NOW)
#define DllProcAddr dlsym

#endif

static DllHandle GLibrary = nullptr;

static DllHandle VKLoadSharedLib()
{
    static DllHandle dll = nullptr;
    if (!dll)
    {
        dll = LoadVulkanDLL();
    }
    return dll;
}

extern bool LoadVulkanLibrary(PFN_vkGetInstanceProcAddr &procAddr)
{
    if (!GLibrary)
    {
        GLibrary = LoadVulkanDLL();
    }
    if (GLibrary)
    {
        PFN_vkGetInstanceProcAddr result = reinterpret_cast<PFN_vkGetInstanceProcAddr>(DllProcAddr(GLibrary, "vkGetInstanceProcAddr"));
        if (result)
        { 
            procAddr = result;
            return true;
        }
    }
    return false;
}