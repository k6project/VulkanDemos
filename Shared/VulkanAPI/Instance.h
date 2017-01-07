#pragma once

#include <vulkan/vulkan.h>

struct NativeWindow;

namespace VK
{

    struct DeviceRequest;

    class Device;

    class Instance
    {

    public:

        bool Init(const std::string &engineName);

        bool CreateSurface(const NativeWindow &window, VkSurfaceKHR &surface);

        bool CreateDevice(const DeviceRequest &request, Device &device);

        void Destroy(VkSurfaceKHR object);

        void Destroy();

    private:

        struct DeviceInfo
        {
            VkPhysicalDeviceProperties Properties;
            VkPhysicalDeviceFeatures Features;
            std::vector<VkQueueFamilyProperties> QueueFamilies;
            std::vector<VkExtensionProperties> Extensions;

        };

        struct DeviceMatch
        {
            std::uint32_t GraphicsQueueFamily;
            std::uint32_t PresentQueueFamily;
            std::uint32_t NumSurfaceFormats;
            std::uint32_t NumPresentModes;
            bool IsValid;
        };

        DeviceMatch MatchDevice(std::uint32_t index, const DeviceRequest &request);

        void GetPhysicalDeviceInfo(std::uint32_t index);

        VkResult InstallDebugCallback();

        #define VULKAN_DLL_EXPORT(proc)   PFN_##proc proc;
        #define VULKAN_API_GOBAL(proc)    PFN_##proc proc;
        #define VULKAN_API_INSTANCE(proc) PFN_##proc proc;
        #include "Instance.inl"

        std::vector<VkPhysicalDevice> PhysicalDevices;

        std::vector<DeviceInfo> PhysicalDevicesInfo;

        VkInstance Handle = VK_NULL_HANDLE;

        VkDebugReportCallbackEXT OnErrorCallback = VK_NULL_HANDLE;

    };

}
