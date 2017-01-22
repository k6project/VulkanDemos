#include <Globals.h>
#include <Platform/NativeWindow.h>

#include "Instance.h"
#include "Device.h"
#include "DeviceRequest.h"

#include <set>
#include <unordered_set>

extern bool LoadVulkanLibrary(PFN_vkGetInstanceProcAddr &procAddr);

bool VK::Instance::Init(const std::string &engineName)
{
    if (LoadVulkanLibrary(vkGetInstanceProcAddr))
    {
#define VK_CALL(cmd, err) do { VkResult r = cmd; if(r!=VK_SUCCESS) { return false; } } while(0)
        VkApplicationInfo ApplicationInfo = {};
        VkInstanceCreateInfo InstanceCreateInfo = {};
        
#ifdef VK_DEBUG_ON
        const char *debugLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
        std::uint32_t numDebugLayers = std::extent<decltype(debugLayers)>::value;
        const char *extensionNames[] = {
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#else
        const char **debugLayers = nullptr;
        std::uint32_t numDebugLayers = 0;
        const char *extensionNames[] = {
#endif
            VK_KHR_SURFACE_EXTENSION_NAME,
            NATIVE_WINDOW_EXT_NAME
        };

        ApplicationInfo.pEngineName = engineName.c_str();
        ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
        ApplicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
        InstanceCreateInfo.ppEnabledLayerNames = debugLayers;
        InstanceCreateInfo.ppEnabledExtensionNames = extensionNames;
        InstanceCreateInfo.enabledLayerCount = numDebugLayers;
        InstanceCreateInfo.enabledExtensionCount = std::extent<decltype(extensionNames)>::value;

        #define VULKAN_API_GOBAL(fn) if ( !(fn = reinterpret_cast<PFN_##fn>(vkGetInstanceProcAddr(nullptr, #fn))) ) return false;
        #include "Instance.inl"
        VK_CALL(vkCreateInstance(&InstanceCreateInfo, nullptr, &Handle), "Failed to create Vulkan instance");

        #define VULKAN_API_INSTANCE(fn) if ( !(fn = reinterpret_cast<PFN_##fn>(vkGetInstanceProcAddr(Handle, #fn))) ) return false;
        #include "Instance.inl"
        VK_CALL(InstallDebugCallback(), "Failed to install debug callback");

        std::uint32_t numDevices = 0;
        VK_CALL(vkEnumeratePhysicalDevices(Handle, &numDevices, nullptr), "Failed to enumerate physical devices");
        if (numDevices > 0)
        {
            PhysicalDevices.resize(numDevices);
            PhysicalDevicesInfo.resize(numDevices);
            VK_CALL(vkEnumeratePhysicalDevices(Handle, &numDevices, PhysicalDevices.data()), "Failed to enumerate physical devices");
            for (std::uint32_t i = 0; i < numDevices; i++)
            {
                GetPhysicalDeviceInfo(i);
            }
        }

        #undef VK_CALL
        return true;
    }
    return false;
}

bool VK::Instance::CreateSurface(const NativeWindow &window, VkSurfaceKHR &surface)
{
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
#ifdef VK_USE_PLATFORM_WIN32_KHR
    createInfo.hwnd = window.Handle;
    createInfo.hinstance = GetModuleHandle(nullptr);
    if (vkCreateWin32SurfaceKHR(Handle, &createInfo, nullptr, &surface) != VK_SUCCESS)
    {
        return false;
    }
#endif
    return true;
}

bool VK::Instance::CreateDevice(const DeviceRequest &request, Device &device)
{
    bool result = false;
    float queuePriority = 1.f;
#ifdef VK_DEBUG_ON
    const char *debugLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
    std::uint32_t numDebugLayers = std::extent<decltype(debugLayers)>::value;
#else
    const char **debugLayers = nullptr;
    std::uint32_t numDebugLayers = 0;
#endif

    std::vector<const char *> extensions(request.Extensions);
    if (request.TargetSurface != VK_NULL_HANDLE)
    {
        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    for (std::uint32_t i = 0; i < PhysicalDevices.size(); i++)
    {
        DeviceMatch match = MatchDevice(i, request);
        if (match.IsValid)
        {
            std::vector<VkDeviceQueueCreateInfo> queuesInfo;
            std::set<std::uint32_t> families = { match.GraphicsQueueFamily, match.PresentQueueFamily };
            for (std::uint32_t index : families)
            {
                queuesInfo.emplace_back();
                queuesInfo.back().sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queuesInfo.back().queueFamilyIndex = index;
                queuesInfo.back().queueCount = 1;
                queuesInfo.back().pQueuePriorities = &queuePriority;
                queuesInfo.back().pNext = nullptr;
                queuesInfo.back().flags = 0;
            }
            VkDeviceCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pQueueCreateInfos = queuesInfo.data();
            createInfo.pEnabledFeatures = &request.Features;
            createInfo.queueCreateInfoCount = queuesInfo.size() & 0xffffffffu;
            createInfo.ppEnabledLayerNames = debugLayers;
            createInfo.ppEnabledExtensionNames = extensions.data();
            createInfo.enabledLayerCount = numDebugLayers;
            createInfo.enabledExtensionCount = extensions.size() & 0xffffffffu;
            if (vkCreateDevice(PhysicalDevices[i], &createInfo, nullptr, &device.Handle) == VK_SUCCESS)
            {
                #define VULKAN_API_DEVICE(fn) if ( !(device.##fn = reinterpret_cast<PFN_##fn>(vkGetDeviceProcAddr(device.Handle, #fn))) ) continue;
                #include "Device.inl"
                result = true;
                device.Surface = request.TargetSurface;
                if (device.Surface != VK_NULL_HANDLE)
                {
                    std::uint32_t tmp = 0;
                    device.PresentModes.resize(match.NumPresentModes);
                    device.SurfaceFormats.resize(match.NumSurfaceFormats);
                    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevices[i], device.Surface, &device.SurfaceCapabilities);
                    tmp = device.SurfaceFormats.size() & 0xffffffffu;
                    vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevices[i], device.Surface, &tmp, device.SurfaceFormats.data());
                    tmp = device.PresentModes.size() & 0xffffffffu;
                    vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevices[i], device.Surface, &tmp, device.PresentModes.data());
                }
                device.MemoryProperties = &PhysicalDevicesInfo[i].MemoryProperties;
                result = result && device.InitQueue(Device::GRAPHICS_QUEUE, match.GraphicsQueueFamily);
                result = result && device.InitQueue(Device::PRESENT_QUEUE, match.PresentQueueFamily);
                break;
            }
        }
    }
    return result;
}

void VK::Instance::Destroy(VkSurfaceKHR object)
{
    vkDestroySurfaceKHR(Handle, object, nullptr);
}

void VK::Instance::Destroy()
{
#ifdef VK_DEBUG_ON
    if (OnErrorCallback != VK_NULL_HANDLE)
    {
        vkDestroyDebugReportCallbackEXT(Handle, OnErrorCallback, nullptr);
    }
#endif
    vkDestroyInstance(Handle, nullptr);
}

VK::Instance::DeviceMatch VK::Instance::MatchDevice(std::uint32_t index, const DeviceRequest &request)
{
    DeviceMatch result = { 0, 0, 0, 0, false };
    std::uint32_t graphicsQueueFamily = VK_NONE, presentQueueFamily = VK_NONE;
    std::unordered_set<std::string> extensions(request.Extensions.begin(), request.Extensions.end());
    if (request.TargetSurface != VK_NULL_HANDLE)
    {
        extensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevices[index], request.TargetSurface, &result.NumSurfaceFormats, nullptr);
        vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevices[index], request.TargetSurface, &result.NumPresentModes, nullptr);
        if (result.NumSurfaceFormats == 0 || result.NumPresentModes == 0)
        {
            return result;
        }
    }
    for (std::uint32_t i = 0; i < PhysicalDevicesInfo[index].Extensions.size(); i++)
    {
        extensions.erase(PhysicalDevicesInfo[index].Extensions[i].extensionName);
    }
    if (!extensions.empty())
    {
        return result;
    }

    /**/
    VkPhysicalDevice phDevice = PhysicalDevices[index];
    request.FindQueues(PhysicalDevicesInfo[index].QueueFamilies, [this, phDevice, &request](std::uint32_t i) 
    {
        VkBool32 canPresent = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(phDevice, i, request.TargetSurface, &canPresent);
        return (canPresent == VK_TRUE);
    });
    /**/

    for (std::uint32_t i = 0; i < PhysicalDevicesInfo[index].QueueFamilies.size(); i++)
    {
        if ((request.QueueFlagBits & VK_QUEUE_GRAPHICS_BIT) && (graphicsQueueFamily == VK_NONE))
        {
            if (PhysicalDevicesInfo[index].QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueueFamily = i;
            }
        }
        if (request.TargetSurface != VK_NULL_HANDLE && presentQueueFamily == VK_NONE)
        {
            VkBool32 canPresent = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevices[index], i, request.TargetSurface, &canPresent);
            if (canPresent == VK_TRUE)
            {
                presentQueueFamily = i;
            }
        }
    }
    result.IsValid = (graphicsQueueFamily != VK_NONE);
    result.GraphicsQueueFamily = graphicsQueueFamily;
    if (request.TargetSurface != VK_NULL_HANDLE)
    {
        result.IsValid = result.IsValid && (presentQueueFamily != VK_NONE);
        result.PresentQueueFamily = presentQueueFamily;
    }
    return result;
}

void VK::Instance::GetPhysicalDeviceInfo(std::uint32_t index)
{
    std::uint32_t count = 0;
    vkGetPhysicalDeviceProperties(PhysicalDevices[index], &PhysicalDevicesInfo[index].Properties);
    vkGetPhysicalDeviceFeatures(PhysicalDevices[index], &PhysicalDevicesInfo[index].Features);
    vkGetPhysicalDeviceMemoryProperties(PhysicalDevices[index], &PhysicalDevicesInfo[index].MemoryProperties);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[index], &count, nullptr);
    if (count > 0)
    {
        PhysicalDevicesInfo[index].QueueFamilies.resize(count);
        VkQueueFamilyProperties *target = PhysicalDevicesInfo[index].QueueFamilies.data();
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[index], &count, target);
    }
    vkEnumerateDeviceExtensionProperties(PhysicalDevices[index], nullptr, &count, nullptr);
    if (count > 0)
    {
        PhysicalDevicesInfo[index].Extensions.resize(count);
        VkExtensionProperties *target = PhysicalDevicesInfo[index].Extensions.data();
        vkEnumerateDeviceExtensionProperties(PhysicalDevices[index], nullptr, &count, target);
    }
}

#ifdef VK_DEBUG_ON
VKAPI_ATTR VkBool32 VKAPI_CALL OnError(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
    std::uint64_t object, std::size_t location, std::int32_t messageCode, const char *layerPrefix, const char *message,
    void *userData)
{
    return VK_FALSE;
}
#endif

VkResult VK::Instance::InstallDebugCallback()
{
#ifdef VK_DEBUG_ON
    VkDebugReportCallbackCreateInfoEXT cbInfo = {};
    cbInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    cbInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    cbInfo.pfnCallback = &OnError;
    return vkCreateDebugReportCallbackEXT(Handle, &cbInfo, nullptr, &OnErrorCallback);
#else
    return VK_SUCCESS;
#endif
}
